/*
Shreyash H. Turkar
27 April 2020
*/

/*
This main has designed for 3 node distributed system
Suppose A, B, C are three node
A will establish connection with B
B will establish connection with C
C will establish connection with A
In this way all nodes will connet with each other using TCP/IP
*/

/*
Calling parameters
./main <server_addr_port> <remote_addr_port> <sec>  
server_addr_port: port number of this node
remote_addr_port: port number of a remote node this node wishes to establish connection with
sec: interval in seconds for calling cs

In case of running node A
server_addr_port will be port number of A
remote_addr_port will be port number of B
*/

//standard
#include<iostream>
#include<inttypes.h>
#include<cstring>
#include<string.h>
#include<thread>
#include<mutex>
#include<vector>

//specific
#include"timeutil.hpp"
#include"tcp.hpp"
#include"criticalsection.hpp"
#include"message.hpp"

//in this case 2
#define NUMBER_REMOTE_NODES 2
#define RECV_CONN_REQ 1
#define SENT_CONN_REQ 0

//Ricart Aggrawala's Ri consists of all nodes
//Ri consists of fd of remote nodes
//Ri[0] -> B (Sent Req) and R[1] -> C (Received Req)
uint32_t *Ri = new uint32_t[NUMBER_REMOTE_NODES];
//to store reply received from
bool *reply_received = new bool[NUMBER_REMOTE_NODES];

//local queue
messageQueue *localQ = new messageQueue();

//localQ mutex
std::mutex localQmutex;

//fd for this server
uint64_t server_fd;

//will request critical section after every sec (4th Parameter) seconds
void request_access_critical_section(uint64_t);

//this will store time when this node last requested cs
uint64_t req_seconds;
uint64_t req_useconds;

//boolean variable to tell if this node is in CS
bool access_granted;

//to tell if this node is interested in CS
bool interested_cs;

//will listen for incomming cs requests
void listen_for_requests();

//nodes are based on tcp connection each connection requires a separate receiver
//For all nodes in Ri there is a Ni to handle incoming data
void NiHandler(uint16_t);

int main(int argc, char *args[]){
    
    if(argc<4){
        std::cout<<"./main <server_addr_port> <remote_addr_port> <sec>\n";
        exit(EXIT_FAILURE);
    }

    uint16_t server_port = std::stoi(args[1]);
    uint16_t remote_port = std::stoi(args[2]);
    uint64_t sec = std::stoull(args[3]);
    
    //initialise server
    std::cout<<"Initial: Creaing server on: "<<server_port<<"\n";
    server_fd = init_tcp_server(127,0,0,1,server_port);
    std::cout<<"Success: Server Running on: "<<server_port<<"\n";
    std::cout<<"Initial: Creating threads t1 -> Sending and t2 -> Accepting connection Requests\n";
    
    std::thread t1(outgoing_conn,127,0,0,1,remote_port,&Ri[SENT_CONN_REQ]);
    std::thread t2(incoming_conn,server_fd,&Ri[RECV_CONN_REQ]);

    t1.join();
    t2.join();
    system("clear");
    std::cout<<"Success: Remote Connections established\n";
    std::cout<<"Success: Ri Constructed\n";

    std::cout<<"Initial: Creating threads t1 -> accessing CS and t2 -> replying to CS requests\n";

    std::thread t3(request_access_critical_section,sec);
    std::thread t4(listen_for_requests);

    std::cout<<"Success: This node is running successfully!\n";

    t3.join();
    t4.join();

}


