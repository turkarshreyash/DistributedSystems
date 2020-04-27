#include"tcp.hpp"

uint32_t  init_tcp_server(uint8_t oct4, uint8_t oct3, uint8_t oct2, uint8_t oct1, uint16_t port){
    int32_t fd;
    struct sockaddr_in addr;
    //create file descriptor
    fd = socket(AF_INET, SOCK_STREAM, 0);
    
    //failed 
    if(fd == 0){
        perror("socket failed"); 
        exit(EXIT_FAILURE);
    }

    int opt = 1;

    //Idk
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){ 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    }

    memset(&addr, 0, sizeof(addr)); 
    //populate address
    addr.sin_family    = AF_INET;
    addr.sin_addr.s_addr =  (((uint32_t)oct1&0xFF)<<24)|(((uint32_t)oct2&0xFF)<<16)|(((uint32_t)oct3&0xFF)<<8)|(((uint32_t)oct4&0xFF)); 
    addr.sin_port = htons(port);

    //bind fd to addr
    if (bind(fd, (struct sockaddr *)&addr,sizeof(addr))<0){
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 

    //start listening
    if (listen(fd, 3) < 0){ 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    }

    return fd;
}


void incoming_conn(uint32_t fd, uint32_t *in_conn){
    struct sockaddr_in re_addr;
    if((*in_conn = accept(fd, (struct sockaddr *)&re_addr, (socklen_t*)&re_addr))<0){ 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    }
    return;
}

void outgoing_conn(uint8_t oct4, uint8_t oct3, uint8_t oct2, uint8_t oct1, uint16_t port, uint32_t *re_fd){

    struct sockaddr_in re_addr;
    *re_fd = 0;

    memset(&re_addr, 0, sizeof(re_addr)); 
    //populate address
    re_addr.sin_family    = AF_INET;
    re_addr.sin_addr.s_addr =  (((uint32_t)oct1&0xFF)<<24)|(((uint32_t)oct2&0xFF)<<16)|(((uint32_t)oct3&0xFF)<<8)|(((uint32_t)oct4&0xFF)); 
    re_addr.sin_port = htons(port);

    if ((*re_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Socket creation error");
        return; 
    }

    while(connect(*re_fd, (struct sockaddr *)&re_addr, sizeof(re_addr)) < 0){ 
        perror("Connection Error Retrying...");
    }

    return;
}

void send_data(uint32_t fd, int8_t* arr, uint32_t len){
    send(fd , arr , len , 0 ); 
}

void receive_data(uint32_t fd, int8_t* arr, uint32_t len){
    read(fd, arr, len);
}