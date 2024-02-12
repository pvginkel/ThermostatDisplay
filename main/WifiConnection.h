#pragma once

class WifiConnection {
    static constexpr int MAX_RECONNECT_TRIES = 5;

    static void eventHandlerThunk(void *eventHandlerArg, esp_event_base_t eventBase, int32_t eventId, void *eventData) {
        ((WifiConnection *)eventHandlerArg)->eventHandler(eventBase, eventId, eventData);
    }

    EventGroupHandle_t _wifiEventGroup;
    int _reconnectTries;

    void eventHandler(esp_event_base_t eventBase, int32_t eventId, void *eventData);

public:
    WifiConnection();

    bool begin();
};
