#pragma once

class OTAManager {
    esp_timer_handle_t _updateTimer;

public:
    OTAManager();

    void begin();

private:
    void updateCheck();
    bool installUpdate();
    bool parseHash(char* buffer, uint8_t* hash);
};
