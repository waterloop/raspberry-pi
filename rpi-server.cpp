#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>


// Prints msg to stderr and exits
//     with a non-zero exit code
void error(char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}


// argc = # of args passed in from terminal + 1
// argv[0] = path or name of executable
// arg[i] = ith argument passed in from terminal, i > 0
int main(int argc, char *argv[]) {

    // sockfd and newsockfd are file descriptors
    //     meaning they're indices in their socket's 
    //     file descriptor table. This table stores pointers
    //     to stdin, stdout, stderr.
    int sockfd, newsockfd;

    // portno is the port number the RPi will accept connections
    // clilen is the length of the l=clients address
    int portno, clilen, n;

    // will store values read from the socket into a cleared buffer
    char buffer[256];
    bzero(buffer,256);

    // The internet addresses of server (rpi) and client (desktop)
    struct sockaddr_in rpi_addr, desktop_addr;

    if (argc < 2) { // no arguments were passed in
        error("No port number provided");
    }
    portno = atoi(argv[1]); // store the port number as an int

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


    // NOW rpi_addr is properly initialized and is ready to
    //    be used.
    if (bind(sockfd, (struct sockaddr *) &rpi_addr, sizeof(rpi_addr)) < 0) {
        error("Could not bind socket to rpi address. Is it already in use?");
    }
    
    // the process will not wait for a connection up until more than 5 connections are put on hold
    listen(sockfd, 5);

    // Now that we passed the call a connection occured!!!
    //     We will need a NEW file descriptor now that the socket is accepting
    clilen = sizeof(desktop_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &desktop_addr, (socklen_t *) &clilen);
    if (newsockfd < 0) error("ERROR on accept");





    


}

