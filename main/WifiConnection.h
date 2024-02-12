#pragma once

class WifiConnection {
    static constexpr int MAX_RECONNECT_TRIES = 5;

    static void wifiEventHandlerThunk(void *eventHandlerArg, esp_event_base_t eventBase, int32_t eventId,
                                      void *eventData) {
        ((WifiConnection *)eventHandlerArg)->wifiEventHandler(eventBase, eventId, eventData);
    }

    EventGroupHandle_t _wifiEventGroup;
    int _reconnectTries;
    Callback _connected;

    void wifiEventHandler(esp_event_base_t event_base, int32_t event_id, void *event_data);

public:
    WifiConnection();

    void begin();
};
