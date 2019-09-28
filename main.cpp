#include <stdio.h>
#include <canbus.h>
#include <task_manager.h>

using namespace wlp;

int main() {

	canbus bus("can0");

	if(!bus.begin()) {
		return 1;
	}

	uint32_t id;
	uint8_t data[8];
	uint8_t len;

	while(1) {
		if(!bus.recv(&id, data, &len)) {
			return 1;
		}
		for(int i = 0; i < 8; ++i) {
			printf("%02x ", data[i]);
		}
		printf("\n");
	}

	return 0;
}
