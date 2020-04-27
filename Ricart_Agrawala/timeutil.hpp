/*
Shreyash H. Turkar
26 April 2020
Covid19 Special

Time utilities
*/

#pragma once

#include<sys/time.h>
#include<inttypes.h>

//returns timestamp in milli
uint64_t millis();

//returns timestamp in sec
uint64_t seconds();

//sleep
//para1: seconds para2: useconds
void delay(uint64_t,uint64_t);

//generates random delays
void random_delay();

/*
!nnovation Eternity
*/