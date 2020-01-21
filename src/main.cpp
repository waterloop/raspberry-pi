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
public:
	printTask() : task(2, "printTask") {}
	void start() {
		while (true) {
			message *msg = new message(5);
			send_msg(0, msg);
			sleep(1);
		}
	}
};

// Mock out the behaviour of the CAN stuff 
class canMock : public task {
public:
	canMock() : task(50, "canMock") {}
	void start() override {
		int i = 0;

		while (true) {
			auto nice = new sensor_imu1accel_data();
			nice->x = i;
			nice->y = i;
			nice->z = i;

			std::shared_ptr<sensor_data> sample(nice);
			message *msg = new sensor_data_message(sample);
			send_msg(TASK_TCP, msg);
			i ++;
			sleep(1);
		}
	}
};

class checkTask : public task {
public:
	checkTask() : task(0, "checkTask") {}
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
	task_manager s{100};
	tcp tcpTingz {};
	canMock canManz {};
	s.add_task(&tcpTingz);
	s.add_task(&canManz);
	//printTask printyBoi;

	//for testing
	sensor_imu1accel_data *sampleData = new sensor_imu1accel_data();
    sampleData->x = 1;
    sampleData->y = 2;
    sampleData->z = 14;

    std::shared_ptr<sensor_data> sample = std::shared_ptr<sensor_data>(sampleData);
    //tcpTingz->recv(sample);

	s.start_all();
	s.wait_all(); //


	return 0;
}
