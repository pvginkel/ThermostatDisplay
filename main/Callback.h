#pragma once

#include "Queue.h"

class Callback {
public:
    using Func = void (*)(uintptr_t);

private:
    Func _func;
    uintptr_t _data;

public:
    Callback() : _func(nullptr), _data((uintptr_t) nullptr) {}

    void set(Func func, uintptr_t data) {
        _func = func;
        _data = data;
    }

    bool call() {
        if (_func) {
            _func(_data);
            return true;
        }
        return false;
    }

    void queue(Queue* queue) {
        queue->enqueue(Task([](auto data) { ((Callback*)data)->call(); }, (uintptr_t)this));
    }
};

template <typename Arg>
class CallbackArg {
    static_assert((std::is_trivially_copyable<Arg>::value), "Arg must be trivially copyable");

public:
    using Func = void (*)(Arg, uintptr_t);

private:
    struct Payload {
        CallbackArg<Arg>* self;
        Arg arg;

        Payload(CallbackArg<Arg>* self, Arg arg) : self(self), arg(arg) {}
    };

    Func _func;
    uintptr_t _data;

public:
    CallbackArg() : _func(nullptr), _data((uintptr_t) nullptr) {}

    void set(Func func, uintptr_t data) {
        _func = func;
        _data = data;
    }

    bool call(Arg arg) {
        if (_func) {
            _func(arg, _data);
            return true;
        }
        return false;
    }

    void queue(Queue* queue, Arg arg) {
        queue->enqueue(Task(
            [](uintptr_t data) {
                const auto payload = (Payload*)data;

                payload->self->call(payload->arg);

                delete payload;
            },
            (uintptr_t) new Payload(this, arg)));
    }
};
