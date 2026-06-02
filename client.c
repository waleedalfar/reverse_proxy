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
    int bytes_sent;
    int buff_read;
    int status;

    struct addrinfo *servinfo;
    struct addrinfo hints = {0};

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

    if (connect(socketfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1){
        perror("connect");
        exit(1);
    }



    char *msg = "Waleed was here!";
    int len = strlen(msg);

    if((bytes_sent = send(socketfd, msg, len, 0)) == -1){
        perror("send");
        exit(1);
    }


    // receive from proxy the new string
    char buffer1[256];
    if((buff_read = recv(socketfd, buffer1, sizeof(buffer1), 0)) == -1){
        perror("receive");
        exit(1);
    }

    buffer1[buff_read] = '\0';
    printf("Received: %s\n", buffer1);

    close(socketfd);
    freeaddrinfo(servinfo);
}