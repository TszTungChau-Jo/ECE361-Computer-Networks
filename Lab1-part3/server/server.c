#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

char* itoa(int num) {
    char* str = (char*)malloc(12); // Maximum size for a 32-bit integer in base 10
    if (str == NULL) {
        return NULL; // Memory allocation failed
    }

    sprintf(str, "%d", num);
    return str;
}

#define INTERNET_PROTOCAL 0
#define MAX_PENDING 5
#define MAX_LINE 256
#define PACKET_SIZE 1000

typedef struct{
  char array[PACKET_SIZE+255+64];
  int len;
} charArray;

typedef int SockDescriptor;

typedef struct{
  SockDescriptor sockfd;
  struct sockaddr_in address;
  socklen_t address_len;
} SocketObj;

struct packet {
  unsigned int total_frag;
  unsigned int frag_no;
  unsigned int size;
  char filename[MAX_LINE];
  char filedata[PACKET_SIZE];
};

SocketObj make_udp_socket(int port) {//////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /* build address data structure */
  SocketObj new_socket;
  bzero((char * ) & new_socket.address, sizeof(new_socket.address));
  
  /* set address atrributes */
  new_socket.address.sin_family = AF_INET;
  new_socket.address.sin_addr.s_addr = INADDR_ANY;
  new_socket.address.sin_port = htons(port);

  new_socket.sockfd = socket(PF_INET, SOCK_DGRAM, INTERNET_PROTOCAL);
  
  /* setup passive open */
  if (new_socket.sockfd < 0) {
    perror("server : socket cannot be created");
    exit(1);
  }

  if ((bind(new_socket.sockfd, (struct sockaddr * ) & new_socket.address, sizeof(new_socket.address))) < 0) {  // assigns the previously set address to the socket
    perror("server : bind");
    exit(1);
  }
  
  /* Set address length */
  new_socket.address_len = sizeof(new_socket.address);
  
  return new_socket;
}

int parseArgs(int argc, char * argv[]) {///////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (argc != 2) {
    printf("Please give <port>\n");
    return 1;
  }
  int port = atoi(argv[1]);
  
  return port;
}

/* Receives data from a UDP socket and stores it in a charArray structure */
bool sock_receive(SocketObj* sock, charArray* message_string){/////////////////////////////////////////////////////////////////////////////////////
  memset(message_string->array, 0, sizeof(message_string->array)); //set to 0
  
  // receive data from a UDP socket, returns the number of bytes received
  message_string->len = recvfrom(
    sock->sockfd, 
    message_string->array, 
    sizeof(message_string->array), 
    0, 
    (struct sockaddr * ) &sock->address, 
    &sock->address_len
  );
  
  if (message_string->len < 0) {
    perror("server : NOTHING RECEIVED");
    close(sock->sockfd);
    exit(1);
  }
  
  return (message_string->len>0);
}

/* To send data over a UDP socket */
void sock_send(SocketObj* sock, char* string){/////////////////////////////////////////////////////////////////////////////////////////////////////
  sendto(
    sock->sockfd, 
    string, 
    strlen(string), 
    0, 
    (struct sockaddr * ) &sock->address, 
    sizeof(sock->address)
  );
}

void writeDataToFile(char *filename, char *buffer, int frag_no, int bytes_read) {////////////////////////////////////////////////////////////////////////
    // Calculate the position to write the fragment to
    long long offset = (long long)frag_no * 1000; // Each fragment seems to have a fixed size of 1000 bytes (or intended to), 
                                                  // so the offset is the fragment number multiplied by 1000.

    // Open the file in binary write mode
    FILE *file = fopen(filename, "r+b"); // open the file in "read and binary write" mode
    if (file == NULL) {
        file = fopen(filename, "w+");  // create the file in "read/write" mode
        if (file == NULL) {
            perror("Error creating the file");
            return;
        }
    }

    // Move the file pointer to the correct position, ensuring that the data fragment is written to the correct position in the file
    fseek(file, offset, SEEK_SET);

    // Write the fragment to the file
    size_t bytes_written = fwrite(buffer, 1, bytes_read, file); // the number of bytes to write is determined by 'bytes_read'

    if (bytes_written != bytes_read) {  //check if we write the same as we read
        perror("Failed to write fragment to the file");
    }

    // Close the file
    fclose(file);
}

/////////////////////////////////////////////////// main /////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char * argv[]) {
  int port = parseArgs(argc,argv);
  SocketObj sock = make_udp_socket(port);
  charArray message_string;
  printf("server : listening to %d\n",port);

  /* wait for connection, then receive and print text */
  while ( sock_receive(&sock, &message_string) == true ) {
    if (strcmp(message_string.array, "ftp") == 0) {
      char yes[] = "yes";
      sock_send(&sock, yes);
      puts("server : connected");
      break;
    }
  }
  
  puts("server : starting to receive...");
  
  char filename[256];
  int total_frag, frag_no, bytes_read;
  char buffer[1000];

  while(1){  // the server will keep listening for new information
    sock_receive(&sock,&message_string);

    if(sscanf(message_string.array, "%d:%d:%d:%[^:]:", &total_frag, &frag_no, &bytes_read, filename)!=4){continue;} // check for correct headers
    printf("server : [RECEIVED] %d:%d:%d:%s:\n", total_frag, frag_no, bytes_read, filename); // packet being handled
    memcpy(buffer, message_string.array + message_string.len-bytes_read, bytes_read);  // (*destination, *source, length of memcpy)

    writeDataToFile(filename, buffer, frag_no-1, bytes_read);
    if(rand()%2==0){
          sock_send(&sock, itoa(frag_no)); // acknoledgement for each packet received

    }
  }

  close(sock.sockfd);
}
