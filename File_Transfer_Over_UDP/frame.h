/*
Shreyash H. Turkar
Test S
*/

#pragma once

#include<iostream>
#include<inttypes.h>

//Size of data segment of packet
#define MAX_DATA_LENGHT 16

//Data Packet Type
#define ACK (uint8_t)1
#define DATA (uint8_t)2
#define INFO (uint8_t)3//No Ack for this type of frames

//Message Contains - Valid only for INFO frames
#define REQ_CONN (uint8_t)1
#define ACCEPT_CONN (uint8_t)2
#define REJECT_CONN (uint8_t)3
#define TERMINATE_CONN (uint8_t)4
#define TERMINATE_CONN_ERROR (uint8_t)5
#define TERMINATE_ACK (uint8_t)6

//DATA I_CODE 
#define GENERAL (uint8_t)1  //Non EOF
#define FILE_EOF (uint8_t)2     //EOF

//Index for string of struct packet
#define HEADER_LEN 8
//Message Header
#define FRAME_TYPE 0
#define SEQ_NUMBER_HIGH 1
#define SEQ_NUMBER_LOW 2
#define I_CODE 3
#define CHECKSUM_HIGH 4
#define CHECKSUM_LOW 5
#define DATA_LEN_HIGH 6
#define DATA_LEN_LOW 7
//DATA SEGMENT
#define DATA_OFFSET 8


//packet
typedef struct packet{
    //HEADER
    uint8_t frame_type;       //type of frame 
    uint16_t seq_number;      //sequence_number for DATA frame and ack_number for ACK frame
    uint8_t i_code;           //For INFO frame only tell action to be taken 
    uint16_t checksum;        //To-Do List*
    uint16_t data_len;        //length of data segment

    //DATA SEGMENT
    uint8_t data[MAX_DATA_LENGHT];  //Data
}packet;

char * make_frame(packet *);
    
struct packet * make_packet(char *);

struct packet* add_data(struct packet*,char *);

char* get_data(struct packet*);

uint16_t get_checksum(struct packet*);

bool checksum_check(struct packet*);