#ifndef TCP_H
#define TCP_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <canbus/canbus.h>
#include "../../src/packet_decoder.h"
#include <memory>
#include <task_manager/task_manager.h>
#include <registry.h>


namespace wlp { 
//class sensor_data_message;

class sensor_data_message : public message {
	public:
	std::shared_ptr<sensor_data> data; // 🤭
	sensor_data_message(std::shared_ptr<sensor_data> data): message{MSG_SENSOR_DATA}, data{data} {}
};


class tcp : public task {
public:
	tcp();
	tcp(const char *ifname);
	~tcp();

    // TO BE UPDATED TO PROPER LOGGING
    void error(const char *msg);
	void trace(const char *msg);

	// Initialize
	void start() override;

	// Send a TCP packet
    //     This will take a CAN frame as input
    bool send(char *message); 

	// Receive a TCP packet
	bool forwardToDesktop(std::shared_ptr<sensor_data> data);

	// Set CAN filter
	bool filter(can_filter *filters, size_t nfilters);
private:
	template<int level>
	void canbus_log(const char *msg);

	void canbus_errno(const char *msg);

	char buffer[2048];
	int sockfd, newsockfd, portno, clilen, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
};
}

#endif
