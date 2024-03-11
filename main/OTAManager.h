#pragma once

class OTAManager {
    esp_timer_handle_t _updateTimer;
    Callback _otaStart;

public:
    OTAManager();

    void begin();
    void onOTAStart(Callback::Func func, uintptr_t data = 0) { _otaStart.set(func, data); }

private:
    void updateCheck();
    bool installUpdate();
    bool parseHash(char* buffer, uint8_t* hash);
};
