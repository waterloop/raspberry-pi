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

		task(uint32_t id, const char *const name);
		virtual ~task();

		void send_msg(uint32_t dst, message *msg);

		std::unique_ptr<message> recv_msg(uint64_t timeout = 0);

		virtual void start() = 0;

		const uint32_t id;
		const char *const name;

	private:
		//task(const task &t); // deleted 
		//task &operator=(const task &t);

		bool running = false;
		task_manager *serv = nullptr;
		queue<message *> msg_queue;
		std::thread *thread;
	
	};
}

#endif

