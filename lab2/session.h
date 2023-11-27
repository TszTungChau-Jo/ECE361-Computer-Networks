#ifndef MESSAGE_H
#define MESSAGE_H

#define MAX_NAME 1000
#define MAX_DATA 1000

enum msgType {
    LOGIN,
    // <clientID, clientPW>         
    
    LO_ACK,        
    // <>
    
    LO_NAK,
    // <failureMsg>
    
    EXIT,
    // <>
    
    JOIN,
    // <sessionID>
    
    JN_ACK,
    // <sessionID>
    
    JN_NAK,
    // <sessionID, failureMsg>
    
    LEAVE_SESS,
    // <>
    
    NEW_SESS,
    // <>

    NS_ACK,
    // <sessionID>

    MESSAGE,
    // <msg>

    QUERY,
    // <>

    QU_ACK,
    // <users and sessions list>

};

struct message {
    unsigned int type; // mapping to a specific task
    unsigned int size;
    unsigned char source[MAX_NAME];
    unsigned char data[MAX_DATA];
};




#endif