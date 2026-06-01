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
    int new_fd;
    int status;
    int bytes_sent;

    struct addrinfo *servinfo;
    struct addrinfo hints = {0};

    struct sockaddr_storage their_addr; // this is where information about incoming information will go
    socklen_t addr_len; // socklen_t = sizeof(sockaddr_storage)

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;


    if((status = getaddrinfo(NULL, "8000", &hints, &servinfo)) != 0) 
    {
        fprintf(stderr, "gai error: %s\n", gai_strerror(status));
        exit(1);
    }

    /* ai_protocol is inferred from the sock_type specified in the addrinfo hints */

    if ((socketfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1) {
        perror("socket");
        exit(1);
    }

    if (bind(socketfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        perror("bind");
        exit(1);
    }

    int yes = 1;
    setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);


    if(listen(socketfd, 5) == -1){
        perror("listen");
        exit(1);
    }

    if((new_fd = accept(socketfd, (struct sockaddr *)&their_addr, &addr_len)) == -1){
        perror("accept");
        exit(1);
    }

    char *msg = "Waleed was here!";
    int len = strlen(msg);

    if((bytes_sent = send(new_fd, msg, len, 0)) == -1){
        perror("send");
        exit(1);
    }


    freeaddrinfo(servinfo);

}