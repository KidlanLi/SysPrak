#ifndef PERFORMCONNECTION_H_
#define PERFORMCONNECTION_H_

#include "game-info.h"
#include "player-info.h"
#include "config-info.h"
#include "sharedMemory.h"
int performConnection(int socket,gameInfo *game, playerInfo *player, configInfo *config,sharedMemoryInfo *shm, int semId,int pipeFd[2]);
#endif