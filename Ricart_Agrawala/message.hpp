/*
Shreyash H. Turkar
27 April 2020
Covid19 Special

Message System
and
Priority Queue w.r.t Timestamp 
*/

#pragma once

#include<iostream>
#include<inttypes.h>
#include<cstring>
#include"timeutil.hpp"

/*
Message Structure
Fixed 18-Byte message Size

|<----------------------------------------------------------------26 byte------------------------------------------------------->|

|<----------8 byte--------->|<----------8 byte--------->|<----------8 byte--------->|<-------1 byte------>|<-------1 byte------>|
0                           8                           16                          24                    25            
+---------------------------+---------------------------+---------------------------+---------------------+---------------------+
| timestamp sec 64-bit      |   timestamp usec 64-bit   |   req_ser_fd 64-bit       |  message_type 8-bit |   NULL (EOM)        |
+---------------------------+---------------------------+---------------------------+---------------------+---------------------+
*/

#define MESSAGE_SIZE 26

#define REQUEST (int8_t)64
#define REPLY (int8_t)32

#define OFFSET_SEC 0
#define OFFSET_USEC 8
#define OFFSET_FD 16
#define OFFSET_MSG_TYPE 24
#define OFFSET_EOM 25

/*
Creates request message to be sent to nodes
para: fd of server
*/
int8_t* create_request_message(uint64_t);

/*
Creates reply message to be sent to nodes
para: fd of server
*/
int8_t* create_reply_message(uint64_t);

uint64_t get_fd_message(int8_t*);

uint64_t get_timestamp_seconds(int8_t*);

uint64_t get_timestamp_useconds(int8_t*);

int8_t get_message_type(int8_t*);

//Priority Queue


struct node{
    uint64_t timestamp_seconds;
    uint64_t timestamp_useconds;
    int8_t *arr;
    struct node *next;
};

class messageQueue{
    struct node *head;
    public:
        messageQueue();
        bool isEmpty();
        void insert(int8_t*);
        int8_t* pop();
};