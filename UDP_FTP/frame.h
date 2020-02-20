/*
Shreyash H. Turkar
*/

#pragma once

#include<iostream>
#include<inttypes.h>

//Size of data segment of packet
#define MAX_DATA_LENGHT 512

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


//converting packet to string to send over network
uint16_t make_frame(packet *input, char *output){

    //Frame Type
    output[FRAME_TYPE] = input->frame_type;
    
    //Sequence Number
    output[SEQ_NUMBER_HIGH] = (input->seq_number>>8)&255;
    output[SEQ_NUMBER_LOW] = (input->seq_number)&255;
    
    //Message Code
    output[I_CODE] = input->i_code;

    //Checksum
    output[CHECKSUM_HIGH] = (input->checksum>>8)&255;
    output[CHECKSUM_LOW] = (input->checksum)&255;

    //Data Len
    output[DATA_LEN_HIGH] = (input->data_len>>8)&255;
    output[DATA_LEN_LOW] = (input->data_len)&255;

    //Data Segment
    uint16_t i = 0;
    while (i<input->data_len){
        output[DATA_OFFSET+i] = input->data[i];
        i++;
    }
    return HEADER_LEN + input->data_len;
}

//Convert a string received over network to packet
void make_packet(char *message, struct packet *output){
    
    //frame type
    output->frame_type = (uint8_t)message[FRAME_TYPE];

    //Sequence Number
    output->seq_number = (message[SEQ_NUMBER_HIGH]<<8)|(message[SEQ_NUMBER_LOW]);

    //I_CODE
    output->i_code = (uint8_t)message[I_CODE];

    //CHECKSUM
    output->checksum = (message[CHECKSUM_HIGH]<<8)|(message[CHECKSUM_LOW]);

    //DATALEN
    output->data_len = (message[DATA_LEN_HIGH]<<8)|(message[DATA_LEN_LOW]);

    //Data
    uint16_t i = 0;
    while(i < output->data_len){
        output->data[i] = message[DATA_OFFSET+i];
        i++;
    }

    return;
}