#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>  
#include <time.h>
#define MAX_LINE 256
#define PACKET_SIZE 1000

#define INTERNET_PROTOCAL 0
#include <stdbool.h>

#include "../print.c"
typedef struct{
  char array[PACKET_SIZE+255+64] ;
  int len;
} charArray;

struct packet {
  unsigned int total_frag; //total number of fragments of the file. Eachpacket contains one fragment
  unsigned int frag_no; //The frag_no field indicates the sequence number of the fragment, starting from 1.
  unsigned int size; //The size field should be set to the size of the data. Therefore, it should be in the range of 0 to 1000. 
  char filename[MAX_LINE];
  char filedata[1000]; //All members of the packet should be sent as a single string
};


typedef int SockDescriptor;
typedef struct{
  SockDescriptor sockfd;
  struct sockaddr_in address;
  socklen_t address_len;
} SocketObj;


SocketObj make_udp_socket(int port, char* host) { //////////////////////////////////////////////////////////////////////////////////////////

  /* translate host name into peer's IP address */
  struct hostent *ip_address = gethostbyname(host);
  if (!ip_address) {
    fprintf(stderr, "simplex-talk: unknown host: %s\n", host);
    exit(1);
  }

  /* build address data structure */
  SocketObj new_socket;
  bzero((char * ) & new_socket.address, sizeof(new_socket.address));
  new_socket.address.sin_family = AF_INET;
  bcopy(ip_address->h_addr, (char *)&new_socket.address.sin_addr.s_addr, ip_address->h_length); //copy into address
  new_socket.address.sin_port = htons(port);

  new_socket.sockfd = socket(PF_INET, SOCK_DGRAM, INTERNET_PROTOCAL);
  
  /* setup passive open */
  if (new_socket.sockfd < 0) {
    perror("deliver: socket cannot be created");
    exit(1);
  }

  new_socket.address_len = sizeof(new_socket.address);
  return new_socket;
}

void sock_send(SocketObj* sock, char* string){ //////////////////////////////////////////////////////////////////////////////////////////
  sendto(
    sock->sockfd, 
    string, 
    strlen(string), 
    0, 
    (struct sockaddr *) &sock->address, 
    sizeof(sock->address)
  );
}
     
void sock_send_raw(SocketObj* sock, charArray* message_string){ ////////////////////////////////////////////////////////////////////////
  sendto(
    sock->sockfd, 
    message_string->array, 
    message_string->len, 
    0, 
    (struct sockaddr * ) &sock->address, 
    sizeof(sock->address)
  );
}     

bool sock_receive(SocketObj * sock, charArray* message_string){ ////////////////////////////////////////////////////////////////////////

  // Set up a timeval struct for the timeout
  struct timeval timeout;
  timeout.tv_sec = 5;  // 5 seconds
  timeout.tv_usec = 0;

  // Set up the file descriptor set
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(sock->sockfd, &fds);//
  int result = select(sock->sockfd + 1, &fds, NULL, NULL, &timeout); //we use the select function to wait for data to be available on the socket for up to 5 seconds 
                                                                     //if no data arrives within that time, it returns false. Otherwise, it proceeds to receive data as before
  if (result == -1) {
      perror("select error");
      close(sock->sockfd);
      exit(1);
  } else if (result == 0) {
      // Timeout occurred
      return false;
  }

  memset(message_string->array, 0, sizeof(message_string->array)); //set to 0
  message_string->len = recvfrom(
    sock->sockfd, 
    message_string->array, 
    sizeof(message_string->array), 
    0, 
    (struct sockaddr * ) &sock->address, 
    &sock->address_len
  );

  if (message_string->len < 0) {
      perror("deliver: NOTHING RECEIVED");
      close(sock->sockfd);
      exit(1);
  }

  return (message_string->len>0);
}


/////////////////////////////////////////// main ////////////////////////////////////////////////////////
bool hasConnection=false;

