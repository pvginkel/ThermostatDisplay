#pragma once

#include "DeviceConfiguration.h"
#include "ThermostatState.h"

struct MQTTConnectionState {
    bool connected;
};

class MQTTConnection {
    static constexpr double DEFAULT_SETPOINT = 19;

    static string getDeviceId();

    const DeviceConfiguration &_configuration;
    Queue *_synchronizationQueue;
    string _deviceId;
    esp_mqtt_client_handle_t _client;
    string _modeTopic;
    string _localTemperatureTopic;
    string _humidityTopic;
    string _setpointTopic;
    string _heatingTopic;
    string _entityTopic;
    string _stateTopic;
    ThermostatState _state;
    CallbackArg<MQTTConnectionState> _stateChanged;
    Callback _thermostatStateChanged;

public:
    MQTTConnection(Queue *synchronizationQueue, const DeviceConfiguration &configuration);

    void begin();
    void onStateChanged(CallbackArg<MQTTConnectionState>::Func func, uintptr_t data = 0) {
        _stateChanged.set(func, data);
    }
    void onThermostatStateChanged(Callback::Func func, uintptr_t data = 0) { _thermostatStateChanged.set(func, data); }
    ThermostatState getState() { return _state; }
    void setState(ThermostatState &state, bool force = false);

private:
    void initializeState();
    void saveState(ThermostatState &state);
    void eventHandler(esp_event_base_t eventBase, int32_t eventId, void *eventData);
    void handleConnected();
    void handleData(esp_mqtt_event_handle_t event);
    void subscribe(string &topic);
    void publishDiscovery();
    void setOnline();
    const char *serializeMode(ThermostatMode value);
    ThermostatMode deserializeMode(string &value);
};
