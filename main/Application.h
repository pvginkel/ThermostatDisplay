#pragma once

#include "LoadingUI.h"
#include "MQTTConnection.h"
#include "OTAManager.h"
#include "Queue.h"
#include "ThermostatUI.h"
#include "WifiConnection.h"

class Application {
    lv_obj_t* _parent;
    WifiConnection _wifiConnection;
    MQTTConnection _mqttConnection;
    OTAManager _otaManager;
    LoadingUI* _loadingUI;
    ThermostatUI* _thermostatUI;
    Queue _queue;

public:
    Application();

    void begin(lv_disp_t* disp);
    void process();

private:
    void setupI2C();
    void setupFlash();
    void begin();
    void beginWifi();
    void beginWifiAvailable();
    void beginMQTT();
    void beginUI();
};
