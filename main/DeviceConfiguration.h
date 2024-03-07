#pragma once

class DeviceConfiguration {
private:
    string _deviceName;
    string _deviceEntityId;

public:
    DeviceConfiguration();

    const string& getDeviceName() const { return _deviceName; }
    const string& getDeviceEntityId() const { return _deviceEntityId; }
};
