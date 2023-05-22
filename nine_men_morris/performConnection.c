#include "game-info.h"
#include "player-info.h"
#include "config-info.h"
#include <stdio.h>
#include <stdlib.h>
#include "prologPhase.h"
#include "sharedMemory.h"
#include "gamePhase.h"
#include "gameOver.h"
void printInformation(playerInfo *player,configInfo *config,gameInfo *game){

    printf("---------------------------------\n");
    printf("Game's information\n");
    printf("---------------------------------\n");
    printf("The game kind is: %s\n",config->gameKindName);
    printf("The game ID is: %s\n", (game)->gameID);
    printf("The lobby's name is: %s\n", (game)->gameName);
    printf("---------------------------------\n");
    printf("Players' information\n");
    printf("---------------------------------\n");
    printf("Your player id is: %d\n", (player)->playerId);
    printf("Your name is: %s\n", (player)->playerName);
    printf("Another player's id is: %d\n", (player)->anotherPlayerId);
    printf("Another player's name is: %s\n", (player)->anotherPlayerName);
    printf("---------------------------------\n");
    printf("Game is begin.\n");
    printf("---------------------------------\n");
}

int performConnection(int socket, gameInfo *game, playerInfo *player, configInfo *config,sharedMemoryInfo *shm, int semId,int pipeFd[2]){
    sharedMemoryInfo sharedStruct;
    if((prolog(socket, game, player, config))==1){
        fprintf(stderr,"failed by prolog phase\n");
        return EXIT_FAILURE;
    }
    printInformation(player,config,game);
    initStruct(&sharedStruct,game,player,pipeFd);
    lockSemaphore(semId);
    copyStructInShm(&sharedStruct,shm);
    signalSemaphore(semId);
    if((gamePhase(socket, semId, shm))==1){
        fprintf(stderr,"failed by game phase\n");
        return EXIT_FAILURE;
    }
    if((gameOver(socket, semId, shm))==1){
        fprintf(stderr,"failed by game over phase\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}



    