void request_access_critical_section(uint64_t sec){
    int8_t *req_message;
    int8_t *reply_messsage;
    uint64_t reply_fd;
    uint16_t retry;

    //forever loop :-DDDDDDD
    while(true){
        //not interested
        interested_cs = false;

        //delay of sec
        delay(sec,0);
        
        //interested
        interested_cs = true;

        std::cout<<"Intitial: Requesting Critical Section\n";
        //create req message with current time stamp and fs of this server
        req_message = create_request_message(server_fd);
        //store timestamp when cs requestdee
        req_seconds = get_timestamp_seconds(req_message);
        req_useconds = get_timestamp_useconds(req_message);


        //send req to all nodes in Ri
        for(uint16_t i = 0 ; i < NUMBER_REMOTE_NODES ; i++){
            reply_received[i] = false;
            send_data(Ri[i],req_message,MESSAGE_SIZE);
        }
        std::cout<<"Success: Requested Critical Section at "<<req_seconds<<"."<<req_useconds<<"\n";

        std::cout<<"Intitial: Waiting for reponse\n";
        //wait for response from all other nodes

        for(uint16_t i = 0 ; i < NUMBER_REMOTE_NODES ; i++){
            retry=1;
            while(!reply_received[i]){
                if(retry>6){
                    std::cout<<"Info: Retrying to Req "<<i<<"\n";
                    send_data(Ri[i],req_message,MESSAGE_SIZE);
                    retry = 1;
                }
                delay(0,500);
                retry++;
            }
        }
        std::cout<<"Success: Reply received\n";
        std::cout<<"Intitial: Accessing Critical Section\n";
        access_granted = true;

        //access critical section
        //<<<<<<<<<<<<<<<CRITICAL SECTION>>>>>>>>>>>>>>>>>>>>>
        critical_section();
        //<<<<<<<<<<<<<<<<_________>>>>>>>>>>>>>>>>>>>>>>>>>>>

        access_granted = false;
        std::cout<<"Success: Released Critical Section\n";
        std::cout<<"Intitial: Replying to all Ni in LocalQ\n";
        //create reply message and sent to all nodes in localQ
        reply_messsage = create_reply_message(server_fd);
        while(!localQ->isEmpty()){
            reply_fd = get_fd_message(localQ->pop());
            send_data(reply_fd,reply_messsage,MESSAGE_SIZE);
        }
        std::cout<<"Success: Replied to all Ni in LocalQ\n";
    }
}

void listen_for_requests(){
    //creating i threads for one for each node in Ri
    std::vector<std::thread> threads;

    std::cout<<"Initial: Creating "<<NUMBER_REMOTE_NODES<<" handler threads\n";

    //run thread for each ni
    for(uint16_t i = 0 ; i < NUMBER_REMOTE_NODES ; i++){
        threads.push_back(std::thread(NiHandler,i));
    }

    std::cout<<"Success: Created "<<NUMBER_REMOTE_NODES<<" handler threads\n";
    
    //run thread for each ni
    for(std::vector<std::thread>::iterator itr = threads.begin(); itr != threads.end() ; itr++){
        itr->join();
    }
}


void NiHandler(uint16_t re){
    int8_t *reply_message;
    int8_t *received_message = new int8_t[MESSAGE_SIZE];
    uint16_t message_type;
    uint64_t recv_seconds,recv_useconds;

    while(true){
        std::cout<<"Info: ReadyReceiving message\n";
        receive_data(Ri[re],received_message,MESSAGE_SIZE);
        message_type = get_message_type(received_message);
        std::cout<<"Info: A message received from "<<re<<" Message type: "<<message_type<<"\n";
        //if reply is received from ni
        if(message_type == (uint16_t)REPLY){
            std::cout<<"Info: Messsage type [REPLY] from "<<re<<"\n";
            reply_received[re] = true;
        }else if(message_type == (uint16_t)REQUEST){
            std::cout<<"Info: Messsage type [REQUEST] from "<<re<<"\n";
            //if request message  is received
            if(!interested_cs && !access_granted){
                std::cout<<"Intitial: Not in CS nor Interested Replying to "<<re<<"\n";
                reply_message = create_reply_message(server_fd);
                delay(0,100);
                send_data(Ri[re],reply_message,MESSAGE_SIZE);
                std::cout<<"Success: Replyed to "<<re<<"\n";
            }else if(interested_cs && !access_granted){
                recv_seconds = get_timestamp_seconds(received_message);
                recv_useconds = get_timestamp_useconds(received_message);
                std::cout<<"REQ : "<<req_seconds<<"."<<req_useconds<<"\n";
                std::cout<<"RECV: "<<recv_seconds<<"."<<recv_useconds<<"\n";
                if(recv_seconds < req_seconds || (recv_seconds == req_seconds && recv_useconds < req_useconds)){
                    std::cout<<"Intitial: Not in CS Replying to "<<re<<"\n";
                    reply_message = create_reply_message(server_fd);
                    send_data(Ri[re],reply_message,MESSAGE_SIZE);
                    std::cout<<"Success: Replyed to "<<re<<"\n";
                }else{
                    localQ->insert(received_message);
                    std::cout<<"Info: Requested by "<<re<<" deffered added to localQ\n";
                }
            }

        }
    }   
    
}