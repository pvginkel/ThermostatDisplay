#pragma once

#include "WifiConnection.h"

class Application {
    WifiConnection wifiConnection;

public:
    void run();

private:
    void setupFlash();
    void begin();
    void loop();
};
