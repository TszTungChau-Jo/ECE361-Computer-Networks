#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER_SIZE 1024

// enum macros
enum MessageType {
    LOGIN, LO_ACK, LO_NAK, EXIT, JOIN, JN_ACK, JN_NAK, LEAVE_SESS, NEW_SESS, NS_ACK, NS_NAK, MESSAGE, QUERY, QU_ACK
};

struct Message {
    int type;
    int size;
    char source[MAX_BUFFER_SIZE];
    char data[MAX_BUFFER_SIZE];
};

void message_to_str(struct Message msg, char *result) {
    sprintf(result, "%d:%d:%s:%s", msg.type, msg.size, msg.source, msg.data);
}

struct Message str_to_msg(char *s) {
    struct Message msg;
    sscanf(s, "%d:%d:%[^:]:%[^\n]", &msg.type, &msg.size, msg.source, msg.data);
    return msg;
}

// Global variables
char client_id[MAX_BUFFER_SIZE] = "";
int is_in_session = 0;
// Add socket-related variables as needed

void connect_to_server(char *server_ip, int server_port) {
    // Implement socket connection (not shown)
}

char* receive_from_server() {
    // Implement receiving from server (not shown)
    return NULL;
}

void receive_loop() {
    while (1) {
        char *message_str = receive_from_server();
        struct Message msg = str_to_msg(message_str);
        printf("%s\n", msg.data);
    }
}

void send_to_server(enum MessageType message_type, char *data) {
    // Implement sending to server (not shown)
}

void login(char *client_id, char *password, char *server_ip, int server_port) {
    connect_to_server(server_ip, server_port);
    // Start a separate thread for receiving messages
    // Implement threading (not shown)
    printf("hi\n");
    send_to_server(LOGIN, password);
    char *reply = receive_from_server();
    struct Message reply_msg = str_to_msg(reply);
    if (reply_msg.type == LO_ACK) {
        printf("login success\n");
    } else if (reply_msg.type == LO_NAK) {
        printf("login failed. Reason is %s\n", reply_msg.data);
    }
}

void logout() {
    send_to_server(EXIT, "");
}

void join_session(char *session_id) {
    if (is_in_session) {
        printf("Error: please leave session first\n");
        return;
    }
    send_to_server(JOIN, session_id);
    char *reply = receive_from_server();
    struct Message reply_msg = str_to_msg(reply);
    if (reply_msg.type == JN_ACK) {
        printf("join %s success\n", reply_msg.data);
        is_in_session = 1;
    } else if (reply_msg.type == JN_NAK) {
        printf("join %s failed. Reason is %s\n", session_id, reply_msg.data);
    }
}

void leave_session() {
    send_to_server(LEAVE_SESS, "");
    is_in_session = 0;
}

void create_session(char *session_id) {
    send_to_server(NEW_SESS, session_id);
    char *reply = receive_from_server();
    struct Message reply_msg = str_to_msg(reply);
    if (reply_msg.type == NS_ACK) {
        printf("join %s success\n", reply_msg.data);
        is_in_session = 0;
    } else if (reply_msg.type == NS_NAK) {
        printf("join %s failed. Reason is %s\n", session_id, reply_msg.data);
    }
}

void list_users() {
    send_to_server(QUERY, "");
    char *reply = receive_from_server();
    printf("User list is %s\n", reply);
}

void quit() {
    logout();
    // Implement code to disconnect and stop the program (main return 0)
}

void text_send_to_server(char *text) {
    send_to_server(MESSAGE, text);
}

int main() {
    // Command inputs
    char inputs[7][15] = {"/login", "/logout", "/joinsession", "/leavesession", "/createsession", "/list", "/quit"};

    while (1) {
        char cmd[MAX_BUFFER_SIZE];
        fgets(cmd, sizeof(cmd), stdin);
        cmd[strcspn(cmd, "\n")] = '\0';

        if (strncmp(cmd, inputs[0], strlen(inputs[0])) == 0) {
            char client_id[MAX_BUFFER_SIZE], password[MAX_BUFFER_SIZE], server_ip[MAX_BUFFER_SIZE];
            int server_port;
            sscanf(cmd, "%*s %s %s %s %d", client_id, password, server_ip, &server_port);
            login(client_id, password, server_ip, server_port);
            break;
        } else {
            printf("Please login first\n");
        }
    }

    while (1) {
        char cmd[MAX_BUFFER_SIZE];
        fgets(cmd, sizeof(cmd), stdin);
        cmd[strcspn(cmd, "\n")] = '\0';

        char cmd_firstword[MAX_BUFFER_SIZE];
        sscanf(cmd, "%s", cmd_firstword);

        if (strncmp(cmd_firstword, inputs[1], strlen(inputs[1])) == 0) {
            logout();
        } else if (strncmp(cmd_firstword, inputs[2], strlen(inputs[2])) == 0) {
            char session_id[MAX_BUFFER_SIZE];
            sscanf(cmd, "%*s %s", session_id);
            join_session(session_id);
        } else if (strncmp(cmd_firstword, inputs[3], strlen(inputs[3])) == 0) {
            leave_session();
        } else if (strncmp(cmd_firstword, inputs[4], strlen(inputs[4])) == 0) {
            char session_id[MAX_BUFFER_SIZE];
            sscanf(cmd, "%*s %s", session_id);
            create_session(session_id);
        } else if (strncmp(cmd_firstword, inputs[5], strlen(inputs[5])) == 0) {
            list_users();
        } else if (strncmp(cmd_firstword, inputs[6], strlen(inputs[6])) == 0) {
            quit();
            break;
        } else {
            if (is_in_session) {
                text_send_to_server(cmd);
            } else {
                printf("Please join session first\n");
            }
        }
    }

    return 0;
}
