#include <iostream> 
#include<pthread.h> 
#include<semaphore.h> 
#include <unistd.h>
#include <fstream> 



int variable, reader_count;
sem_t critical_access;
sem_t reader_count_access;

std::ifstream fin;
std::ofstream fout;

void* Reader(void *args){

    //Pre Reader Count Critical Section Starts
    sem_wait(&reader_count_access);
    reader_count++;
    if(reader_count==1){
        sem_wait(&critical_access);
    }
    sem_post(&reader_count_access);
    //Pre Reader Count Critical Section Ends
    //Critical Section Starts

    // printf("Reader : %d\n",variable);
    // std::cout<<variable<<std::endl;
    std::ifstream fin("readerWriter.txt"); 
    std::string line;

  
    // Execute a loop until EOF (End of File) 
    while(getline(fin, line)){
        // Read a Line from File 
        // Print line in Console 
        std::cout << line << std::endl; 
    } 
  
    // Close the file 
    fin.close();
    //Critical Section Ends



    //Post Reader Count Critical Section Starts
    sem_wait(&reader_count_access);
    reader_count--;
    if(reader_count==0){
        sem_post(&critical_access);
    }
    sem_post(&reader_count_access);
    //Post Reader Count Critical Section Ends


    return NULL;
}


void* Writer(void* args){
    //Critical Section Starts
    sem_wait(&critical_access);
    // printf("Writer Incremented !\n");
    // std::cout<<"Increased value of variable!"<<std::endl;
    variable++;

    std::ofstream fout;
    fout.open("readerWriter.txt",std::ios::trunc);
    
    fout << "Variable : " << variable << "\n"; 
  
    // Close the File 
    fout.close();

    sem_post(&critical_access);
    //Critical Section Ends


    return NULL;
}

int main(){
    


    sem_init(&critical_access, 0, 1);
    sem_init(&reader_count_access, 0, 1);
    pthread_t rd[5], wr[5];

    for(int i = 0 ; i < 5 ; i++){
        pthread_create(&rd[i],NULL,Writer,NULL); 
        pthread_create(&wr[i],NULL,Reader,NULL);
    }

    for(int i = 0 ; i < 5 ; i++){
        pthread_join(rd[i],NULL); 
        pthread_join(wr[i],NULL);
    }

    sem_destroy(&critical_access); 
    sem_destroy(&reader_count_access); 
}