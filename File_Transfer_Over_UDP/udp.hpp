/*
Shreyash H. Turkar
2020
*/

#pragma once

#include<iostream>
#include<netinet/in.h>
#include<sys/socket.h>
#include<cstring> 
#include<unistd.h> 


#define MAX_MESSAGE_LEN 1024

typedef struct message{
    struct sockaddr_in hostaddr;
    socklen_t sock_len;
    int len;
    char buffer[MAX_MESSAGE_LEN];

    message(){
        memset(&hostaddr, 0, sizeof(hostaddr));
        sock_len = sizeof(hostaddr);
    }
}message;

class UDPHost{
    int sockfd;  
    struct sockaddr_in servaddr;
    public:
        UDPHost(uint8_t , uint8_t , uint8_t , uint8_t , uint16_t );
        void send(char* , uint8_t , uint8_t , uint8_t , uint8_t , uint16_t);
        void send(char*,sockaddr_in);
        message* recv(message*);

};  