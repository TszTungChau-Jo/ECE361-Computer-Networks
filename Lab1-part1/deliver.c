/*
** deliver.c -- a datagram sockets "client" - working
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

#define SERVERPORT "5000" // the port users will be connecting to

#define MAX_BUFFER_SIZE 100

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server address> <server port number>\n", argv[0]);
        exit(1);
    }

    struct sockaddr_in serverAddr;
    int sockfd;
    int serverPort = atoi(argv[2]);
    char buffer[1024];
    char message[1024];
    socklen_t addr_size;

    // Create UDP socket
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    // Configure settings in address struct
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    serverAddr.sin_addr.s_addr = inet_addr(argv[1]);

    addr_size = sizeof(serverAddr);

    // Input message from the user
    printf("Enter a message in the format 'ftp <file name>': ");
    fgets(buffer, sizeof(buffer), stdin);

    // Check
    char filename[MAX_BUFFER_SIZE];
    for( int i=4, j=0; i < MAX_BUFFER_SIZE && (buffer[i]!='\n');i++, j++){
        filename[j] = buffer[i];
    }
    
    printf("\nfilename: %s", filename);
    
    FILE *file;
    file = fopen(filename, "r");
    if(!file){
        perror("File does not exist");
        exit(1);
    }
    fclose(file);
    
    // Send the message to the server
    strcpy(buffer, "ftp");
    if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0){
        perror("error: sendto");
        exit(1);
    };

    // Receive the server's response
    ssize_t bytesReceived = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);

    if (bytesReceived < 0) {
        perror("error: Receive");
        exit(1);
    }

    // Null-terminate the received data
    buffer[bytesReceived] = '\0';

    if(strcmp(buffer, "yes")==0){
        printf("\nA file transfer can start.");
    } else {
        fprintf(stderr, "error: transfer");
        exit(1);
    }

    // Close the socket
    close(sockfd);
    return 0;
}

