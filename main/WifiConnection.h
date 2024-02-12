#pragma once

class WifiConnection {
    static constexpr int MAX_RECONNECT_TRIES = 5;

    EventGroupHandle_t _wifiEventGroup;
    int _reconnectTries;

    void eventHandler(esp_event_base_t eventBase, int32_t eventId, void *eventData);

public:
    WifiConnection();

    bool begin();
};
