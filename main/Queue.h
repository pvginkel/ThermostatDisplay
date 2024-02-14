#pragma once

class Task {
public:
    using Func = void (*)(uintptr_t);

private:
    Func _func;
    uintptr_t _data;

public:
    Task() : _func(nullptr), _data((uintptr_t) nullptr) {}
    Task(Func func, uintptr_t data = 0) : _func(func), _data(data) {}

    void run() {
        if (_func) {
            _func(_data);
        }
    }
};

class Queue {
    QueueHandle_t _queue;

public:
    Queue();

    void enqueue(const Task& task);
    void process();
};