int main(int argc, char * argv[]){
  // For measuring the round-trip time = from sending a file to end of transmission
  clock_t start, end;
  double round;
  
  if (argc!=3) {
    fprintf(stderr, "usage: deliver needs 3 args\n");
    exit(1);
  }

  // extract user input
  char * host = argv[1];
  int port = atoi(argv[2]); 
  printf("deliver: listening to %d\n",port);

  // Prompt the user for input
  char user_input[256]; // Assuming a maximum input length of 255 characters
  printf("Please enter a message: ");
  if (fgets(user_input, sizeof(user_input), stdin) == NULL) {
      fprintf(stderr, "Error reading user input.\n");
      exit(1);
  }

  // Parse the user input
  char command[4];
  char filename[256]; // Assuming a maximum file name length of 255 characters
  if (sscanf(user_input, "%3s %255s", command, filename) != 2) {
      fprintf(stderr, "Invalid input\n");
      exit(1);
  }

  // Check for input command
  if (strcmp(command, "ftp") != 0) {
      fprintf(stderr, "Invalid command. no ftp .\n");
      exit(1);
  }

  // Check the existence of the file
  if (access(filename, F_OK) == -1) {
      fprintf(stderr, "file '%s' does not exist.\n", filename);
      exit(1);
  }

  SocketObj sock= make_udp_socket(port, host);
  charArray message_string;

  // To setup for connection
  if(!hasConnection){
    struct timeval start_time, end_time;
      gettimeofday(&start_time, NULL);  // Record the start time
        sock_send(&sock,"ftp");
        sock_receive(&sock, &message_string);
      gettimeofday(&end_time, NULL);  // Record the end time
      double time_delay = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
      printf("deliver: received response = %s, response time = %fs \n", message_string.array,time_delay);

    /* Check if the response is "YES" */
    if (strcmp(message_string.array, "yes") == 0) {
        printf("deliver: A file transfer can start.\n");
        hasConnection=true;
        
    } else {
        printf("deliver: Server did not respond with YES\n");
    }    
  }
  
  int chunk_size = 1000;
  char buffer[chunk_size];

  // We open and read the file
  FILE *file;
  file = fopen(filename, "rb");
  if (file == NULL) {
    perror("Error opening file");
    exit(1);
  }

  // Determine the file size
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
  printf("deliver: file size = %d B\n", file_size);
  int frag_no=1;
  int total_frag=(file_size+chunk_size-1)/chunk_size;
  char header[255+64];  // Assuming a maximum length of 255+64 characters is enough (file name and numbers)

  // Packets delivery
  while (file_size > 0) {
    size_t bytes_to_read = (file_size > chunk_size) ? chunk_size : file_size;
    memset(buffer,0,sizeof(buffer));
    size_t bytes_read = fread(buffer, 1, bytes_to_read, file);
    sprintf(header, "%d:%d:%d:%s:", total_frag, frag_no, bytes_read,filename);
    
    if (bytes_read < bytes_to_read) perror("Error reading from file\n");        // if (feof(file)) 
    printf("deliver: sending packet with header %s | header leangth %d\n", header,strlen(header));
    strcpy(message_string.array, header);
    memcpy(message_string.array + strlen(header), buffer, sizeof(buffer)); // copy buffer into message_string.array, at the location right after the string header
    message_string.len=strlen(header)+bytes_read;
    
    // start recording for round trip time
    start = clock();
    int yes = 0;

    while(1){

      sock_send_raw(&sock, &message_string);

      if(sock_receive(&sock, &message_string) && atoi(message_string.array)==frag_no){
        puts("deliver: got ack");
        
        if(yes == 0){
          // Record for round trip time for the 1st packet
          end = clock();
          yes = 1;
        }
        
        break;
      }
      printf("deliver: dhasConnectionid not got ack, resending %d\n\n",frag_no);
    }

    round = ((double)end-start) / CLOCKS_PER_SEC;

    file_size -= bytes_read;
    frag_no++;
  }

  printf("The round trip time for a packet is: %fs \n", round);
  
  puts("deliver: finished transmit");
  main(argc,argv);
  close(sock.sockfd);
}
