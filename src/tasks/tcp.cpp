#include "tcp.h"
#include <iostream>
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "string.h"
#include <task_manager/task_manager.h>

/*
struct sensor_data {
    sensor_id sensor;

    uint8_t raw_data[8];
    uint8_t raw_data_len;

    virtual void decode() = 0;
};

*/



wlp::tcp::tcp() : task(TASK_TCP, "tcp") {
    printf("CTOR CALLED\n");
    //this->begin();
}

wlp::tcp::~tcp() {}

void wlp::tcp::error(const char *msg) {
    printf("[TCP] ERROR: %s\n", msg);
    exit(EXIT_FAILURE);
}

void wlp::tcp::trace(const char *msg) {
    printf("[TCP] TRACE: %s\n", msg);
}

void wlp::tcp::start() {
    int n = 0;
    // will store values read from the socket into a cleared buffer
    bzero(buffer,2048);
    
    // The internet addresses of server (rpi) and client (desktop)
    struct sockaddr_in rpi_addr, desktop_addr;

    //if (argc < 2) { // no arguments were passed in
    //    error("No port number provided");
    //}
    portno = 1234; // store the port number as an int

    // AF_INET indicates that the address domain will be 
    //     the internet domain. SOCK_STREAM and 0 ensure that
    //     the TCP protocol is used.
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("Could not open socket");

    // NOW CONFIGURE THE KEEPALIVE
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int)) < 0) {
        error("Could not set the keepalive option to yes");
    }


    // PREPARING THE SERVER's ADDRESS:
    bzero((char *) &rpi_addr, sizeof(rpi_addr));
    rpi_addr.sin_family = AF_INET; // this is an INTERNET address
    rpi_addr.sin_port = htons(portno); // ensure portno is stored in network byte order
    // When this code is run on the RPi INADDR_ANY 
    //     will store the IP address of the RPi.
    rpi_addr.sin_addr.s_addr = INADDR_ANY;
    printf(INADDR_ANY);


    // NOW rpi_addr is properly initialized and is ready to
    //    be used.
    if (bind(sockfd, (struct sockaddr *) &rpi_addr, sizeof(rpi_addr)) < 0) {
        error("Could not bind socket to rpi address. Is it already in use?");
    }
    
    trace("Bound socket to rpi address");
    
    
    // the process will not wait for a connection up until more than 5 connections are put on hold
    listen(sockfd, 5);
    trace("Connection established");
    // Now that we passed the call a connection occured!!!
    //     We will need a NEW file descriptor now that the socket is accepting
    clilen = sizeof(desktop_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &desktop_addr, (socklen_t *) &clilen);
    if (newsockfd < 0) error("ERROR on accept");

    /*filename logs.csv

    */

    std::ofstream data_log;

    bzero(buffer,2048);
    while (1) {
        std::unique_ptr<message> dataFromMessage = wlp::task::recv_msg();
        if (dataFromMessage->msg_type != MSG_SENSOR_DATA) continue;

        // WARNING: nice will be deleted as dataFromMessage soon as it goes out of scope. So after this iteration of while loop finishes, nice is deleted
        sensor_data_message *nice = static_cast<sensor_data_message*>(dataFromMessage.get());
        
        std::string raw_data_string;
        for (int a : nice->data->raw_data) {
            raw_data_string.append(std::to_string(a));
        }

        data_log.open("sensor_data.csv");
        data_log << std::to_string(dataFromMessage->timestamp) << ',';
        data_log << std::to_string(nice->data->sensor.sensor_type) << ',';
        data_log << std::to_string(nice->data->sensor.sensor_inst) << ',';
        data_log << std::to_string(nice->data->sensor.can_id) << ',';
        data_log << raw_data_string << ',';
        data_log.close();

        forwardToDesktop(nice->data);
    }

    //n = read(newsockfd,buffer,255);
    //if (n < 0) error("ERROR reading from socket");
    //printf("Here is the message: %s\n", buffer);

    //n = write(newsockfd,"I got your message",18);
     //if (n < 0) error("ERROR writing to socket");

}


