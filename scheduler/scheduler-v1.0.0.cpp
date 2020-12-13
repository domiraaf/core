/**
 * Arduino scheduler. Supports deep sleep, wifi-enabled/disabled runs,
 * simulated multithreading and sequential execution.
 */

#include "scheduler-v1.0.0.h"

namespace DomiRaaf {
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

        typedef struct {
            Task* task;
            int time;
        } QueuedTask;

        auto tasks  = std::vector<Task*>();
        auto active = std::vector<Task*>();
        auto queue  = std::vector<QueuedTask>();

        void enqueue(Task* task, int last_time) {
            int enqueue_time = last_time + task->interval;
            int index        = 0;
            while (index < queue.size() && queue[index].time < enqueue_time)
                index++;

            const QueuedTask queueTask{
                    .task=task,
                    .time=enqueue_time,
            };
            queue.insert(index, queueTask);
        }

        void add(Task* task) {
            tasks.push_back(task);
        }

        void setup() {
            for (Task* task : tasks) {
                task->setup();
                enqueue(task, 0);
            }
        }

        void loop() {

        }
    }
}