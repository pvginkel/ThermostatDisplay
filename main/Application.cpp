#include "includes.h"

#include "Application.h"

static const char *TAG = "Application";

void Application::run() {
    setupFlash();
    begin();

    /*
    while (1) {
        loop();
    }
    */
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
    if (!wifiConnection.begin()) {
        ESP_LOGE(TAG, "Aborting startup, failed to connect to Wifi");
        return;
    }

    mqttConnection.begin();
}

void Application::loop() {}
