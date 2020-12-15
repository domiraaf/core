#include "task-queue-1.0.0.h"

#include "scheduler-v1.0.0.h"

const unsigned int INT_2_7 = 128;
const unsigned int INT_2_8 = 256;
const unsigned int INT_2_16 = INT_2_8 * INT_2_8;
const unsigned int INT_2_31 = INT_2_16 * INT_2_8 * INT_2_7;

namespace DomiRaaf {
    namespace Scheduler {
        const int DS_SECOND = 1000 * 1000;

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

        auto tasks        = std::vector<Task*>();
        auto queue_sleep  = TaskQueue();
        auto queue_active = TaskQueue();

        void add(Task* task) {
            tasks.push_back(task);
        }

        void setup() {
            for (Task* task : tasks) {
                task->setup();
                queue_sleep.enqueue(task, 0);
            }
        }

        void awake_sleepers() {
            const auto time = millis();
            for (auto  qTask : *queue_sleep.vector()) {
                if (qTask.time > time)
                    break;
                queue_active.enqueue(qTask.task, 0);
                queue_sleep.requeue(qTask.task->interval);
                qTask.task->wake();
            }
        }

        void iterate_active() {
            if (queue_active.vector()->empty())
                return;
            auto     size = queue_active.vector()->size();
            for (int i    = 0; i < size; i++) {
                auto task   = queue_active.vector()->front().task;
                auto result = task->loop();

                if (result == task_done) {
                    task->finish();
                    queue_active.dequeue();
                } else
                    queue_active.requeue(task->loop_interval);

            }
        }

        bool next_run_wifi(unsigned int sleep_time) {
            for (auto task : *queue_active.vector()) {
                if (task.task->mode & use_wifi)
                    return true;
            }
            for (auto task : *queue_sleep.vector()) {
                unsigned int time = task.time - millis() - sleep_time - 100;
                if (time > INT_2_31)
                    return true;
            }
            return false;
        }

        void queue_front_sleep() {
            unsigned int current_time = millis();
            unsigned int wake_time = queue_active.vector()->empty()
                            ? queue_active.vector()->front().time
                            : queue_sleep.vector()->front().time;

            // millis() will overflow after about 50 days of runtime. However, because we do all computations
            // as unsigned 32 bit integers, we automatically do all computations modulo 2^32. The timer rollover
            // will not have any effect on the computation below.
            unsigned int sleep_time = wake_time - current_time;

            // However, as we are using unsigned ints, if wake_time < current_time, we will get an integer underflow.
            // This can happen when the runtime of some processes takes a bit too much time. Detect integer underflows
            // by handling all values > 2^31 like an underflow.
            if (sleep_time > INT_2_31)
                sleep_time = 1;

            bool enable_wifi = next_run_wifi(sleep_time);
            ESP.deepSleep(sleep_time * 1000, enable_wifi ? RF_DEFAULT ? RF_DISCONNECT);
        }

        void loop() {
            awake_sleepers();
            iterate_active();
            queue_front_sleep();
        }
    }
}