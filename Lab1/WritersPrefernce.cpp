#include <iostream> 
#include<pthread.h> 
#include<semaphore.h> 
#include <unistd.h>
#include <fstream> 



int variable = 0, reader_count = 0, writer_count = 0;
sem_t critical_access;
sem_t writer_count_access;
sem_t early_bird_access;
sem_t reader_count_access;

std::ifstream fin;
std::ofstream fout;

void* Reader(void *args){


    sem_wait(&early_bird_access);
    //Pre Reader Count Critical Section Starts
    sem_wait(&reader_count_access);
    reader_count++;
    if(reader_count==1){
        sem_wait(&critical_access);
    }
    sem_post(&reader_count_access);
    sem_post(&early_bird_access);
    //Pre Reader Count Critical Section Ends
    //Critical Section Starts

    // printf("Reader : %d\n",variable);
    // std::cout<<variable<<std::endl;
    std::ifstream fin("readerWriter2.txt"); 
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
    sem_wait(&writer_count_access);
    writer_count++;
    if(writer_count == 1){
        sem_wait(&early_bird_access);
    }
    sem_post(&writer_count_access);

    //Critical Section Starts
    sem_wait(&critical_access);
    // printf("Writer Incremented !\n");
    // std::cout<<"Increased value of variable!"<<std::endl;
    variable++;
    std::ofstream fout;
    fout.open("readerWriter2.txt",std::ios::trunc);
    fout << "Variable : " << variable << "\n";
    // Close the File 
    fout.close();
    sem_post(&critical_access);
    //Critical Section Ends

    sem_wait(&writer_count_access);
    writer_count--;
    if(writer_count == 0){
        sem_post(&early_bird_access);
    }
    sem_post(&writer_count_access);

    return NULL;
}

int main(){
    std::ofstream fout;
    fout.open("readerWriter2.txt",std::ios::trunc);
    fout << "Variable : " << variable << "\n";
    // Close the File 
    fout.close();

    sem_init(&critical_access, 0, 1);
    sem_init(&reader_count_access, 0, 1);
    sem_init(&writer_count_access, 0, 1);
    sem_init(&early_bird_access, 0, 1);

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
    sem_destroy(&writer_count_access); 
    sem_destroy(&early_bird_access); 

}