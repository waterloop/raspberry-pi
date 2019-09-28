#include "task_manager.h"
#include "task.h"

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

message *task::recv_msg() {
	return msg_queue.pop();
}

void task::log_flush(uint8_t log_level) {
	serv->log(id, log_level, log_buf.str());
	log_buf.str("");
}

