#include"message.hpp"

int8_t* create_request_message(uint64_t fd){
    int8_t *arr = new int8_t[MESSAGE_SIZE];
    uint64_t sec = seconds();
    uint64_t usec = millis();
    int8_t type = REQUEST;

    memcpy(arr+OFFSET_SEC,(void*)&sec,8);

    memcpy(arr+OFFSET_USEC,(void*)&usec,8);

    memcpy(arr+OFFSET_FD,(void*)&fd,8);

    memcpy(arr+OFFSET_MSG_TYPE,(void*)&type,1);

    return arr;
}

int8_t* create_reply_message(uint64_t fd){
    int8_t *arr = new int8_t[MESSAGE_SIZE];
    uint64_t sec = seconds();
    uint64_t usec = millis();
    int8_t type = REPLY;

    memcpy(arr+OFFSET_SEC,(void*)&sec,8);

    memcpy(arr+OFFSET_USEC,(void*)&usec,8);

    memcpy(arr+OFFSET_FD,(void*)&fd,8);

    memcpy(arr+OFFSET_MSG_TYPE,(void*)&type,1);

    return arr;
}

uint64_t get_fd_message(int8_t *arr){
    uint64_t fd;
    memcpy((void*)&fd,arr+OFFSET_FD,8);
    return fd;
}

uint64_t get_timestamp_seconds(int8_t *arr){
    uint64_t time;
    memcpy((void*)&time,arr+OFFSET_SEC,8);
    return time;
}

uint64_t get_timestamp_useconds(int8_t *arr){
    uint64_t time;
    memcpy((void*)&time,arr+OFFSET_USEC,8);
    return time;
}

int8_t get_message_type(int8_t* arr){
    int8_t mtype;
    memcpy((void*)&mtype,arr+OFFSET_MSG_TYPE,1);
    return mtype;
}


messageQueue::messageQueue(){
    head = nullptr;
}

bool messageQueue::isEmpty(){
    return head==nullptr;
}

void messageQueue::insert(int8_t *arr){
    
    struct node *new_node = new node;
    new_node->timestamp_seconds = get_timestamp_seconds(arr);
    new_node->timestamp_useconds = get_timestamp_useconds(arr);
    new_node->arr = new int8_t[MESSAGE_SIZE];
    memcpy(new_node->arr,arr,MESSAGE_SIZE);
    new_node->next = nullptr;

    if(head==nullptr){
        head = new_node;
        return;
    }

    struct node *temp = head;

    while(temp->next != nullptr && (temp->next->timestamp_seconds < new_node->timestamp_seconds || (temp->next->timestamp_seconds == new_node->timestamp_seconds && temp->next->timestamp_useconds < new_node->timestamp_useconds))){
        temp = temp->next;
    }

    new_node->next = temp->next;
    temp->next = new_node;

    return;
}

int8_t* messageQueue::pop(){
    if(isEmpty()){
        return nullptr;
    }
    int8_t *temp = new int8_t[MESSAGE_SIZE];
    memcpy(temp,head->arr,MESSAGE_SIZE);
    struct node *rm = head;
    head = head->next;
    delete(rm);
    return temp;
}