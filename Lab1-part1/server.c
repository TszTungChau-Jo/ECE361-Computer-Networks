/*
** server.c -- a datagram sockets "server" - working
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MAX_BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <UDP listen port>\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);
    int sockfd;
    struct sockaddr_in serverAddr, clientAddr;
    char buffer[MAX_BUFFER_SIZE];
    socklen_t addr_size;

    // Create UDP socket
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    // Configure settings in address struct
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    // Bind socket with address struct
    if(bind(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0){
        perror("error: Bind");
        exit(1);
    }

    addr_size = sizeof(clientAddr);

    printf("Start waiting for connections:~~~\n");

    while (1) {
        // Try to receive any incoming UDP datagram.
        ssize_t message_size = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&clientAddr, &addr_size);
        
        // check if nothing received
        if (message_size < 0) {
            perror("error: recvfrom");
            continue;
        }

        // add terminater the buffer 
        buffer[message_size] = '\0';

        // print out what we received
        printf("We just received: %s\n", buffer);

        // Check if the received message is "ftp"
        if (strcmp(buffer, "ftp") == 0) {
            strcpy(buffer, "yes");
        } else {
            strcpy(buffer, "no");
        }

        // Send the response back to the client
        if( sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *) &clientAddr, addr_size) < 0){
            perror("error: sendto");
            continue;
        }
    }

    return 0;
}
