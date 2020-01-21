#ifndef SIMPLE_LOGGER_H
#define SIMPLE_LOGGER_H

#include <registry.h>
#include <task_manager/task_manager.h>

#define LOG_INFO 3
#define LOG_WARNING 2
#define LOG_ERROR 1
#define LOG_

namespace wlp {
    struct logger_message : public message {
        logger_message() : message(MSG_LOG) {}
    };

    class task_logger : public task {
        void start();
    public:
        task_logger() : task(TASK_LOGGER, "logger") {}
    };
}

#endif
