#pragma once

#include "ESP_Panel.h"
#include "LoadingUI.h"
#include "MQTTConnection.h"
#include "MotionSensor.h"
#include "OTAManager.h"
#include "Queue.h"
#include "ThermostatUI.h"
#include "WifiConnection.h"

class Application {
    ESP_Panel& _panel;
    lv_obj_t* _parent;
    WifiConnection _wifiConnection;
    MQTTConnection* _mqttConnection;
    OTAManager _otaManager;
    LoadingUI* _loadingUI;
    ThermostatUI* _thermostatUI;
    Queue _queue;
    DeviceConfiguration _configuration;
    MotionSensor _motionSensor;

public:
    Application(ESP_Panel& panel);

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
