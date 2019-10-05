#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <signal.h>
#include <errno.h>
#include "gpiolib_addr.h"
#include "gpiolib_reg.h"
#include "gpiolib_reg.c"

typedef int bool;
#define true 1
#define false 0

#define left_motor_pin_control_1 16
#define left_motor_pin_control_2 19
#define right_motor_pin_control_1 15
#define right_motor_pin_control_2 18
#define left_motor_power 14
#define right_motor_power 17
#define forward_time 50000
#define back_time 50000
#define turn_time 50000

struct config {
    char roverName[64];
};

struct config get_config(char *fileName) 
{
        struct config configStruct;
        FILE *config = fopen (fileName, "r");

        if (config != NULL){ 
                char line[64];
                char *configLine;
                while(fgets(line, sizeof(line), config) != NULL){
                    configLine = strstr((char *)line,"=");
                    configLine += strlen("=");
                    memcpy(configStruct.roverName,configLine,strlen(configLine));
                }
        }
        fclose(config);//close file
        return configStruct;
}

void setToOutput(GPIO_Handle gpio, int pinNumber)
{
    //Check that the gpio is functional
    if (gpio == NULL) {
        printf("The GPIO has not been intitialized properly \n");
        return;
    }

    //Check that we are trying to set a valid pin number
    if (pinNumber < 2 || pinNumber > 27) {
        printf("Not a valid pinNumer \n");
        return;
    }

    //This will create a variable that has the appropriate select
    //register number. For more information about the registers
    //look up BCM 2835.
    int registerNum = pinNumber / 10;

    //This will create a variable that is the appropriate amount that
    //the 1 will need to be shifted by to set the pin to be an output
    int bitShift = (pinNumber % 10) * 3;

    //This is the same code that was used in Lab 2, except that it uses
    //variables for the register number and the bit shift
    uint32_t sel_reg = gpiolib_read_reg(gpio, GPFSEL(registerNum));
    sel_reg |= 1 << bitShift;
    gpiolib_write_reg(gpio, GPFSEL(1), sel_reg);
}

void outputOn(GPIO_Handle gpio, int pinNumber)
{
    gpiolib_write_reg(gpio, GPSET(0), 1 << pinNumber);
}

void outputOff(GPIO_Handle gpio, int pinNumber)
{
    gpiolib_write_reg(gpio, GPCLR(0), 1 << pinNumber);
}

void setCarPins(GPIO_Handle gpio)
{
    setToOutput(gpio, left_motor_pin_control_1);
    setToOutput(gpio, left_motor_pin_control_2);
    setToOutput(gpio, right_motor_pin_control_1);
    setToOutput(gpio, right_motor_pin_control_2);
    setToOutput(gpio, left_motor_power);
    setToOutput(gpio, right_motor_power);
}

void left_motor_on(GPIO_Handle gpio)
{
    outputOn(gpio, left_motor_power);
}
void right_motor_on(GPIO_Handle gpio)
{
    outputOn(gpio, right_motor_power);
}
void left_motor_off(GPIO_Handle gpio)
{
    outputOff(gpio, left_motor_power);
}
void right_motor_off(GPIO_Handle gpio)
{
    outputOff(gpio, right_motor_power);
}
void car_on(GPIO_Handle gpio)
{
    outputOn(gpio, left_motor_power);
    outputOn(gpio, right_motor_power);
}
void car_off(GPIO_Handle gpio)
{
    outputOff(gpio, left_motor_power);
    outputOff(gpio, right_motor_power);
}

