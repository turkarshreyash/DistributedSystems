#include"frame.h"


//converting packet to string to send over network
char* make_frame(packet *input){

    char *output = new char[MAX_DATA_LENGHT+HEADER_LEN];

    //Frame Type
    output[FRAME_TYPE] = input->frame_type&0xff;
    
    //Sequence Number
    output[SEQ_NUMBER_HIGH] = (uint8_t)(input->seq_number>>8)&255;
    output[SEQ_NUMBER_LOW] = (uint8_t)(input->seq_number)&255;
    
    //Message Code
    output[I_CODE] = (uint8_t)input->i_code;

    //Checksum
    output[CHECKSUM_HIGH] = (uint8_t)(input->checksum>>8)&255;
    output[CHECKSUM_LOW] = (uint8_t)(input->checksum)&255;

    //Data Len
    output[DATA_LEN_HIGH] = (uint8_t)(input->data_len>>8)&255;
    output[DATA_LEN_LOW] = (uint8_t)(input->data_len)&255;

    //Data Segment
    uint16_t i = 0;
    while (i<input->data_len){
        output[DATA_OFFSET+i] = input->data[i];
        i++;
    }
    return output;
}

//Convert a string received over network to packet
struct packet* make_packet(char *message){

    struct packet *output = new packet;
    
    //frame type
    output->frame_type = (uint8_t)message[FRAME_TYPE];

    //Sequence Number
    output->seq_number = (((uint16_t)message[SEQ_NUMBER_HIGH]&0xff)<<8)|((uint16_t)message[SEQ_NUMBER_LOW]&0xff);

    //I_CODE
    output->i_code = (uint8_t)message[I_CODE];

    //CHECKSUM
    output->checksum = (((uint16_t)message[CHECKSUM_HIGH]&0xff)<<8)|((uint16_t)message[CHECKSUM_LOW]&0xff);

    //DATALEN
    output->data_len = (((uint16_t)message[DATA_LEN_HIGH]&0xff)<<8)|((uint16_t)message[DATA_LEN_LOW]&0xff);

    //Data
    uint16_t i = 0;
    while(i < output->data_len){
        output->data[i] = message[DATA_OFFSET+i];
        i++;
    }

    return output;
}


struct packet* add_data(struct packet* input, char *arr){
    uint16_t i = 0;
    while (arr[i] != '\0' && i < MAX_DATA_LENGHT){
        input->data[i] = arr[i]; 
        i++;
    }
    input->data[i] = 0;
    input->data_len = i;
    return input;
}

char* get_data(struct packet *input){
    char *arr = new char[input->data_len];
    uint16_t i = 0;
    while (i < input->data_len){
        arr[i] = input->data[i];
        i++;
    }
    return arr;
}

uint16_t get_checksum(packet *input){
    
    uint16_t checksum = 0;

    //frame type addition
    checksum += ((uint16_t)input->frame_type);
    //if carry then add
    checksum = (checksum + (checksum >> 8)) & 0xff;
    //seq number low addition
    checksum += ((uint16_t)input->seq_number & 0xff);
    //if carry then add
    checksum = (checksum + (checksum >> 8)) & 0xff;
    //seq number high addition
    checksum += ((uint16_t)(input->seq_number>>8));
    //if carry then add
    checksum = (checksum + (checksum >> 8)) & 0xff;
    //i_code addition
    checksum += ((uint16_t)input->i_code);
    //if carry then add
    checksum = (checksum + (checksum >> 8)) & 0xff;
    //data _len low addition
    checksum += ((uint16_t)input->data_len & 0xff);
    //if carry then add
    checksum = (checksum + (checksum >> 8)) & 0xff;
    //data _len high addition
    checksum += ((uint16_t)(input->data_len>>8));
    //if carry then add
    checksum = (checksum + (checksum >> 8)) & 0xff;

    uint16_t i = 0;
    while(i < input->data_len){
        //data addition
        checksum += ((uint16_t)input->data[i]);
        //if carry then add
        checksum = (checksum + (checksum >> 8)) & 0xff;
        i++;
    }

    return 0xff - checksum;
}

bool checksum_check(packet *input){
    return get_checksum(input) == input->checksum;
}

