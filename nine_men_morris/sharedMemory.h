#ifndef NINE_MEN_MORRIS_SHAREDMEMORY_H
#define NINE_MEN_MORRIS_SHAREDMEMORY_H

#include "game-info.h"
#include "player-info.h"
#include "config-info.h"
#include <sys/types.h>
typedef struct {
    int numberOfPlayers; // Anzahl der Spieler
    pid_t connectorId; // Prozess-IDs der beiden Prozesse
    pid_t thinkerId;
    int pipeRead;
    int pipeWrite;
    int winnerId;
    long maxTime;
    int thinkFlag;
    int captureFlag;
    int playerId;
    int anotherPlayerId;
    char gameName[1024];
    int boardShm;
    int amountOfPiece;
}sharedMemoryInfo;

int creatShm(int shmSize);
void *attachShm(int shmID);
int detachShm(const void *shm_address);
void initStruct(sharedMemoryInfo *sharedStruct,gameInfo *game, playerInfo *player,int pipeFd[2]);
int copyStructInShm(sharedMemoryInfo *sharedStruct,sharedMemoryInfo *shm);
int deleteShm(int shmId);
int creatSemaphore(void);
void lockSemaphore(int semId);
void signalSemaphore(int semID);
void deleteSemaphore(int semId);
void printShm(sharedMemoryInfo *shm);
#endif