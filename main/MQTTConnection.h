#pragma once

class MQTTConnection {
    static void eventHandlerThunk(void *eventHandlerArg, esp_event_base_t eventBase, int32_t eventId, void *eventData) {
        ((MQTTConnection *)eventHandlerArg)->eventHandler(eventBase, eventId, eventData);
    }

    static string getDeviceId();

    string _deviceId;
    esp_mqtt_client_handle_t _client;
    string _modeTopic;
    string _localTemperatureTopic;
    string _setpointTopic;
    string _stateTopic;

public:
    MQTTConnection();

    void begin();

private:
    void eventHandler(esp_event_base_t eventBase, int32_t eventId, void *eventData);
    void handleConnected();
    void handleData(esp_mqtt_event_handle_t event);
    void subscribe(string &topic);
    void publishDiscovery();
    void setOnline();
};
