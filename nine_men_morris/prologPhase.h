#ifndef PROLOGPHASE_H_
#define PROLOGPHASE_H_


#include "game-info.h"
#include "player-info.h"
#include "config-info.h"



int prolog(int socket,gameInfo *game, playerInfo *player, configInfo *config);

int receiveMessageFromServer(char* msgBuffer, int socket);
#endif