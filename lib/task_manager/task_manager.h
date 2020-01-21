#ifndef task_manager_h
#define task_manager_h

#include <thread>
#include <stdint.h>

#include "task.h"
#include "message.h"

namespace wlp {
	class task_manager {

	public:

		task_manager(uint32_t ntasks);

		bool add_task(task *t);

		void start_all();
		void wait_all();

		void send_msg(uint32_t dst, uint32_t src, message *msg);

		void call_start(task *t);

	private:

		std::vector<task *> task_list;
		uint32_t num_tasks;
		bool started = false;
		std::mutex mut;
	};
}

#endif

