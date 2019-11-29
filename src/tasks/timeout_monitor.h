#ifndef TIMEOUT_MONITOR_H
#define TIMEOUT_MONITOR_H

#include <unordered_map>
#include <set>

#include <task_manager/task_manager.h>
#include <registry.h>

namespace wlp {
    struct monitor_info {
        uint32_t id;
        uint64_t timeout; // timeout period in microseconds
        uint64_t expiry_time;
        uint32_t task_id;
        bool expired;
    };

    struct monitor_configure : public message {
        monitor_configure(uint32_t monitor_id, uint64_t timeout, uint32_t task_id) : 
            message(MSG_TIMEOUT_CONFIGURE), 
            monitor_id(monitor_id), 
            timeout(timeout),
            task_id(task_id) {}

        uint32_t monitor_id;
        uint64_t timeout;
        uint32_t task_id;
    };

    struct monitor_update : public message {
        monitor_update(uint32_t monitor_id) : 
            message(MSG_TIMEOUT_UPDATE), 
            monitor_id(monitor_id) {}

        uint32_t monitor_id;
    };

    struct monitor_expire : public message {
        monitor_expire(uint32_t monitor_id) : 
            message(MSG_TIMEOUT_EXPIRED), 
            monitor_id(monitor_id) {}

        uint32_t monitor_id;
    };

    class task_monitor : public task {
        std::unordered_map<uint32_t, monitor_info> monitors;
        std::set<std::pair<uint64_t, uint32_t>> timeout;
        void start();
    };
}

#define task_monitor_configure(monitor_id, timeout, task_id) \
    send_msg(TASK_MONITOR, new wlp::monitor_configure{monitor_id, timeout, task_id})
#define task_monitor_update(monitor_id) \
    send_msg(TASK_MONITOR, new wlp::monitor_update{monitor_id})
    
#endif