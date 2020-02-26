#include<iostream>
#include<cstring>
#include"ftou.hpp"

uint16_t chartou16(char* oct){
    uint16_t i = 0, j = 0;
    while (oct[j]!='\0'){
        i *= 10;
        i += oct[j] - '0';
        j++; 
    }
    return i;
}



int main(int argc, char* args[]){
    
    // recv -> ./ftou.out -r 127 0 0 1 8000
    // send -> ./ftou.out -s 127 0 0 1 8008 <path>  127  0   0    1   8000
    //               0     1  2 3 4  5   6     7     8   9  11   12    13  
    std::cout<<"[F]ile [T]ransfer [O]ver [U]DP Server\n";

    if(argc < 7){
        std::cout<<"Too Few Arguments\n";
        exit(0);
    }

    uint8_t oct4, oct3, oct2, oct1;
    uint16_t port;

    oct4 = (uint8_t)(chartou16(args[2])&0xff);
    oct3 = (uint8_t)(chartou16(args[3])&0xff);
    oct2 = (uint8_t)(chartou16(args[4])&0xff);
    oct1 = (uint8_t)(chartou16(args[5])&0xff);

    port = (chartou16(args[6]));
    
    std::cout<<"Server Iniatizing. . . !\n";
    FTOUServer *server = new FTOUServer(oct4,oct3,oct2,oct1,port);
    std::cout<<"Server Iniatized!\n";

    if(strcmp(args[1],"-r") == 0){
        std::cout<<"Receiving at "<<(uint16_t)oct4<<"."<<(uint16_t)oct3<<"."<<(uint16_t)oct2<<"."<<(uint16_t)oct1<<"."<<port<<"\n";
        server->recvFile();
    }else if(strcmp(args[1],"-s") == 0){
        if(argc != 13){
            std::cout<<"Sender Error : Too Few Arguments\n";
            exit(0);
        }
        std::cout<<"Sender : "<<(uint16_t)oct4<<"."<<(uint16_t)oct3<<"."<<(uint16_t)oct2<<"."<<(uint16_t)oct1<<"."<<port<<"\n";
        oct4 = (uint8_t)(chartou16(args[8])&0xff);
        oct3 = (uint8_t)(chartou16(args[9])&0xff);
        oct2 = (uint8_t)(chartou16(args[10])&0xff);
        oct1 = (uint8_t)(chartou16(args[11])&0xff);

        port = (chartou16(args[12]));
        std::cout<<"Sending '"<<args[7]<<"' to : "<<(uint16_t)oct4<<"."<<(uint16_t)oct3<<"."<<(uint16_t)oct2<<"."<<(uint16_t)oct1<<"."<<port<<"\n";

        server->sendFile(args[7],oct4,oct3,oct2,oct1,port);
    }




}