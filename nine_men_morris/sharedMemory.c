#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <unistd.h>
#include <string.h>
#include "game-info.h"
#include "player-info.h"
#include "sharedMemory.h"
#define WAIT -1
#define SIGNAL 1

//create a new shared memory
int creatShm(int shmSize){
    int shmId;
    shmId=shmget(IPC_PRIVATE,shmSize,IPC_CREAT|IPC_EXCL|0644);
    if(shmId<0){
        //because of ICP_EXCL it will return -1 when a shared memory already exists
        fprintf(stderr,"shared memory already exist\n");
        return -1;
    }
    return shmId;
}

//attach shared memory with process
void *attachShm(int shmID){
    void *shm_address= shmat(shmID,NULL,0);
    if(shm_address==(void *)-1){
        fprintf(stderr,"failed to attach shared memory with process\n");
        return NULL;
    }
    return shm_address;
}

//detach shared memory with process
int detachShm(const void *shm_address){
    int result=shmdt(shm_address);
    if(result==-1){
        fprintf(stderr,"failed to detach shared memory with process\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

//later add fd to parameter,this methode is trying to init a struct that later copy to shared memory
void initStruct( sharedMemoryInfo *sharedStruct,gameInfo *game,playerInfo *player,int pipeFd[2]){
    memset(sharedStruct, 0, sizeof(sharedMemoryInfo));
    strcpy(sharedStruct->gameName, game->gameName);
    sharedStruct->connectorId = getpid();
    sharedStruct->thinkerId = getppid();
    sharedStruct->numberOfPlayers = 2;
    sharedStruct->captureFlag = 0;
    sharedStruct->playerId = player->playerId;
    sharedStruct->anotherPlayerId = player->anotherPlayerId;
    sharedStruct->pipeRead=pipeFd[0];
    sharedStruct->pipeWrite=pipeFd[1];
}

int copyStructInShm(sharedMemoryInfo *sharedStruct,sharedMemoryInfo *shm){
    if((memcpy(shm,sharedStruct,sizeof (sharedMemoryInfo))!=shm)){
        fprintf(stderr,"failed by copy shared struct to shared memory\n");
        return 1;
    }
    return 0;
}

int deleteShm(int shmId){
    int result=shmctl(shmId,IPC_RMID,NULL);
    if(result==-1){
        fprintf(stderr,"can not delete shared memory\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int creatSemaphore(){
    //NSEMS=1
    int semId=semget(IPC_PRIVATE,1,IPC_CREAT|0644);
    int result;
    if(semId==-1){
        fprintf(stderr,"failed by create semaphore\n");
        return -1;
    }
    result= semctl(semId,0,SETVAL,1);
    if(result==-1){
        fprintf(stderr,"failed by init semaphore\n");
        return -1;
    }
    return semId;
}


void lockSemaphore(int semId){
    struct sembuf wait;
    wait.sem_num=0;
    wait.sem_op=WAIT;
    wait.sem_flg=0;
    if(-1==(semop(semId,&wait,1))){
        fprintf(stderr,"can not lock semaphore\n");
    }
}

void signalSemaphore(int semID){
    struct sembuf signal;
    signal.sem_num=0;
    signal.sem_op=SIGNAL;
    signal.sem_flg=0;
    if(-1==(semop(semID,&signal,1))){
        fprintf(stderr,"can not signal semaphore\n");
    }
}

void deleteSemaphore(int semId){
    if(-1==(semctl(semId,0,IPC_RMID,0))){
        fprintf(stderr,"can not delete semaphore\n");
    }
}

void printShm(sharedMemoryInfo *shm){
    printf("another player id:%d\n",shm->anotherPlayerId);
    printf("player id:%d\n",shm->playerId);
    printf("game name:%s\n",shm->gameName);
    printf("number of player:%d\n",shm->numberOfPlayers);
    printf("process thinker id:%d\n",shm->thinkerId);
    printf("process connector id:%d\n",shm->connectorId);
}