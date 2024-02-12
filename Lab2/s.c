#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define MAX_NAME 20
#define MAX_DATA 1024

// enum macros
enum MessageType {
    LOGIN, LO_ACK, LO_NAK, EXIT, JOIN, JN_ACK, JN_NAK, LEAVE_SESS, NEW_SESS, NS_ACK, NS_NAK, MESSAGE, QUERY, QU_ACK
};

struct Message {
    int type;
    int size;
    char source[MAX_NAME];
    char data[MAX_DATA];
};

struct Client {
    char client_id[MAX_NAME];
    char session_id[MAX_NAME];
    char ip[MAX_DATA];
    int portaddress;
    int client_socket;
};

struct Client list_of_clients[MAX_DATA];
char list_of_sessions[MAX_DATA][MAX_NAME];
int num_clients = 0;
int num_sessions = 0;

void message_to_str(struct Message *msg, char *result) {
    sprintf(result, "%d:%d:%s:%s", msg->type, msg->size, msg->source, msg->data);
}

void str_to_msg(char *s, struct Message *msg) {
    sscanf(s, "%d:%d:%[^:]:%[^\n]", &msg->type, &msg->size, msg->source, msg->data);
}

// Function prototypes
// Function to create a server socket and bind it to the specified port
int create_server_socket(int port) {
    // Create a socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        return -1;
    }

    // Set up the server address struct
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Error binding socket");
        close(server_socket);
        return -1;
    }

    // Listen for incoming connections
    if (listen(server_socket, 10) == -1) {
        perror("Error listening for connections");
        close(server_socket);
        return -1;
    }

    printf("Server socket created and bound to port %d\n", port);

    return server_socket;
}

void handle_clients(int server_socket) {
    int client_sockets[MAX_CLIENTS];
    int server_socket_fd = server_socket;
    fd_set read_fds;

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        client_sockets[i] = -1;
    }

    client_sockets[0] = server_socket_fd;

    while (1) {
        FD_ZERO(&read_fds);
        int max_fd = server_socket_fd;

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (client_sockets[i] > 0) {
                FD_SET(client_sockets[i], &read_fds);
                if (client_sockets[i] > max_fd) {
                    max_fd = client_sockets[i];
                }
            }
        }

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (FD_ISSET(client_sockets[i], &read_fds)) {
                if (client_sockets[i] == server_socket_fd) {
                    // New client connection
                    // (implement server_socket.accept() equivalent in C)
                    // client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
                    // client_sockets[num_clients++] = client_socket;
                    printf("New client connected\n");
                } else {
                    // Received message from a client
                    struct Message message;
                    // (implement recv equivalent in C)
                    // recv(client_sockets[i], &message, sizeof(struct Message), 0);
                    if (message.size == 0) {
                        printf("Client disconnected\n");
                        // (implement remove client_sockets[i] from the array in C)
                        // client_sockets[i] = -1;
                    } else {
                        printf("Received message from client: %s\n", message.data);
                        handle_msg(message, "IP", 12345, client_sockets[i]);
                    }
                }
            }
        }
    }
}
void _send_to(char *client_id, int message_type, char *data) {
    // Assuming you have a data structure or mechanism to map client_id to their corresponding sockets

    // Example: You have a map or an array of client_info structs with socket information
    // struct client_info {
    //     char id[50];
    //     int socket;
    // };

    // Replace the following with your actual data structure
    // For demonstration purposes, a simple array is used here
    struct client_info {
        char id[50];
        int socket;
    };

    struct client_info clients[] = {
        {"client1", /*client1_socket*/},
        {"client2", /*client2_socket*/},
        // Add more clients as needed
    };

    // Find the socket associated with the given client_id
    int client_socket = -1;
    for (size_t i = 0; i < sizeof(clients) / sizeof(clients[0]); ++i) {
        if (strcmp(client_id, clients[i].id) == 0) {
            client_socket = clients[i].socket;
            break;
        }
    }

    if (client_socket == -1) {
        fprintf(stderr, "Error: Client with ID %s not found\n", client_id);
        return;
    }

    // Assuming message_type and data are passed as parameters
    // Construct the message to be sent
    char buffer[MAX_BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "%d:%s", message_type, data);

    // Send the message to the client
    if (send(client_socket, buffer, strlen(buffer), 0) == -1) {
        perror("Error sending data");
        // Handle error as needed
    } else {
        printf("Data sent to client %s\n", client_id);
    }
}
int login_success(char *username, char *password);
// Define your Client structure here
struct Client {
    // Add necessary fields based on your implementation
    char client_id[256];
    char session_id[256];
    // Add other fields as needed
};

// Function prototypes
void _send_to(const char *source, int type, const char *data);
int login_success(const char *source, const char *data);

// Global variables
struct Client list_of_clients[100];
char list_of_sessions[100][256];
int list_of_clients_count = 0;
int list_of_sessions_count = 0;

void handle_msg(struct Message msg, const char *ip, int portaddress, int client_socket) {
    int i;
    struct Client newClient;

    msg.type = atoi(msg.type);

    if (msg.type == LOGIN) {
        int already_login = 0;
        for (i = 0; i < list_of_clients_count; ++i) {
            if (strcmp(list_of_clients[i].client_id, msg.source) == 0) {
                already_login = 1;
                break;
            }
        }
        if (already_login) {
            _send_to(msg.source, LO_NAK, "already logged in in another place");
        } else if (login_success(msg.source, msg.data)) {
            _send_to(msg.source, LO_ACK, "");

            strcpy(newClient.client_id, msg.source);
            strcpy(newClient.session_id, "");
            // Set other fields of newClient as needed
            list_of_clients[list_of_clients_count++] = newClient;
            printf("hi\n");
        } else {
            _send_to(msg.source, LO_NAK, "wrong password or username");
        }
    } else if (msg.type == EXIT) {
        for (i = 0; i < list_of_clients_count; ++i) {
            if (strcmp(list_of_clients[i].client_id, msg.source) == 0) {
                // Assuming list_of_clients and list_of_sessions are parallel arrays
                // Adjust this part based on your actual implementation
                for (int j = i; j < list_of_clients_count - 1; ++j) {
                    list_of_clients[j] = list_of_clients[j + 1];
                }
                --list_of_clients_count;
                break;
            }
        }
    } else if (msg.type == JOIN) {
        // Implement JOIN logic
    }
    // Add similar blocks for other message types
}

int login_success(const char *source, const char *data) {
    // Implement login_success function
    return 1; // Replace with actual logic
}

int main() {
    int server_socket = create_server_socket(12345);
    handle_clients(server_socket);
    return 0;
}

