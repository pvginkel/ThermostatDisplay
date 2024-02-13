#pragma once

struct WifiConnectionState {
    bool connected;
    uint8_t errorReason;
};

class WifiConnection {
    EventGroupHandle_t _wifiEventGroup;
    CallbackArgs<WifiConnectionState> _stateChanged;

    void eventHandler(esp_event_base_t eventBase, int32_t eventId, void *eventData);

public:
    void begin();
    void onStateChanged(CallbackArgs<WifiConnectionState>::Func func, uintptr_t data = 0) {
        _stateChanged.set(func, data);
    }
};
