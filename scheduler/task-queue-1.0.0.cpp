#include <utility>

#include "task-queue-1.0.0.h"

namespace DomiRaaf{namespace Scheduler{

        TaskQueue::task() {
            queue = std::vector<QueuedTask>();
        }

        void TaskQueue::enqueue(Task* task, int enqueue_time) {
            enqueue_time += task->interval;
            int index        = 0;
            while (index < queue.size() && queue[index].time < enqueue_time)
                index++;

            const QueuedTask queueTask{
                    .task=task,
                    .time=enqueue_time,
            };
            queue.insert(index, queueTask);
        }

        void TaskQueue::requeue(int interval) {
            queue[0].time += interval;
            for (int i=1; i<queue.size(); i++) {
                if (queue[i].time > queue[i-1].time)
                    break;
                std::swap(queue[i], queue[i-1]);
            }
        }

        void TaskQueue::dequeue() {
            queue.erase(0);
        }

        std::vector<QueuedTask>* TaskQueue::vector() {
            return &queue;
        }
    }}
