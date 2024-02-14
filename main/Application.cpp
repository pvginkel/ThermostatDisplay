#include "includes.h"

#include "Application.h"

#include "Messages.h"
#include "driver/i2c.h"

static const char *TAG = "Application";

Application::Application()
    : _parent(nullptr),
      _wifiConnection(&_queue),
      _mqttConnection(&_queue),
      _loadingUI(nullptr),
      _thermostatUI(nullptr) {}

void Application::begin(lv_disp_t *disp) {
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
                self->_loadingUI->setTitle(Messages::connectingToHomeAssistant);

                self->beginMQTT();
            } else {
                self->_loadingUI->setError(Messages::failedToConnect);
                self->_loadingUI->setState(LoadingUIState::Error);
            }

            self->_loadingUI->redraw();
        },
        (uintptr_t)this);

    _wifiConnection.begin();
}

void Application::beginMQTT() {
    ESP_LOGI(TAG, "Connecting to MQTT / Home Assistant");

    _mqttConnection.onStateChanged(
        [](auto state, uintptr_t data) {
            auto self = (Application *)data;

            if (!self->_loadingUI) {
                esp_restart();
            }

            if (state.connected) {
                delete self->_loadingUI;
                self->_loadingUI = nullptr;

                self->beginUI();
            } else {
                self->_loadingUI->setError(Messages::failedToConnect);
                self->_loadingUI->setState(LoadingUIState::Error);
                self->_loadingUI->redraw();
            }
        },
        (uintptr_t)this);

    _mqttConnection.begin();
}

void Application::beginUI() {
    ESP_LOGI(TAG, "Connected, showing UI");

    _thermostatUI = new ThermostatUI(_parent);

    _mqttConnection.onThermostatStateChanged(
        [](auto data) {
            ESP_LOGI(TAG, "Sending new state from MQTT to the thermostat");

            auto self = (Application *)data;

            self->_thermostatUI->setState(self->_mqttConnection.getState());
        },
        (uintptr_t)this);

    _thermostatUI->onSetpointChanged(
        [](auto setpoint, auto data) {
            ESP_LOGI(TAG, "Sending new setpoint from the thermostat to MQTT");

            auto self = (Application *)data;

            auto state = self->_mqttConnection.getState();
            state.setpoint = setpoint;
            self->_mqttConnection.setState(state);
        },
        (uintptr_t)this);

    _thermostatUI->begin();
    _thermostatUI->setState(_mqttConnection.getState());
}

void Application::process() { _queue.process(); }
