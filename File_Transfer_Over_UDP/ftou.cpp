#include"ftou.hpp"

FTOUServer::FTOUServer(uint8_t oct4, uint8_t oct3, uint8_t oct2, uint8_t oct1, uint16_t port){
    
    host = new UDPHost(oct4,oct3,oct2,oct1,port);
    remote_host = NULL;

    inorder_arrivals = new Queue;
    out_of_arrivals = new PriorityQueue;
    
    next_seq_to_receive = 0;
    next_seq_send = 0;
    
    buffer = new char*[WS];
    buffer_status = new status;
    timer = new timeval[WS];

    uint16_t i = 0;
    while(i<WS){
        buffer_status[i] = FREE;
        buffer[i] = new char[MAX_MESSAGE_LEN];
        i++;
    }
    set_one = true;

}

void FTOUServer::timerHandler(){
    
    timeval current_sys_time;
    bool change_window;

    while (true){
        change_window = true;
        for(uint16_t i = 0 ; i < WS ; i++){
            gettimeofday(&current_sys_time,NULL);
            timermtx.lock();
            if(buffer_status[i] == ACK_NOT_RECEIVED && timer[i].tv_usec+RESEND_TIME < current_sys_time.tv_usec){
                //resend buffer[i]
                
                host->send(buffer[i],*remote_host);

                DEBUG("Resent : "<<(!set_one)*(WS) + i<<"\n");
                
                //set timer to current time
                timer[i] = current_sys_time;

            }   
            timermtx.unlock();
            if(buffer_status[i] == ACK_NOT_RECEIVED || buffer_status[i] == FREE){
                change_window = false;
            }
            usleep(1000);
        }

        if(change_window){
            DEBUG("All received change window\n");
            set_one = !set_one;
            next_seq_send = set_one?0:WS;
            for(uint16_t i = 0 ; i < WS ; i++){
                buffer_status[i] = FREE;
            }
            
        }
    }
}


void FTOUServer::receiver(){
    sockaddr_in *received_from;
    packet *received_packet;
    message *received_message = new message;
    while(true){
        received_message = host->recv(received_message);
        received_packet = make_packet(received_message->buffer);

        //if checksum is incorrect ignore the packet
        if(!checksum_check(received_packet)){
            continue;
        }
        
        if(remote_host != NULL && remote_host->sin_addr.s_addr == received_message->hostaddr.sin_addr.s_addr && remote_host->sin_port == received_message->hostaddr.sin_port){
            //returning user
            switch (received_packet->frame_type){
                
                case ACK:
                    DEBUG("ACK IN : "<<received_packet->seq_number<<"\n");
                    timermtx.lock();
                    if(received_packet->seq_number < WS && set_one == true){
                        buffer_status[received_packet->seq_number] = ACK_RECEIVED;
                    }else if(received_packet->seq_number >= WS && set_one == false){
                        buffer_status[received_packet->seq_number%WS] = ACK_RECEIVED;
                    }
                    timermtx.unlock();
                    
                    break;

                case DATA:
                    DEBUG("Data IN : "<<received_packet->seq_number<<"\n");
                    sendAck(received_packet->seq_number);
                    inqueuemtx.lock();
                    if(received_packet->seq_number == next_seq_to_receive){
                        inorder_arrivals->enqueue(received_packet);
                        next_seq_to_receive++;
                        while(!out_of_arrivals->isEmpty() && out_of_arrivals->top()->seq_number == next_seq_to_receive){
                            inorder_arrivals->enqueue(out_of_arrivals->top());
                            out_of_arrivals->dequeue();
                            next_seq_to_receive++;
                        }
                        next_seq_to_receive%=2*WS;
                    }else{
                        out_of_arrivals->enqueue(received_packet);
                    }
                    inqueuemtx.unlock();
                    break;
                case INFO:
                    if(received_packet->i_code == TERMINATE_ACK){
                        exit(0);
                    }
                    break;
            }

        }
    }
}

void FTOUServer::sendData(char* arr, uint16_t len){
    
    while((next_seq_send >= WS && set_one) || (next_seq_send >= 2*WS)){}

    packet *new_arr = new packet;
    new_arr->frame_type = DATA;
    new_arr = add_data(new_arr,arr);
    new_arr->seq_number = next_seq_send;
    new_arr->i_code = GENERAL;
    new_arr->checksum = get_checksum(new_arr);
    char *send_arr = make_frame(new_arr);
    buffer[next_seq_send%WS] = send_arr;

    timeval cur_time;
    gettimeofday(&cur_time,NULL);

    timermtx.lock();
    timer[next_seq_send%WS] = cur_time;
    buffer_status[next_seq_send%WS] = ACK_NOT_RECEIVED;
    timermtx.unlock();

    host->send(buffer[next_seq_send],*remote_host);
    DEBUG("Packet Out : "<<next_seq_send<<"\n");
    next_seq_send++;
}

void FTOUServer::sendAck(uint16_t seq_num){
    packet *new_arr = new packet;
    new_arr->frame_type = ACK;
    new_arr->seq_number = seq_num;
    new_arr->checksum = get_checksum(new_arr);
    DEBUG("ACK out : "<<seq_num<<"\n");
    char *send_arr = make_frame(new_arr);

    host->send(send_arr,*remote_host);
}

