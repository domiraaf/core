/**
 * Arduino scheduler. Supports deep sleep, wifi-enabled/disabled runs,
 * simulated multithreading and sequential execution.
 */

#include "scheduler.h"

#include "bitflag_enum.h"


namespace DomoRaaf {
    namespace Scheduler {
        SequencedTask::SequencedTask() {
            functionIndex = 0;
        }

        TaskReturn SequencedTask::loop() {
            auto op = seqFunctions[functionIndex]();
            if ((op & seqt_repeat) == 0)
                functionIndex = (functionIndex + 1) % seqFunctions.size();
            if (op & seqt_restart)
                functionIndex = 0;
            if (op & seqt_not_done)
                return task_not_done;
            functionIndex = 0;
            return task_done;
        }

        void SequencedTask::addStep(SeqTReturn (* step)()) {
            seqFunctions.push_back(step);
        }

        struct QueuedTask {
            Task* task;
            int time;
        };

        auto tasks  = std::vector<Task*>();
        auto active = std::vector<Task*>();
        auto queue  = std::vector<QueuedTask>();

        void add(Task* task) {
            tasks.push_back(task);
        }

        void setup() {
            for (Task* task : tasks) {
                task->setup();
                enqueue(task, 0)
            }
        }

        void enqueue(Task* task, int queue_time) {
            queue_time += task->interval;
            int index = -1;
            for (int i=0; i<queue.size(); i++){
                task* qtime = queue[i].time;
                if (ctime < queue_time)
                    index = i;
            }

            const QueuedTask queueTask;
            queueTask.task = task;
            queueTask.time = queue_time;
            queue.insert(i+1, queueTask);
        }

        void loop() {
            
        }
    }
}