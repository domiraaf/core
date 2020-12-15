//
// Created by niek on 13-12-20.
//

#ifndef CORE_TASK_QUEUE_H
#define CORE_TASK_QUEUE_H

#include <vector>
#include "scheduler-v1.0.0.h"

namespace DomiRaaf {
    namespace Scheduler {

        typedef struct {
            Task* task;
            int time;
        } QueuedTask;

        class TaskQueue {
        private:
            std::vector<QueuedTask> queue;
        public:
            task();

            inline std::vector<QueuedTask>* vector();

            inline void enqueue(Task* task, int enqueue_time);

            inline void requeue(int interval);

            inline void dequeue();
        };
    }
}

#endif
