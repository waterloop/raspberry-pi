#ifndef task_manager_h
#define task_manager_h

#include <thread>
#include <stdint.h>
#include <memory>
#include <string>
#include <vector>
#include <set>

#include "task.h"
#include "message.h"

namespace wlp {
	class task_manager {

	public:

		task_manager(uint32_t ntasks);

		bool add_task(uint32_t id, task *t);

		void start_all();
		void wait_all();

		void send_msg_task(uint32_t src, message *msg, task *t);
		void send_msg(uint32_t dst, uint32_t src, message *msg);

		void log(uint32_t src, uint8_t log_level, std::string log_msg);
		bool add_log_handler(uint32_t id);
		bool remove_log_handler(uint32_t id);

		void call_start(task *t);

	private:

		std::vector<task *> task_list;
		std::set<task *> task_logging;
		uint32_t num_tasks;
		bool started = false;
		std::mutex mut;
	};
}

#endif

