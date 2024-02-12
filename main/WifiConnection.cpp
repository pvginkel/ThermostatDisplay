#include "includes.h"

#include "WifiConnection.h"

static const char *TAG = "WifiConnection";

// The event group allows multiple bits for each event, but we only care about
// two events:
//
// - We are connected to the AP with an IP.
// - We failed to connect after the maximum amount of retries
//
constexpr EventBits_t WIFI_CONNECTED_BIT = BIT0;
constexpr EventBits_t WIFI_FAIL_BIT = BIT1;

WifiConnection::WifiConnection() : _reconnectTries(0) {}

void WifiConnection::begin() {
    _wifiEventGroup = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifiEventHandlerThunk, this,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifiEventHandlerThunk, this,
                                                        &instance_got_ip));

    wifi_config_t wifiConfig = {
        .sta =
            {
                .ssid = CONFIG_WIFI_SSID,
                .password = CONFIG_WIFI_PASSWORD,

                // Authmode threshold resets to WPA2 as default if password
                // matches WPA2 standards (pasword len => 8). If you want to
                // connect the device to deprecated WEP/WPA networks, Please set
                // the threshold value to WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK and
                // set the password with length and format matching to
                // WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK standards.

                .threshold = {.authmode = WIFI_AUTH_WPA2_PSK},
                .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
                .sae_h2e_identifier = "",
            },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifiConfig));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Finished setting up WiFi");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT)
     * or connection failed for the maximum number of re-tries (WIFI_FAIL_BIT).
     * The bits are set by event_handler() (see above) */
    auto bits =
        xEventGroupWaitBits(_wifiEventGroup, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence
     * we can test which event actually happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "Connected to AP SSID: %s password: %s", wifiConfig.sta.ssid, wifiConfig.sta.password);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID: %s, password: %s", wifiConfig.sta.ssid, wifiConfig.sta.password);
    } else {
        ESP_LOGE(TAG, "Unexpected event");
    }
}

void WifiConnection::wifiEventHandler(esp_event_base_t eventBase, int32_t eventId, void *eventData) {
    if (eventBase == WIFI_EVENT && eventId == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "Connecting to AP");
        esp_wifi_connect();
    } else if (eventBase == WIFI_EVENT && eventId == WIFI_EVENT_STA_DISCONNECTED) {
        auto event = (wifi_event_sta_disconnected_t *)eventData;

        ESP_LOGI(TAG, "Disconnected from AP, reason %d", event->reason);

        if (_reconnectTries < MAX_RECONNECT_TRIES) {
            ESP_LOGI(TAG, "Retry to connect to the AP");
            esp_wifi_connect();
            _reconnectTries++;
        } else {
            ESP_LOGI(TAG, "Failed to connect to AP");
            xEventGroupSetBits(_wifiEventGroup, WIFI_FAIL_BIT);
        }
    } else if (eventBase == IP_EVENT && eventId == IP_EVENT_STA_GOT_IP) {
        auto event = (ip_event_got_ip_t *)eventData;

        ESP_LOGI(TAG, "Got ip:" IPSTR, IP2STR(&event->ip_info.ip));

        _reconnectTries = 0;

        xEventGroupSetBits(_wifiEventGroup, WIFI_CONNECTED_BIT);
    }
}
