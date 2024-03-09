#include "includes.h"

#include "Application.h"

#include "Messages.h"
#include "driver/i2c.h"

static const char *TAG = "Application";

Application::Application(ESP_Panel &panel)
    : _panel(panel),
      _parent(nullptr),
      _wifiConnection(&_queue),
      _mqttConnection(nullptr),
      _loadingUI(nullptr),
      _thermostatUI(nullptr),
      _motionSensor(&_queue) {}

void Application::begin(lv_disp_t *disp) {
    ESP_LOGI(TAG, "Setting up motion sensor");

    _motionSensor.onTriggered(
        [](auto data) {
            ESP_LOGI(TAG, "Turning display on because of motion");
            ((Application *)data)->_panel.displayOn();
        },
        (uintptr_t)this);

    _motionSensor.begin();

    _parent = lv_disp_get_scr_act(disp);
    lv_obj_set_style_bg_color(_parent, lv_color_white(), 0);

    setupFlash();
    begin();
}

void Application::setupFlash() {
    ESP_LOGI(TAG, "Setting up flash");

    auto ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

void Application::begin() {
    ESP_LOGI(TAG, "Setting up loading UI");

    _loadingUI = new LoadingUI(_parent);

    _loadingUI->begin();
    _loadingUI->setTitle(Messages::connectingToWifi);
    _loadingUI->setState(LoadingUIState::Loading);
    _loadingUI->onRetryClicked([](uintptr_t) { esp_restart(); });
    _loadingUI->redraw();

    beginWifi();
}

void Application::beginWifi() {
    ESP_LOGI(TAG, "Connecting to WiFi");

    _wifiConnection.onStateChanged(
        [](auto state, uintptr_t data) {
            auto self = (Application *)data;

            if (!self->_loadingUI) {
                esp_restart();
            }

            if (state.connected) {
                self->beginWifiAvailable();
            } else {
                self->_loadingUI->setError(Messages::failedToConnect);
                self->_loadingUI->setState(LoadingUIState::Error);
                self->_loadingUI->redraw();
            }
        },
        (uintptr_t)this);

    _wifiConnection.begin();
}

void Application::beginWifiAvailable() {
    ESP_LOGI(TAG, "WiFi available, starting other services");

    auto err = _configuration.load();

    if (err != ESP_OK) {
        auto error = format(Messages::failedToRetrieveConfiguration, _configuration.getEndpoint().c_str());

        _loadingUI->setError(strdup(error.c_str()));
        _loadingUI->setState(LoadingUIState::Error);
        _loadingUI->redraw();

        return;
    }

    if (_configuration.getEnableOTA()) {
        _otaManager.begin();
    }

    beginMQTT();
}

void Application::beginMQTT() {
    _loadingUI->setTitle(Messages::connectingToHomeAssistant);
    _loadingUI->redraw();

    ESP_LOGI(TAG, "Connecting to MQTT / Home Assistant");

    _mqttConnection = new MQTTConnection(&_queue, _configuration);

    _mqttConnection->onStateChanged(
        [](auto state, uintptr_t data) {
            auto self = (Application *)data;

            if (!self->_loadingUI) {
                esp_restart();
            }

            if (state.connected) {
                delete self->_loadingUI;
                self->_loadingUI = nullptr;

                // Log the reset reason.
                auto resetReason = esp_reset_reason();
                self->_mqttConnection->logMessage(format("esp_reset_reason: %d", (int)resetReason));

                self->beginUI();
            } else {
                self->_loadingUI->setError(Messages::failedToConnect);
                self->_loadingUI->setState(LoadingUIState::Error);
                self->_loadingUI->redraw();
            }
        },
        (uintptr_t)this);

    _mqttConnection->begin();
}

void Application::beginUI() {
    ESP_LOGI(TAG, "Connected, showing UI");

    _thermostatUI = new ThermostatUI(_parent);

    _mqttConnection->onThermostatStateChanged(
        [](auto data) {
            ESP_LOGI(TAG, "Sending new state from MQTT to the thermostat");

            auto self = (Application *)data;

            auto newState = self->_mqttConnection->getState();
            auto setpointChanged = self->_thermostatUI->getState().setpoint != newState.setpoint;

            self->_thermostatUI->setState(newState);

            if (setpointChanged) {
                self->_panel.displayOn();
            }
        },
        (uintptr_t)this);

    _thermostatUI->onSetpointChanged(
        [](auto setpoint, auto data) {
            ESP_LOGI(TAG, "Sending new setpoint from the thermostat to MQTT");

            auto self = (Application *)data;

            auto state = self->_mqttConnection->getState();
            state.setpoint = setpoint;
            self->_mqttConnection->setState(state);
        },
        (uintptr_t)this);

    _thermostatUI->begin();
    _thermostatUI->setState(_mqttConnection->getState());
}

void Application::process() { _queue.process(); }
