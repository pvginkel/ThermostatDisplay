#pragma once

#include "DeviceConfiguration.h"

class LogManager {
    static LogManager* _instance;
    static char* _buffer;

    vprintf_like_t _defaultLogHandler;
    SemaphoreHandle_t _lock;
    vector<char*> _messages;
    const DeviceConfiguration* _configuration;
    esp_timer_handle_t _logTimer;

    static int logHandler(const char* message, va_list va);

public:
    LogManager();

    void begin();
    void setConfiguration(const DeviceConfiguration& configuration);

private:
    void uploadLogs();
    void startTimer();
};
