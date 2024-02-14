#include "includes.h"

#include "Queue.h"

Queue::Queue() { _queue = xQueueCreate(32, sizeof(Task)); }

void Queue::enqueue(const Task &task) { xQueueSend(_queue, &task, portMAX_DELAY); }

void Queue::process() {
    while (uxQueueMessagesWaiting(_queue) > 0) {
        Task task;
        if (xQueueReceive(_queue, &task, 0) == pdTRUE) {
            task.run();
        }
    }
}
