#include"udp.hpp"

UDPHost::UDPHost(uint8_t oct4, uint8_t oct3, uint8_t oct2, uint8_t oct1, uint16_t port){
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    }

    memset(&servaddr, 0, sizeof(servaddr)); 

    servaddr.sin_family    = AF_INET;
    servaddr.sin_addr.s_addr =  (((uint32_t)oct1&0xFF)<<24)|(((uint32_t)oct2&0xFF)<<16)|(((uint32_t)oct3&0xFF)<<8)|(((uint32_t)oct4&0xFF)); 
    servaddr.sin_port = htons(port); 

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ){ 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    }
}

void UDPHost::send(char *message,uint8_t oct4, uint8_t oct3, uint8_t oct2, uint8_t oct1, uint16_t port){
    sockaddr_in cliaddr_in;
    cliaddr_in.sin_family    = AF_INET;
    cliaddr_in.sin_addr.s_addr = (((uint32_t)oct1&0xFF)<<24)|(((uint32_t)oct2&0xFF)<<16)|(((uint32_t)oct3&0xFF)<<8)|(((uint32_t)oct4&0xFF)); 
    cliaddr_in.sin_port = htons(port); 
    send(message, cliaddr_in);
}

void UDPHost::send(char *message, sockaddr_in cliaddr_in){
    sendto(sockfd, message, MAX_MESSAGE_LEN, 0, (struct sockaddr*)&cliaddr_in, sizeof(cliaddr_in));
}

message* UDPHost::recv(message* new_message){
    new_message->len = recvfrom(sockfd,new_message->buffer, MAX_MESSAGE_LEN, 0, (struct sockaddr*)&new_message->hostaddr,&new_message->sock_len);
    new_message->buffer[new_message->len] = '\0';
    return new_message;
}