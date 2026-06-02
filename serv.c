#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>


int main() {

    int socketfd;
    int backsocket;
    int new_fd;
    int status;
    int bytes_sent;
    int buff_read;

    struct addrinfo *servinfo;
    struct addrinfo *serv2;
    struct addrinfo hints = {0};

    struct sockaddr_storage their_addr; // this is where information about incoming information will go
    socklen_t addr_len = sizeof(their_addr); // socklen_t = sizeof(sockaddr_storage)

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;


    if((status = getaddrinfo("localhost", "8000", &hints, &servinfo)) != 0) 
    {
        fprintf(stderr, "gai error: %s\n", gai_strerror(status));
        exit(1);
    }

    /* ai_protocol is inferred from the sock_type specified in the addrinfo hints */

    if ((socketfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1) {
        perror("socket");
        exit(1);
    }

    int yes = 1;
    setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

    if (bind(socketfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        perror("bind");
        exit(1);
    }

    freeaddrinfo(servinfo);


    if(listen(socketfd, 5) == -1){
        perror("listen");
        exit(1);
    }

    if((new_fd = accept(socketfd, (struct sockaddr *)&their_addr, &addr_len)) == -1){
        perror("accept");
        exit(1);
    }

    char buffer[256];
    if((buff_read = recv(new_fd, buffer, sizeof(buffer), 0)) == -1){
        perror("receive");
        exit(1);
    }

    buffer[buff_read] = '\0';
    printf("Received: %s\n", buffer);

    memset(&hints, 0, sizeof(hints));

    /* Create new socket to connect to backend */
    if((status = getaddrinfo("localhost", "3000", &hints, &serv2)) != 0) 
    {
        fprintf(stderr, "gai error: %s\n", gai_strerror(status));
        exit(1);
    }

    if ((backsocket = socket(serv2->ai_family, serv2->ai_socktype, serv2->ai_protocol)) == -1) {
        perror("socket");
        exit(1);
    }

    yes = 1;
    setsockopt(backsocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

    if (connect(backsocket, serv2->ai_addr, serv2->ai_addrlen) == -1){
        perror("connect");
        exit(1);
    }

    char *msg = buffer;
    int len = strlen(msg);

    if((bytes_sent = send(backsocket, msg, len, 0)) == -1){
        perror("send");
        exit(1);
    }

    // receive get answer from backsocket
    char buffer1[256];
    if((buff_read = recv(backsocket, buffer1, sizeof(buffer1), 0)) == -1){
        perror("receive");
        exit(1);
    }

    buffer1[buff_read] = '\0';
    //printf("Received: %s\n", buffer1);


    // send to client via the socketfd
    char *finalmsg = buffer1;
    int len1 = strlen(finalmsg);

    if((bytes_sent = send(new_fd, finalmsg, len1, 0)) == -1){
        perror("send");
        exit(1);
    }

    close(new_fd);
    close(socketfd);
}