#ifndef THINKER_H_
#define THINKER_H_
#include "sharedMemory.h"
#include "gamePhase.h"


int think(int semId, sharedMemoryInfo *shm);
void initBoard(char currentPlayerBoard[7][7], char anotherPlayerBoard[7][7]);
void printBoard(char board[13][24]);
void
cpyPieceInfoToBoard(char currentPlayerBoard[7][7], char anotherPlayerBoard[7][7], char outputBoard[13][24],int semId,
                    piece *pieceList,
                    sharedMemoryInfo *shm);
#endif