void left_motor_forward(GPIO_Handle gpio)
{
    outputOn(gpio, left_motor_pin_control_1);
    outputOff(gpio, left_motor_pin_control_2);
}
void right_motor_forward(GPIO_Handle gpio)
{
    outputOn(gpio, right_motor_pin_control_1);
    outputOff(gpio, right_motor_pin_control_2);
}
void left_motor_back(GPIO_Handle gpio)
{
    outputOff(gpio, left_motor_pin_control_1);
    outputOn(gpio, left_motor_pin_control_2);
}
void right_motor_back(GPIO_Handle gpio)
{
    outputOff(gpio, right_motor_pin_control_1);
    outputOn(gpio, right_motor_pin_control_2);
}
void car_forward(GPIO_Handle gpio)
{
    left_motor_forward(gpio);
    right_motor_forward(gpio);
}
void car_back(GPIO_Handle gpio)
{
    left_motor_back(gpio);
    right_motor_back(gpio);
}
void car_left(GPIO_Handle gpio)
{
    left_motor_forward(gpio);
    right_motor_back(gpio);
}
void car_right(GPIO_Handle gpio)
{
    left_motor_back(gpio);
    right_motor_forward(gpio);
}
void stop(GPIO_Handle gpio)
{
    car_off(gpio);
}
void drive(GPIO_Handle gpio, int pause_time)
{
    car_on(gpio);
    usleep(pause_time);
    stop(gpio);
}

void error(const char* msg, GPIO_Handle gpio)
{
    perror(msg);
    gpiolib_free_gpio(gpio);
    exit(1);
}

int main(int argc, char* argv[])
{
    FILE* f; //THIS IS NEW
    f = fopen("serverError.log", "w");

    struct config configStruct;
    configStruct = get_config("config.conf");
    printf("Rover Name: %s\n", configStruct.roverName);
    
    GPIO_Handle gpio;
    gpio = gpiolib_init_gpio();
    if (gpio == NULL) {
        fprintf(f, "ERROR no GPIO available\n");
        perror("ERROR no GPIO availible");
        return -1;
    }
    setCarPins(gpio);
    bool stop = false;
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    

    if (argc < 2) {
        fprintf(f, "ERROR, no port provided\n");
        fprintf(stderr, "ERROR, no port provided\n");
        gpiolib_free_gpio(gpio);
        exit(1);
    }
    printf("Port number: %s\n", argv[1]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(f, "ERROR opening socket %s\n");
        error("ERROR opening socket", gpio);
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(f, "ERROR on binding\n");
        error("ERROR on binding", gpio);
    }
    fprintf(stderr, "socket binded to address\n");
    fprintf(stderr, "socket opened... waiting for client\n");
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
    if (newsockfd < 0) {
        fprintf(f, "ERROR on accpet\n");
        error("ERROR on accept", gpio);
    }
    fprintf(stderr, "client connected\n");
    //outputOff(gpio,led_pin_1);
    //outputOff(gpio,led_pin_2);
    while (stop == false) {
        bzero(buffer, 256);
        n = read(newsockfd, buffer, 255); //waiting to receive message

        if (n < 0) {
            fprintf(f, "ERROR reading from socket\n");
            error("ERROR reading from socket", gpio);
        }
        printf("Message received: %s\n", buffer);
        if (buffer[0] == 'w') {
            printf("Move forward \n");
            //led_1 = 1;
            car_left(gpio);
            drive(gpio, forward_time);
        }
        else if (buffer[0] == 'a') {
            printf("Move left \n");
            //led_2 = 1;
            car_back(gpio);
            drive(gpio, turn_time);
        }
        else if (buffer[0] == 's') {
            printf("Move back \n");
            //led_1 = 0;
            car_right(gpio);
            drive(gpio, back_time);
        }
        else if (buffer[0] == 'd') {
            //led_2 = 0;
            //printf("Move right \n");
            car_forward(gpio);
            drive(gpio, turn_time);
        }
        else if (buffer[0] == 'q') {
            stop = true;
            printf("quitting \n");
        }
        else {
            printf("Not a proper move command\n");
        }
        n = write(newsockfd, buffer, 18);
        if (n < 0) {
            error("ERROR writing to socket", gpio);
        }
    }
    close(newsockfd);
    close(sockfd);
    fclose(f);
    gpiolib_free_gpio(gpio);
    return 0;
}

