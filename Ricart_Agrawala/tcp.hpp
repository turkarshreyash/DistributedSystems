/*
Shreyash H. Turkar
26 April 2020
Covid19 Special

Hides underlaying complex process of establishing tcp connection
*/

#pragma once

//general
#include<iostream>
#include<inttypes.h>

//sockets
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h>
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h>
/*
init tcp server
parameters: oct1,oct2,oct3,oct4, port
returns fd
*/
uint32_t init_tcp_server(uint8_t , uint8_t , uint8_t , uint8_t , uint16_t);

/*
accept tcp conn
parameters: server fd, remote fd
*/
void incoming_conn(uint32_t,uint32_t*);

/*
send tcp conn req
parameters: server addr -> oct1,oct2,oct3,oct4, port
returns: fd
*/
void outgoing_conn(uint8_t , uint8_t , uint8_t , uint8_t , uint16_t, uint32_t*);

//send data to fd
//parameters: fd, data, len(data)
void send_data(uint32_t,int8_t*,uint32_t);

//recv data from fd
//parameters: fd, data, &len(data)
void receive_data(uint32_t,int8_t*,uint32_t);


/*
!nnovation Eternity
*/