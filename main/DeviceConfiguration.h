#pragma once

class DeviceConfiguration {
private:
    string _deviceName;
    string _deviceEntityId;
    string _endpoint;

public:
    DeviceConfiguration();
    esp_err_t load();

    const string& getDeviceName() const { return _deviceName; }
    const string& getDeviceEntityId() const { return _deviceEntityId; }
    const string& getEndpoint() const { return _endpoint; }
};
