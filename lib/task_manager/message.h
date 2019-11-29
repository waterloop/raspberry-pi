#ifndef message_h
#define message_h

#include <memory>
#include <stdint.h>
#include <string>

namespace wlp {
	struct message {
		message(uint32_t msg_type) : msg_type(msg_type) {}
		virtual ~message() {}
		uint32_t msg_type;
		uint32_t src_task;
		uint64_t timestamp;
	};
}

#endif