void FTOUServer::sendInfo(uint8_t i_code){
    packet *new_arr = new packet;
    new_arr->frame_type = INFO;
    new_arr->i_code = i_code;
    new_arr->checksum = get_checksum(new_arr);
    char *send_arr = make_frame(new_arr);
    DEBUG("INFO sent : "<<(uint16_t)i_code<<"\n");
    host->send(send_arr,*remote_host);

}

void FTOUServer::sendFileRequest(char *file_name){
    packet *new_arr = new packet;
    new_arr->frame_type = INFO;
    new_arr->i_code = REQ_CONN;
    new_arr = add_data(new_arr,file_name);
    new_arr->checksum = get_checksum(new_arr);
    char *send_arr = make_frame(new_arr);

    host->send(send_arr,*remote_host);
}

void FTOUServer::sendEOF(){
    while((next_seq_send > WS && set_one) || (next_seq_send > 2*WS)){}

    packet *new_arr = new packet;
    new_arr->frame_type = DATA;
    new_arr->seq_number = next_seq_send;
    new_arr->i_code = FILE_EOF;
    new_arr->checksum = get_checksum(new_arr);
    char *send_arr = make_frame(new_arr);
    buffer[next_seq_send%WS] = send_arr;

    timeval cur_time;
    gettimeofday(&cur_time,NULL);

    timermtx.lock();
    timer[next_seq_send%WS] = cur_time;
    buffer_status[next_seq_send%WS] = ACK_NOT_RECEIVED;
    timermtx.unlock();

    host->send(buffer[next_seq_send],*remote_host);
}

void FTOUServer::sendFile(char* file_name, uint8_t oct4, uint8_t oct3, uint8_t oct2, uint8_t oct1, uint16_t port){
    
    remote_host = new sockaddr_in;
    remote_host->sin_family    = AF_INET;
    remote_host->sin_addr.s_addr =  (((uint32_t)oct1&0xFF)<<24)|(((uint32_t)oct2&0xFF)<<16)|(((uint32_t)oct3&0xFF)<<8)|(((uint32_t)oct4&0xFF)); 
    remote_host->sin_port = htons(port);

    sendFileRequest(file_name);
    std::cout<<"Request Sent!";
    sockaddr_in *received_from;
    packet *received_packet;
    message *received_message =  new message;
    char *arr;
    while (1){
        received_message = host->recv(received_message);
        received_packet = make_packet(received_message->buffer);
        DEBUG("OK\n");
        if(received_packet->frame_type == INFO && received_packet->i_code == ACCEPT_CONN){
            break;
        }
    }
    std::thread t1(&FTOUServer::receiver,this);
    std::thread t2(&FTOUServer::timerHandler,this);
    std::cout<<"Starting . . .\n";
    std::ifstream fin;
    fin.open(file_name);
    arr = new char[MAX_DATA_LENGHT-1];
    int dis = 0;
    char symbols[] = {'-','\\','|','/'};
    while(!fin.eof()){
        fin.read(arr,MAX_DATA_LENGHT-1);
        DEBUG("Reading And Sending . . . "<<arr<<"\n");
        sendData(arr,MAX_DATA_LENGHT-1);

        std::cout<<"\rSending. . . "<<symbols[dis++];
        dis%=4;
    }

    DEBUG("Sending EOF\n");
    sendEOF();
    DEBUG("EOF SENT \n");
    std::cout<<"\nSend Successfull!\n";


    
    t1.join();
    t2.join();





}

void FTOUServer::recvFile(){
    sockaddr_in *received_from;
    packet *received_packet;
    message *received_message = new message;
    char *arr;
    int temp;
    while (1){
        received_message = host->recv(received_message);
        received_packet = make_packet(received_message->buffer);
        std::cout<<ACCEPT_REQUEST(received_packet->data,&received_message->hostaddr)<<"\n";
        std::cin>>temp;
        if(temp == 1){
            remote_host = new sockaddr_in;
            remote_host->sin_addr.s_addr = received_message->hostaddr.sin_addr.s_addr;
            remote_host->sin_family = received_message->hostaddr.sin_family;
            remote_host->sin_port = received_message->hostaddr.sin_port;
            arr = get_data(received_packet);
            sendInfo(ACCEPT_CONN);
            break;
        }
        
    }

    std::ofstream fout;
    fout.open(arr);
    std::thread t1(&FTOUServer::receiver,this);
    std::thread t2(&FTOUServer::timerHandler,this);
    int dis = 0;
    char symbols[] = {'-','\\','|','/'};
    while(1){
        
        inqueuemtx.lock();

        if(!inorder_arrivals->isEmpty() && inorder_arrivals->top()->frame_type == DATA && inorder_arrivals->top()->i_code == FILE_EOF){
            DEBUG("EOF received\n");
            std::cout<<"\nReceived\n";
            sendInfo(TERMINATE_ACK);
            fout.close();
            exit(0);
            
        }
        if(!inorder_arrivals->isEmpty() && inorder_arrivals->top()->frame_type == DATA ){
            DEBUG((get_data(inorder_arrivals->top()))<<"\n");
            fout.write(get_data(inorder_arrivals->top()),inorder_arrivals->top()->data_len);

        }

        inorder_arrivals->dequeue();
        inqueuemtx.unlock();
        usleep(200);
        std::cout<<"\rReceiving. . . "<<symbols[dis++];
        dis%=4;

    }
    
    t1.join();
    t2.join();
    
}           