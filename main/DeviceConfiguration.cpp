#include "includes.h"

#include "DeviceConfiguration.h"

static const char* TAG = "DeviceConfiguration";

DeviceConfiguration::DeviceConfiguration() {
    uint8_t mac[6];

    ESP_ERROR_CHECK(esp_read_mac(mac, ESP_MAC_WIFI_STA));

    auto formattedMac = format("%02x-%02x-%02x-%02x-%02x-%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    auto endpoint = format(CONFIG_DEVICE_CONFIG_ENDPOINT, formattedMac.c_str());

    ESP_LOGI(TAG, "Getting device configuration from %s", endpoint.c_str());

    esp_http_client_config_t config = {
        .url = endpoint.c_str(),
        .timeout_ms = CONFIG_OTA_RECV_TIMEOUT,
    };

    string json;
    ESP_ERROR_CHECK(esp_http_download_string(config, json, 128 * 1024));

    auto root = cJSON_Parse(json.c_str());
    if (!root) {
        ESP_LOGE(TAG, "Failed to parse JSON");
        abort();
    }

    auto deviceNameItem = cJSON_GetObjectItemCaseSensitive(root, "deviceName");
    if (!cJSON_IsString(deviceNameItem) || !deviceNameItem->valuestring) {
        ESP_LOGE(TAG, "Cannot get deviceName property");
        abort();
    }

    _deviceName = deviceNameItem->valuestring;

    ESP_LOGI(TAG, "Device name: %s", _deviceName.c_str());

    auto deviceEntityIdItem = cJSON_GetObjectItemCaseSensitive(root, "deviceEntityId");
    if (!cJSON_IsString(deviceEntityIdItem) || !deviceEntityIdItem->valuestring) {
        ESP_LOGE(TAG, "Cannot get deviceEntityIdItem property");
        abort();
    }

    _deviceEntityId = deviceEntityIdItem->valuestring;

    ESP_LOGI(TAG, "Device entity ID: %s", _deviceEntityId.c_str());

    cJSON_Delete(root);
}