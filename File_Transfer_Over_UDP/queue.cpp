#include"queue.hpp"

//*********************QUEUE*************************************

Queue::Queue(){
    tail = NULL;
}

bool Queue::isEmpty(){
    return tail == NULL;
}

void Queue::enqueue(packet *input){
    node *temp = new node;
    temp->data = input;
    temp->next = tail==NULL?temp:tail->next;
    if(tail != NULL){
        tail->next = temp;
    }
    tail = temp;
}

void Queue::dequeue(){
    if(tail == NULL){
        return;
    }
    node *temp = tail->next;
    if(tail == tail->next){
        tail = NULL;
    }else{
        tail->next = tail->next->next;
    }
    delete(temp);
}

packet* Queue::top(){
    return tail==NULL?NULL:tail->next->data;
}

void Queue::Display() {
    struct node* temp = tail;
    do{
        temp = temp->next;
        std::cout<<temp->data->seq_number<<"\t";
    }while(temp != tail);
}

//*********************PRIORITY QUEUE*************************************

PriorityQueue::PriorityQueue(){
    head = NULL;
}

bool PriorityQueue::isEmpty(){
    return head == NULL;
}

void PriorityQueue::enqueue(struct packet *input){
    
    if(head == NULL || head->data->seq_number > input->seq_number){
        struct node *new_node = new node;
        new_node->data = input; 
        new_node->next = head;
        head = new_node;
        return;
    }else if(head->data->seq_number == input->seq_number){
        return;
    }

    node *temp = head;

    while(temp->next != NULL && temp->next->data->seq_number < input->seq_number){
        temp = temp->next;
    }

    if(temp->next != NULL && temp->next->data->seq_number == input->seq_number){
        return;
    }

    struct node *new_node = new node;
    new_node->data = input;
    new_node->next = temp->next;
    temp->next = new_node;
}

packet* PriorityQueue::top(){
    return head==NULL?NULL:head->data;
}

void PriorityQueue::dequeue(){
    if(head == NULL){
        return;
    }
    node *temp = head;
    head = head->next;
    delete(temp);
}

//*********************END*************************************