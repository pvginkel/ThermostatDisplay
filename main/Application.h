#pragma once

#include "MQTTConnection.h"
#include "WifiConnection.h"

class Application {
    WifiConnection wifiConnection;
    MQTTConnection mqttConnection;

public:
    void run();

private:
    void setupFlash();
    void begin();
    void loop();
};
