#ifndef message_h
#define message_h

#include <memory>
#include <stdint.h>
#include <string>

#define LOG_FATAL 0
#define LOG_ERR 1
#define LOG_WARN 2
#define LOG_INFO 3
#define LOG_DEBUG 4

namespace wlp {
	class message {
	public:
		uint32_t msg_type;
		uint32_t src_task;
		uint64_t timestamp;
	};

	class message_log : public message {
	public:
		message_log(uint8_t lvl, std::string msg) : log_level(lvl), message(msg) {
			msg_type = 0;
		}
		uint8_t log_level;
		std::string message;
	};
}

#endif

