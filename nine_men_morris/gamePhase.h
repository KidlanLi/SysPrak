#ifndef GAMEPHASE_H_
#define GAMEPHASE_H_
#include "sharedMemory.h"

typedef struct {
    char status;
    char side;
    int position;
    int pieceNumber;
    int ownerOfPiece;
}piece;

int gamePhase(int socket, int semId, sharedMemoryInfo *shm);
int handlePieceList(char *msgBuffer, int socket, int semId, sharedMemoryInfo *shm,int flag);
int defineMsgType(char *msgBuffer);
#endif