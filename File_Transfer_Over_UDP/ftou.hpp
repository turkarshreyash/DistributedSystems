/*
Shreyash H. Turkar
*/

#include<inttypes.h>
#include<fstream>
#include<sys/time.h>
#include<thread>
#include <mutex>

#include"udp.hpp"
#include"queue.hpp"

// #define DEBUG_MODE

#ifdef DEBUG_MODE
#define DEBUG(A) std::cout<<A;
#endif
#ifndef DEBUG_MODE
#define DEBUG(A) ;
#endif

//Address and Port Extract
#define HOST(A) ((A)->sin_addr.s_addr)
#define PORT(A) ((A)->sin_port)

//Display addr and port in HRF - Human readable form
#define HRF_ADDR(A) (((A)>>0)&0xFF)<<"."<<(((A)>>8)&0xFF)<<"."<<(((A)>>16)&0xFF)<<"."<<(((A)>>24)&0xFF)
#define HRF_PORT(A) (htons(A)) //To-Do List 

//User input questions
#define ACCEPT_REQUEST(F,H) "Accept File: "<<(F)<<" From: "<<HRF_ADDR(HOST(H))<<":"<<HRF_PORT(PORT(H))<<" ?\n[1 = Y/0 = n] >> "

#define WS 8
#define RESEND_TIME 2000

enum status{FREE,ACK_NOT_RECEIVED,ACK_RECEIVED};


//[F]ile [T]ransfer [O]ver [U]DP Server
class FTOUServer{

    UDPHost *host;                        //underlaying udp server
    sockaddr_in *remote_host;

    bool receiving;           //receiving or sending => 1 = rec;0 = send
    //***********RECEVING SIDE************
    //receving queues
    Queue *inorder_arrivals;             //store inorder arrivals
    PriorityQueue *out_of_arrivals;    //store out of order arrivals
    uint16_t next_seq_to_receive;

    
    //**********SENDING SIDE******************
    uint16_t next_seq_send;
    char **buffer;
    status *buffer_status;
    timeval *timer;       
    bool set_one;        //0toWS-1 or WSto2WS-1
    std::mutex timermtx; //timer and buffer status mutex
    std::mutex inqueuemtx;


    //*********************SEND FUNCTIONS*********************
    void sendAck(uint16_t);               //send ack for recived frames  
    void sendInfo(uint8_t);               //send info to host 
    void sendData(char* arr, uint16_t);              //send data to host
    void sendEOF();
    void sendFileRequest(char *file_name);

    void timerHandler();
    void receiver();


    public:
        FTOUServer(uint8_t,uint8_t,uint8_t,uint8_t,uint16_t);     //init ftou server at addr
        void sendFile(char*,uint8_t,uint8_t,uint8_t,uint8_t,uint16_t);  //send file to addr
        void recvFile();                                                //File
};