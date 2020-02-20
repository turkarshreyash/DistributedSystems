/*
Shreyash H. Turkar
*/

#pragma once

#include<netinet/in.h>


//Standard Sliding Window protocol for in-order delivery
//GBN - Go Back N - No Recv buffer
typedef struct connection{
    
    struct sockaddr_in remote_addr; 
    
    //recv side 
    //uint16_t window size = 1 GBN
    uint16_t next_frame_expected; //seq number for next frame
    
    //sending side
    uint16_t ws;                   //window size of sender
    uint16_t next_ack_expected;    //next ack to be expected
    uint16_t next_frame_to_send;   //ptr to next free buffer for sending message
    char **buffer;                  //senders side buffer for retransmission of frame on failure
    bool send_ws;                  //for preventing ws-1 th frame from sending before receiving ack of 0
    long long *time_out;           //if time_out > current_time and no ack received resend 

}connection;