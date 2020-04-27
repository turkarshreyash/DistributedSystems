#include"timeutil.hpp"

uint64_t millis(){
    struct timeval temp;
    gettimeofday(&temp,0);
    return temp.tv_usec;
}

uint64_t seconds(){
    struct timeval temp;
    gettimeofday(&temp,0);
    return temp.tv_sec;
}

void delay(uint64_t s, uint64_t us){
   struct timeval temp;
   struct timeval t;
    t.tv_sec = s;
    t.tv_usec = us;
    select(0, 0, 0, 0, &t);
}