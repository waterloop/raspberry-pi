#ifndef task_h
#define task_h

#include <thread>
#include <vector>
#include <chrono>
#include <stdint.h>
#include <sstream>

#include "queue.h"
#include "message.h"

namespace wlp {
	class task_manager;
	class task {
		friend class task_manager;

	public:

		task();
		virtual ~task();

		void send_msg(uint32_t dst, message *msg);

		message *recv_msg();

		void log_flush(uint8_t log_level);

		virtual void start() = 0;

	protected:
		std::ostringstream log_buf;

	private:
		task(const task &t);
		task &operator=(const task &t);

		uint32_t id = -1;
		bool running = false;
		task_manager *serv = nullptr;
		queue<message *> msg_queue;
		std::thread *thread;
	
	};
}

#endif

