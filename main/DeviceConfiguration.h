#pragma once

class DeviceConfiguration {
private:
    static constexpr auto DEFAULT_ENABLE_OTA = true;

    string _deviceName;
    string _deviceEntityId;
    string _endpoint;
    bool _enableOTA;

public:
    DeviceConfiguration();
    esp_err_t load();

    const string& getEndpoint() const { return _endpoint; }
    const string& getDeviceName() const { return _deviceName; }
    const string& getDeviceEntityId() const { return _deviceEntityId; }
    bool getEnableOTA() const { return _enableOTA; }
};
