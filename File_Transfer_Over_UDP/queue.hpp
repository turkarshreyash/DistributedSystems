/*
Shreyash Turkar
*/

#pragma once

#include<iostream>
#include"frame.h"



struct node{
    struct packet *data;
    struct node *next;
};

class Queue{
    
    private:
        struct node *tail;

    public:

        Queue();
        bool isEmpty();
        void enqueue(packet*);
        packet* top();
        void Display();
        void dequeue();

};

class PriorityQueue{
    
    private:
        struct node *head;

    public:

        PriorityQueue();
        bool isEmpty();
        void enqueue(packet*);
        packet* top();
        void dequeue();
};