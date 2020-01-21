#include "simple_logger.h"

using namespace wlp;

void task_logger::start() {
    while(true) {
        std::unique_ptr<message> msg = recv_msg();
    }
}