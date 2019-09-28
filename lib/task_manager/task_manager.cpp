#include "task_manager.h"
#include "queue.h"
#include "task.h"
#include "message.h"

using namespace wlp;

void start_thread(std::pair<task_manager *, task *> *v) {
	v->first->call_start(v->second);
	delete v;
}

// Create task_manager with a maximum of `ntasks`. All task IDs should fall within [0, ntasks - 1]
task_manager::task_manager(uint32_t ntasks) : num_tasks(ntasks) {
	task_list = std::vector<task *>(ntasks, nullptr);
}

// Start all tasks
void task_manager::start_all() {
	if(started)
		return;
	started = true;
	for(auto it = task_list.begin(); it != task_list.end(); ++it) {
		task *t = *it;
		if(t != nullptr && !t->running) {
			t->thread = new std::thread(start_thread, new std::pair<task_manager *, task *>(this, t));
			t->running = true;
		}
	}
}

// Starts up a task
void task_manager::call_start(task *t) {
	fprintf(stderr, "[task_manager] task[%d] running\n", t->id);
	t->running = true;
	t->start();
	t->running = false;
	fprintf(stderr, "[task_manager] task[%d] exited\n", t->id);

	std::unique_lock<std::mutex> lock(mut);
	task_list[t->id] = nullptr;
	task_logging.erase(t);
}

// Wait for all tasks to exit
void task_manager::wait_all() {
	for(auto it = task_list.begin(); it != task_list.end(); ++it) {
		task *t = *it;
		if(t != nullptr && t->running)
			t->thread->join();
	}
}

// Send msg to the specified `task`
void task_manager::send_msg_task(uint32_t src, message *msg, task *t) {
	msg->src_task = src;
	msg->timestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	t->msg_queue.push(msg);
}

// Send msg to the task with id `dst`
void task_manager::send_msg(uint32_t dst, uint32_t src, message *msg) {
	if(dst >= num_tasks || task_list[dst] == nullptr) {
		return;
	}
	send_msg_task(src, msg, task_list[dst]);
}

// Log `log_msg` to all logging tasks
void task_manager::log(uint32_t src, uint8_t log_level, std::string log_msg) {
	std::unique_lock<std::mutex> lock(mut);
	for(auto it = task_logging.begin(); it != task_logging.end(); ++it) {
		message_log *msg = new message_log(log_level, log_msg);
		send_msg_task(src, msg, *it);
	}
}

// Adds `t` to the task list, with the specified `id`
// All task must be added before starting the tasks
// Returns true on successful, false otherwise
bool task_manager::add_task(uint32_t id, task *t) {
	if(id >= num_tasks || task_list[id] != nullptr || started) {
		return false;
	}
	t->id = id;
	t->serv = this;
	task_list[id] = t;
	return true;
}

/**
 * Sets the specified task as a logging handler, and will allow it to receive log messages generated from all other tasks. After the handler is added, `message_log` messages will be added to this task's message queue when a log is generated. Other messages will be delivered normally.
 *
 * The task specified must be added to the task_manager with `add_task` before hand, and must not have exited.
 *
 * @param id - The task id of the task 
 * @return true if the handler is successfully added, false otherwise
 *
 */ 
bool task_manager::add_log_handler(uint32_t id) {
	std::unique_lock<std::mutex> lock(mut);
	if(id >= num_tasks || task_list[id] == nullptr || started) {
		return false;
	}
	return task_logging.insert(task_list[id]).second;
}

/**
 * Removes the specified task as a logging handler. This task will no longer receive log messages.
 *
 * The task specified must be added to the task_manager with `add_task` before hand, and must not have exited.
 *
 * @param id - The task id of the task 
 * @return true if the handler is successfully removed, false otherwise
 *
 */ 
bool task_manager::remove_log_handler(uint32_t id) {
	std::unique_lock<std::mutex> lock(mut);
	if(id >= num_tasks || task_list[id] == nullptr || started) {
		return false;
	}
	return task_logging.erase(task_list[id]) != 0;
}

