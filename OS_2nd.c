#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include  <sys/types.h>
#include <pthread.h>
#include <semaphore.h>


#define NUM_PROCESSES 5
#define MAX_NUMBER 100


union semun1 {
    int val; 
    struct semid_ds* buf; 
    unsigned short* array;
    struct seminfo*  _buf;
};

key_t key;
int semid;
union semun1 arg;

int allocateSem()
{
    semid = semget(IPC_PRIVATE, (NUM_PROCESSES+1), 0666 | IPC_CREAT);
}

int deallocateSem() //deallocte to each semaphore
{
    if (semctl (semid, 0, IPC_RMID, arg)== -1)
    {
    perror("semctl");
    exit(-1);
    }
}

int initSem()
{

    for(int i=1; i<=NUM_PROCESSES; i++) //define each sempaphore 4,3,2,1
    {
        arg.val = 5-i; 
        if(-1 == semctl(semid, i, SETVAL, arg))
        {
            perror("semctlX");
            exit(1);
        }
    }
}

int changeSem (int sem_num, int change)
{
    struct sembuf sops;
    sops.sem_num =  sem_num; 
    sops.sem_op = change; 
    sops.sem_flg = 0; 
    semop(semid, &sops, 1);
}

int incSem(int i)
{
    changeSem(i, 1); //increase sem val by 1 (+1)
}

int waitSem(int i) // decrease sem val by 4 (-4) 
{
 changeSem(i, -4);
}

int nextStep(int i) //using incsem function to all semaphores (beside i!=j)
{
    for (int j = 1; j<=(NUM_PROCESSES+1); ++j) 
    {
        if (j!=i)
        incSem(j);
    }
}

int process(int sem_num) // deallocte- which process need to deallocate after itself
{
    int i = sem_num;
    while (i< (MAX_NUMBER+1))//print num from 1 to 100
    {
        waitSem(sem_num); //wait to our sem
        printf("%d\n", i);//print num
        nextStep(sem_num);//do signal to the rest
        i+=NUM_PROCESSES; //increase by 5
       if (i == 15){
       exit(0);
       }
        
    }
}

int main (int argc, char* argv[])
{
    allocateSem();
    initSem();
    for (int i=1; i<=(NUM_PROCESSES-1); i++) //create only 4 processes
    {
        if(!(fork()))
        {
            process(i);
            exit(1);
        }
    }

    process(NUM_PROCESSES);//the fifth process , father process 
    deallocateSem();//the father process responsible to deallocation
    printf("I am here \n");
 }
