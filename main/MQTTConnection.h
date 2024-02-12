#pragma once

enum class ThermostatRunningState { Unknown, True, False };
enum class ThermostatMode { Off, Heat };

struct ThermostatState {
    double localTemperature;
    double humidity;
    double setpoint;
    ThermostatMode mode;
    ThermostatRunningState state;

    bool equals(ThermostatState &other) {
        return localTemperature == other.localTemperature && humidity == other.humidity && setpoint == other.setpoint &&
               mode == other.mode && state == other.state;
    }

    bool valid() {
        return !isnan(localTemperature) && !isnan(humidity) && !isnan(setpoint) &&
               state != ThermostatRunningState::Unknown;
    }
};

class MQTTConnection {
    static void eventHandlerThunk(void *eventHandlerArg, esp_event_base_t eventBase, int32_t eventId, void *eventData) {
        ((MQTTConnection *)eventHandlerArg)->eventHandler(eventBase, eventId, eventData);
    }

    static string getDeviceId();

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
    Callback _stateChanged;

public:
    MQTTConnection();

    void begin();

private:
    void initializeState();
    void saveState(ThermostatState &state);
    void eventHandler(esp_event_base_t eventBase, int32_t eventId, void *eventData);
    void handleConnected();
    void handleData(esp_mqtt_event_handle_t event);
    void subscribe(string &topic);
    void publishDiscovery();
    void setOnline();
    ThermostatState getState() { return _state; }
    void setState(ThermostatState &state, bool force = false);
    void onStateChanged(Callback::Func func, uintptr_t data = 0) { _stateChanged.set(func, data); }
    const char *serializeMode(ThermostatMode value);
    ThermostatMode deserializeMode(string &value);
};
