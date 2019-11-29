#include <unordered_map>
#include <set>
#include <utility>
#include <chrono>
#include <ctime>

#include "timeout_monitor.h"

using namespace wlp;

void task_monitor::start() {
    while (true) {
        // recv messages:
        //     monitor_configure: add to monitors and timeout list
        //     monitor_update: update a monitor and reset its timeout
        // send messages:
        //     monitor_expired: send a message to notify task_id about timeout expiry
        uint64_t current_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        std::unique_ptr<message> msg_ptr = recv_msg(monitors.empty() ? 0 : monitors.begin()->second.expiry_time - current_time);
        message *msg = msg_ptr.get();
        if (msg != nullptr) {
            if (msg->msg_type == MSG_TIMEOUT_UPDATE) {
                // update a monitor: the expiry timer on a monitor is reset
                uint32_t id = static_cast<monitor_update*>(msg)->monitor_id;
                monitor_info &mi = monitors[id];
                timeout.erase({mi.expiry_time, id});
                mi.expiry_time = current_time + mi.timeout;
                timeout.insert({mi.expiry_time, id});
                if(mi.expired) {
                    mi.expired = false;
                    // TODO send unexpire maybe?
                    // NOTIFY SOMETHING!!!!!!!!!!!!!!!!!!!!!!!!!!!
                }
            } else if (msg->msg_type == MSG_TIMEOUT_CONFIGURE) {
                // configure a monitor: change properties of a monitor.
                //     if a monitor already exists, its settings are overwritten
                monitor_configure* cm = static_cast<monitor_configure*>(msg);
                if(monitors.find(cm->monitor_id) != monitors.end()) {
                    // if monitor already exists, remove it so we can update it
                    timeout.erase({monitors[cm->monitor_id].expiry_time, cm->monitor_id});
                }
                monitors[cm->monitor_id] = {cm->monitor_id, cm->timeout, current_time + cm->timeout, cm->task_id, false};
                timeout.insert({current_time + cm->timeout, cm->monitor_id});
            }
        }
        // time out
        while (!timeout.empty() && current_time > timeout.begin()->first) {
            // pop out all tasks that have expired
            uint32_t id = timeout.begin()->second;
            monitor_info &mi = monitors[id];
            timeout.erase(timeout.begin());
            // update their expiry time to int_max, so it's at the end
            mi.expiry_time = UINT64_MAX;
            timeout.insert({mi.expiry_time, id});
            if(!mi.expired) {
                // notify expiry if this monitor timeout just expired
                mi.expired = true;
                monitor_expire* em = new monitor_expire(id);
                send_msg(mi.task_id, em);
            }
        }
    }
};
