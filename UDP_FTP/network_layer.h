/*
Shreyash H. Turkar
*/

/*
Virtual Physical Layer Working on UDP.
*/

#pragma once

#include<iostream>
#include<sys/socket.h>
#include<stdlib.h> 
#include<netinet/in.h>
#include<unistd.h> 
#include<inttypes.h>
#include<cstring>

#include"sliding_window_stack.h"
#include"frame.h"
#include"user_interface.h"


#define MAX_MESSAGE_LEN 1024

//Network Layer
int sockfd; 
char buffer[MAX_MESSAGE_LEN]; 
struct sockaddr_in servaddr, cliaddr;

//Sliding Window Stack
struct connection *conn;


void startNetworkLayer(uint16_t ws, uint16_t port){

    //Imported From GFG
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    }
      
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
      
    servaddr.sin_family    = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(port); 
      
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ){ 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    }

    conn = NULL;

    //Receiver Thread 1
    //Ack Thread 2
    //****TO-DO*****************************************



    
}

void receiver(){
    uint len;
    uint16_t n;
    struct packet received;

    while(1){        
        //Wait for receival of a packet
        n = recvfrom(sockfd, buffer, MAX_MESSAGE_LEN, 0, (struct sockaddr*)&cliaddr,&len);

        //if message length is less than 8B reject packet
        //Because Header is itself 8B long
        if(n < 8){
            sendto(sockfd, "ERROR", 5, 0,(struct sockaddr*)&cliaddr,len);
            continue;
        }

        //else
        buffer[n] = '\0';

        if(conn == NULL){
            //Create a packet from message
            make_packet(buffer,&received);

            //Accept only INFO frames for connection request
            if(received.frame_type == INFO && received.i_code == REQ_CONN){
                //Ask user to accept the request
                DISPLAY(ACCEPT_REQUEST(received.data,cliaddr));
                //To-Do Take User Input
                //****TO-DO*****************************************
            }   
        }else{
            //only single connection is allowed

            //accept only if already connected host
            if(cliaddr.sin_addr.s_addr == conn->remote_addr.sin_addr.s_addr && cliaddr.sin_port == conn->remote_addr.sin_port){
                make_packet(buffer,&received);
                if(received.frame_type == INFO){
                    if(received.i_code == ACCEPT_CONN){
                        DISPLAY(CONNECTION_ACCEPTED);
                        DISPLAY(STARTING_SEND);
                    }else if(received.i_code == REJECT_CONN){
                        DISPLAY(CONNECTION_REJECTED);
                        //close program if remote host rejects connection
                        exit(0);
                    }else if(received.i_code == TERMINATE_CONN){
                        //File transfer complete
                        DISPLAY(FILE_RECV_SUCCESS);
                        exit(0);
                    }else if(received.i_code == TERMINATE_CONN_ERROR){
                        //File Transfer Failed
                        DISPLAY(FILE_TRANSFER_FAILED);
                        //To-Do => Delete the file
                         //****TO-DO*****************************************


                        //Exit 
                        exit(0);
                    }
                }else if(received.frame_type == DATA){
                    //To-Do List
                     //****TO-DO*****************************************


                }else if(received.frame_type == ACK){
                    //To-Do List
                     //****TO-DO*****************************************

                    
                }

                
            }else{
                sendto(sockfd, "BUSY", 5, 0,(struct sockaddr*)&cliaddr,len);
            }








        }


    }
} 
