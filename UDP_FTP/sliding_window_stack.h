/*
Shreyash H. Turkar
*/

#pragma once

#include<netinet/in.h>
#include<sys/time.h>
#include"network_layer.h"
#include"frame.h"

//Resend Data Packets if ack not received in 
#define TIME_OUT_USECONDS 2000

typedef enum buffer_status{FREE,ACK_RECEIVED,ACK_NOT_RECEIVED}buffer_status;

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
    buffer_status *b_status;            //Check if ack is received or not
    struct timeval **time_out;           //if time_out > current_time and no ack received resend 

}connection;


//DATA packet handler
void data_handler(packet *received){
    //****TO-DO*****************************************

    //GBN Accept only packet with seq_num == expected seq_num
    //Ignore rest
    if(received->seq_number == conn->next_frame_expected){
        //Send to Upper Layer


        //Send ACK
    }

}

//received handler
void ack_handler(packet *received){
    //****TO-DO*****************************************

    //Validate ack_number
    if(received->seq_number < conn->ws && received->seq_number >= 0){
        
        //mark ack as received
        conn->b_status[received->seq_number] = ACK_RECEIVED;
        //if ack_number received == expected time_val
        if(received->seq_number == conn->next_ack_expected){

            //if ack_number == 0
            if(received->seq_number == 0){
                //enable sending ws-1 th frame
                conn->send_ws = true;
            }
            
            //if this was last ack expected
            else if(received->seq_number == conn->ws - 1){
                //Reset for next round
                uint16_t i = 0;
                while(i < conn->ws){
                    conn->b_status[i] = FREE;
                    i++;
                }
                conn->send_ws = false;
                conn->next_frame_to_send = 0;
                conn->next_ack_expected = 0;
            }
            conn->next_ack_expected++;
        }
    }
}

//ACK Timer Handler
void ack_timer_handler(){
    //****TO-DO*****************************************
    struct timeval current_time;
    while(1){
        gettimeofday(&current_time, NULL);
        if(conn->next_ack_expected == ACK_NOT_RECEIVED && conn->time_out[conn->next_ack_expected]->tv_usec+TIME_OUT_USECONDS > current_time.tv_usec){
            gettimeofday(conn->time_out[conn->next_ack_expected], NULL);
            send_to_conn(conn->buffer[conn->next_ack_expected]);
        }
    }
}

void send_data_packet(packet *send){
    //****TO-DO*****************************************
    while(conn->next_frame_to_send == conn->ws || (conn->next_frame_to_send == conn->ws-1 && !conn->send_ws)){
        continue;
    }
    char *temp = new char[MAX_MESSAGE_LEN];
    make_frame(send,temp);

    conn->buffer[conn->next_frame_to_send] = temp;
    send->seq_number = conn->next_frame_to_send;
    conn->b_status[conn->next_frame_to_send] = ACK_NOT_RECEIVED;
    gettimeofday(conn->time_out[conn->next_frame_to_send], NULL);
    
    send_to_conn(temp);

    conn->next_frame_to_send++;

}