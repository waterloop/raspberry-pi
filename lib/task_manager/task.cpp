#include "task_manager.h"
#include "task.h"

#include <chrono>
#include <stdint.h>

using namespace wlp;

task::task() {
}

task::~task() {
	while(!msg_queue.empty()) {
		delete msg_queue.pop();
	}
}

void task::send_msg(uint32_t dst, message *msg) {
	serv->send_msg(dst, id, msg);
}

std::unique_ptr<message> task::recv_msg(uint64_t timeout) {
	return std::unique_ptr<message>(msg_queue.pop(timeout));
}

uint32_t task::get_id() {
	return id;
}

