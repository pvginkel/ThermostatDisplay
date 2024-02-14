#pragma once

struct WifiConnectionState {
    bool connected;
    uint8_t errorReason;
};

class WifiConnection {
    Queue *_synchronizationQueue;
    EventGroupHandle_t _wifiEventGroup;
    CallbackArg<WifiConnectionState> _stateChanged;

    void eventHandler(esp_event_base_t eventBase, int32_t eventId, void *eventData);

public:
    WifiConnection(Queue *synchronizationQueue);

    void begin();
    void onStateChanged(CallbackArg<WifiConnectionState>::Func func, uintptr_t data = 0) {
        _stateChanged.set(func, data);
    }
};
