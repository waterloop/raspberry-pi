#ifndef task_h
#define task_h

#include <thread>
#include <vector>

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

		std::unique_ptr<message> recv_msg(uint64_t timeout = 0);

		uint32_t get_id();

		virtual void start() = 0;

	private:
		//task(const task &t); // deleted 
		//task &operator=(const task &t);

		uint32_t id = -1;
		bool running = false;
		task_manager *serv = nullptr;
		queue<message *> msg_queue;
		std::thread *thread;
	
	};
}

#endif

