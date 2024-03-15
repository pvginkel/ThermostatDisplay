#pragma once

class Mutex;

class MutexLock {
    Mutex* _mutex;

    MutexLock(Mutex* mutex);
    MutexLock(const MutexLock&) = delete;
    MutexLock& operator=(const MutexLock&) = delete;
    MutexLock(MutexLock&&) = delete;
    MutexLock& operator=(MutexLock&&) = delete;

public:
    ~MutexLock();

    friend class Mutex;
};

class Mutex {
    SemaphoreHandle_t _lock;

public:
    Mutex();
    ~Mutex();

    MutexLock take(TickType_t xTicksToWait);

private:
    void give();

    friend class MutexLock;
};
