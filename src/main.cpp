#include <stdio.h>
#include <canbus/canbus.h>
#include <task_manager/task_manager.h>

#include <unistd.h>
#include <thread>
#include <iostream>
#include "packet_decoder.h"
#include "tasks/tcp.h"

using namespace wlp;

class printTask : public task {
	void start() {
		while (true) {
			message *msg = new message(5);
			send_msg(0, msg);
			sleep(1);
		}
	}
};

// Mock out the behaviour of the CAN stuff 🥴🤭😳
class canMock : public task {
	void start() override {
		int i = 0;

		while (true) {
			std::shared_ptr<sensor_data> sample(new sensor_imu1accel_data());
			message *msg = new sensor_data_message(sample);
			send_msg(0, msg);
			sleep(1);

		}
	}
};

class checkTask : public task {
	void start() {	
		while(true) {
			std::unique_ptr<message> m = recv_msg();
			printf("received message.\n");
		}
	}
};

int main() {

	dup2(1, 2); // Redirects stderr to stdout
	/*
	task_manager s{3};
	printTask t1;
//	printTask2 t2;
	checkTask t3;
	s.add_task(0, &t1);
//	s.add_task(1, &t2);
	s.add_task(2, &t3);
	s.start_all();
	s.wait_all();
	*/
	task_manager s{2};
	tcp tcpTingz {};
	canMock canManz {};
	s.add_task(0, &tcpTingz);
	s.add_task(1, &canManz);
	printTask printyBoi;


	s.start_all();
	s.wait_all(); // 💀


	return 0;
}
