/*
Shreyash H. Turkar
*/
#pragma once
/*
This File Contains All Displays Messages
*/

#include<iostream>

//Future Version May Have GUI
//Display To User: using standard i/o making GUI integration easy
#define DISPLAY(A) std::cout<<A<<std::endl;

//Success Messages
#define CONNECTION_ACCEPTED "Connection Accepted!"
#define STARTING_SEND "Starting Sending. . . "
#define FILE_SEND_SUCCESS "File Sent Successfully!"
#define FILE_RECV_SUCCESS "File Received Successfully!"

//Error Messages
#define CONNECTION_REJECTED "Connection Rejected!"
#define FILE_TRANSFER_FAILED "File Transfer Failed !"

//Address and Port Extract
#define HOST(A) (A.sin_addr.s_addr)
#define PORT(A) (A.sin_port)

//Display addr and port in HRF - Human readable form
#define DISPLAY_ADDR(A) ((A>>24) & 0xFF)<<"."<<((A>>16) & 0xFF)<<"."<<((A>>8) & 0xFF)<<"."<<((A & 0xFF)
#define DISPLAY_PORT(A) ((uint16_t)A) //To-Do List 

//User input questions
#define ACCEPT_REQUEST(F,H) "Accept File: "<<(F)<<" From: "<<DISPLAY_ADDR(HOST(H))<<":"<<DISPLAY_PORT(PORT(H))<<" ?"