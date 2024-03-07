#pragma once

class MotionSensor {
    Callback _triggered;
    QueueHandle_t _queue;
    Queue *_synchronizationQueue;

public:
    MotionSensor(Queue *synchronizationQueue);
    void begin();
    void onTriggered(Callback::Func func, uintptr_t data = 0) { _triggered.set(func, data); }

private:
    void taskHandler();
    void interruptHandler();
};
