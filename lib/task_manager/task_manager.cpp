#include "task_manager.h"
#include "queue.h"
#include "task.h"
#include "message.h"

#include <memory>
#include <string>
#include <vector>
#include <set>
#include <functional>

using namespace wlp;

// Create task_manager with a maximum of `ntasks`. All task IDs should fall within [0, ntasks - 1]
task_manager::task_manager(uint32_t ntasks) : 
	num_tasks(ntasks),
	task_list(std::vector<task *>(ntasks, nullptr)) {}

// Start all tasks
void task_manager::start_all() {
	if(started)
		return;
	started = true;
	for(auto it = task_list.begin(); it != task_list.end(); ++it) {
		task *t = *it;
		if(t != nullptr && !t->running) {
			t->thread = new std::thread(std::bind(&task_manager::call_start, this, t));
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
}

// Wait for all tasks to exit
void task_manager::wait_all() {
	for(auto it = task_list.begin(); it != task_list.end(); ++it) {
		task *t = *it;
		if(t != nullptr && t->running)
			t->thread->join();
	}
}

// Send msg to the task with id `dst`
void task_manager::send_msg(uint32_t dst, uint32_t src, message *msg) {
	if(dst >= num_tasks || task_list[dst] == nullptr) {
		return;
	}
	msg->src_task = src;
	msg->timestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	task_list[dst]->msg_queue.push(msg);
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