std::string dataToJsonString(std::shared_ptr<sensor_data> &decoded_data_unique) {
    std::shared_ptr<sensor_data> decoded_data = std::move(decoded_data_unique);
    //Creating json document
    rapidjson::Document json;
    std::string stringVersionOfJson = "";

    json.SetObject();
    rapidjson::Document::AllocatorType& allocator = json.GetAllocator();
    size_t sz = allocator.Size();

    //Case 0 is the mcp6050 accelerometer
    if (decoded_data->sensor.sensor_type == 0)
    {
        auto data = std::dynamic_pointer_cast<sensor_imu1accel_data>(decoded_data);

        json.AddMember("id", decoded_data->sensor.sensor_type, allocator);
        
        //Creating a json array "data"
        /*rapidjson::Value dataArray(rapidjson::kArrayType);
        rapidjson::Value objectsToAddToDataArray(rapidjson::kObjectType);

        objectsToAddToDataArray.AddMember("x", data->x, allocator);
        objectsToAddToDataArray.AddMember("y", data->y, allocator);
        objectsToAddToDataArray.AddMember("z", data->z, allocator);


        dataArray.PushBack(objectsToAddToDataArray, allocator);*/
        
        rapidjson::Document dataObject;
        dataObject.SetObject();
        // must pass an allocator when the object may need to allocate memory
	    //rapidjson::Document::AllocatorType& allocator = dataObject.GetAllocator();
        dataObject.AddMember("x", data->x, allocator);
	    dataObject.AddMember("y", data->y, allocator);
        dataObject.AddMember("z", data->z, allocator);

        json.AddMember("data", dataObject, allocator);
    }
    //Case 1 is the mcp6050 gyroscope
    else if (decoded_data->sensor.sensor_type == 1)
    {
        auto data = std::dynamic_pointer_cast<sensor_imu1gyro_data>(decoded_data);

        json.AddMember("id", decoded_data->sensor.sensor_type, allocator);
        
        //Creating a json array "data"
        /*rapidjson::Value dataArray(rapidjson::kArrayType);
        rapidjson::Value objectsToAddToDataArray(rapidjson::kObjectType);

        objectsToAddToDataArray.AddMember("x", data->x, allocator);
        objectsToAddToDataArray.AddMember("y", data->y, allocator);
        objectsToAddToDataArray.AddMember("z", data->z, allocator);


        dataArray.PushBack(objectsToAddToDataArray, allocator);*/
        
        rapidjson::Document dataObject;
        dataObject.SetObject();
        // must pass an allocator when the object may need to allocate memory
	    //rapidjson::Document::AllocatorType& allocator = dataObject.GetAllocator();
        dataObject.AddMember("x", data->x, allocator);
	    dataObject.AddMember("y", data->y, allocator);
        dataObject.AddMember("z", data->z, allocator);

        json.AddMember("data", dataObject, allocator);
    }
    //Case 2 is the temp411
    else if (decoded_data->sensor.sensor_type == 2){
        auto data = std::dynamic_pointer_cast<sensor_temp1_data>(decoded_data);

        json.AddMember("id", decoded_data->sensor.sensor_type, allocator);
        json.AddMember("temp",data->temp, allocator);
    }
    //Case 3 is the temp2
    else if (decoded_data->sensor.sensor_type == 3){
        auto data = std::dynamic_pointer_cast<sensor_temp2_data>(decoded_data);

        json.AddMember("id", decoded_data->sensor.sensor_type, allocator);
        json.AddMember("temp",data->temp, allocator);
    }
    //Case 4 is the photo2
    else if (decoded_data->sensor.sensor_type == 4){
        auto data = std::dynamic_pointer_cast<sensor_photo2_data>(decoded_data);

        json.AddMember("id", decoded_data->sensor.sensor_type, allocator);
        json.AddMember("counter",data->count, allocator);

    }
    //Case 5 is the hall1
    else if (decoded_data->sensor.sensor_type == 5){
        auto data = std::dynamic_pointer_cast<sensor_hall1_data>(decoded_data);

        json.AddMember("id", decoded_data->sensor.sensor_type, allocator);
        json.AddMember("counter",data->strength, allocator);

    //Case 6 is the piezo1
    }
    else if (decoded_data->sensor.sensor_type == 6){
        auto data = std::dynamic_pointer_cast<sensor_piezo1_data>(decoded_data);

        json.AddMember("id", decoded_data->sensor.sensor_type, allocator);
        json.AddMember("counter",data->v, allocator);
    }
    else
        stringVersionOfJson = "error";

    rapidjson::StringBuffer buffer;

    buffer.Clear();

    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json.Accept(writer);
    stringVersionOfJson = buffer.GetString();

    return stringVersionOfJson;
}

bool wlp::tcp::forwardToDesktop(std::shared_ptr<sensor_data> data) {
    std::string dataJsonString = dataToJsonString(data);
    
    bzero(buffer,2048);
    sprintf(buffer, "%s", dataJsonString.c_str());
    printf("RECV CALLED%s\n", buffer);
    n = write(newsockfd, buffer, 255);

    if (n < 0) error("ERROR writing to socket");

}



/*int main() {
    wlp::tcp *tcpTingz = new wlp::tcp();
    sensor_imu1accel_data *sampleData = new sensor_imu1accel_data();
    sampleData->x = 1;
    sampleData->y = 2;
    sampleData->z = 14;

    std::shared_ptr<sensor_data> sample = std::shared_ptr<sensor_data>(sampleData);
    //tcpTingz->recv(sample);
    return 0;
}
*/