/**
 * Arduino scheduler. Supports deep sleep, wifi-enabled/disabled runs,
 * simulated multithreading and sequential execution.
 */
#ifndef CORE_SCHEDULER_H
#define CORE_SCHEDULER_H

#include <vector>

namespace DomiRaaf {
    namespace Scheduler {

        /**
         * Options are implemented as a bitmask. Use | to combine.
         */
        enum TaskMode {
            /**
             * Default, simple task
             */
            default_mode = 0,

            /**
             * Use wifi
             */
            use_wifi = 1 << 0,

            /**
             * Delay task execution when needed, to limit interference with other tasks
             */
            long_task = 1 << 1
        };

        /**
         * Indicate whether the task's loop function is done or not.
         */
        enum TaskReturn {
            task_done, task_not_done
        };

        class Task {
        public:
            /**
             * The wake interval for the task, in ms
             */
            int interval;

            /**
             * The loop interval for the task, in ms
             */
            int loop_interval;

            /**
             * Options for the task. See the TaskMode documentation
             */
            TaskMode mode;

            /**
             * Setup function is executed once when the Wemos boots. May contain a longer running function, but this will
             * obviously influence boot time.
             */
            virtual void setup() = 0;

            /**
             * Small setup function to be executed when the task is woken up.
             */
            virtual void wake() = 0;

            /**
             * Loop function that is repeated until loop returns TaskReturn::done.
             * @return
             */
            virtual TaskReturn loop() = 0;

            /**
             * Function called just before letting the task sleep again.
             */
            virtual void finish() = 0;
        };

        /**
         * Indicate whether the task's loop functions are done or not.
         */
        enum SeqTReturn {
            /**
             * Either use seqt_done or seqt_not_done. This is the same as TaskReturn
             */
            seqt_done     = 0,
            seqt_not_done = 1,

            /**
             * Optionally, seqt_repeat or seqt_restart can be added.
             * Default behaviour: go to next function (using modulo execution).
             * seqt_repeat: in the next loop, execute the same function again
             * seqt_restart: go to the first function
             */
            seqt_repeat  = 1 << 1,
            seqt_restart = 1 << 2,
        };

        /**
         * SequencedTask supports an array (vector) of functions, where each vector is executed consecutively.
         * The called function can control the execution via it's return value (see SeqTReturn).
         */
        class SequencedTask : public Task {
        protected:
            std::vector<SeqTReturn (*)()> seqFunctions;
            int                           functionIndex;
        public:
            SequencedTask();

            TaskReturn loop();

            void addStep(SeqTReturn (* step)());
        };

        /**
         * Add a task to the scheduler
         * @param task
         */
        void add(Task* task);

        /**
         * Execute the setup
         */
        void setup();

        /**
         * Execute a loop
         */
        void loop();
    }
}

#endif
