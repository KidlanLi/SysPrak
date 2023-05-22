#include <stdio.h>
#include "getopt.h"
#include "performConnection.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "connectServer.h"
#include "config-info.h"
#include "config.h"
#include "sharedMemory.h"
#include <signal.h>
#include "thinker.h"
#include <stdbool.h>
#include <wait.h>
//release the item
void gameInfoFree(gameInfo *game, playerInfo *player){
    free(game->gameName);
    free(player->playerName);
    free(player->anotherPlayerName);
}

int handleSignal(int semId,sharedMemoryInfo *shm){
    sigset_t sigset;
    int returnVal;
    if (-1 == sigemptyset(&sigset)) {
        fprintf(stderr, "can not initialize signal set.\n");
        return EXIT_FAILURE;
    }
    if (-1 == sigaddset(&sigset, SIGUSR1)) {
        fprintf(stderr, "can not add signal SIGUSR1 to set.\n");
        return EXIT_FAILURE;
    }
    if (-1 == sigaddset(&sigset, SIGCHLD)) {
        fprintf(stderr, "failed to add signal SIGCHLD to set.\n");
        return EXIT_FAILURE;
    }
    sigprocmask(SIG_BLOCK, &sigset, NULL);
    while (true) {
        if (sigwait(&sigset, &returnVal)) {
            fprintf(stderr, "can not receive signal SIGCHLD and SIGUSR1\n");
            return EXIT_FAILURE;
        }
        //start to think
        if (SIGUSR1 == returnVal) {
            lockSemaphore(semId);
            if (shm->thinkFlag == 1) {
                shm->thinkFlag = 0;
                signalSemaphore(semId);
                think(semId, shm);
            } else {
                fprintf(stderr, "Thinker: info has not been stored but had received SIGUSR1\n");
                signalSemaphore(semId);
            }
        } else if (SIGCHLD == returnVal) {
            //received SIGCHLD, child process status has changed
            fprintf(stderr, "connector process has terminated, stop thinker\n");
            break;
        }
    }
    return EXIT_SUCCESS;
}
/**
 * main.c is responsible for specifying the sysprak-client file (in the MakeFile file, specify OBJ = sysprak-client)
 * which contains the main method in the format . /sysprak-client -g <GAME-ID> -p <{1,2}> -c "config.conf"
 * -g GAME-ID: specified by the server
 * -p Player-ID: selected by the user, an error will be reported if an already selected player-ID is selected
 * -c Config-Path: "client.conf" by default, the path to the config file can be changed by typing -c "xx.conf"
 * */
int main(int argc, char *argv[]) {
    // The game ID has no initial value and must be passed in via -g
    char gameID[14];
    char *defaultConf = "client.conf";

    // Player-ID: without -p ->  a random value among 0,1: 0 for Alice，1 for Bob
    playerInfo player;
    player.playerId = -1;
    gameInfo game;

    // Config-Path: without -c ->  client.conf is passed in as the config file by default
    char *config_path = (char *) malloc(11 * sizeof(char));
    configInfo conf = config(NULL);
    int opt,shmId,semId,socket,fd[2];
    sharedMemoryInfo *shm;
    pid_t pid;
    /**
     * ` ./sysprak-client -g game number -p {1:Alice, 2:Bob|if none -p -> random one of 1 2} -c "config file address" `
     * */
    while ((opt = getopt(argc, argv, "g:p:c:")) != -1) {
        switch (opt) {
            case 'g':
                strcpy(gameID, optarg);
                //(&game)->gameID=(char *) malloc(14*sizeof (char ));
                (&game)->gameID = gameID;
                break;
            case 'p':
                if (strcmp(optarg, "1") == 0 || strcmp(optarg, "2") == 0) {
                    player.playerId = atoi(optarg) - 1;
                } else {
                    printf("ERROR: Please specify <GAME-ID> and <Player> with ./sysprak-client -g <GAME-ID> -p <Player>.\n");
                    return 1;
                }
                break;
            case 'c':

                if (strcmp(optarg,defaultConf)!=0){
                    configInfoClear(&conf);

                    strcpy(config_path, optarg);
                    conf = config(config_path);
                }
                configPrint(conf);

                break;
            default:
                printf("ERROR: Please specify <GAME-ID> and <Player> with ./sysprak-client -g <GAME-ID> -p <Player>.\n");
                return 1;
        }

    }

    //create shared memory
    shmId=creatShm(sizeof(sharedMemoryInfo));
    semId=creatSemaphore();
    if(shmId<0){
        exit(EXIT_FAILURE);
    }
    shm= (sharedMemoryInfo *)attachShm(shmId);
    if(shm==NULL){
        gameInfoFree(&game,&player);
        deleteSemaphore(semId);
        deleteShm(shmId);
        exit(EXIT_FAILURE);
    }

    //create pipe
    if((pipe(fd))<0){
        fprintf(stderr,"can not create pipe");
        deleteSemaphore(semId);
        deleteShm(shmId);
        gameInfoFree(&game,&player);
        exit(EXIT_FAILURE);
    }

    //create 2 process
    switch (pid=fork()) {
        case -1:
            fprintf(stderr,"failed to create process");
            gameInfoFree(&game,&player);
            deleteSemaphore(semId);
            deleteShm(shmId);
            exit(EXIT_FAILURE);
        case 0:
            //Connector

            //close write side of pipe
            if(0>close(fd[1])){
                fprintf(stderr,"can not close write side of pipe");
                gameInfoFree(&game,&player);
                deleteSemaphore(semId);
                deleteShm(shmId);
                exit(EXIT_FAILURE);
            }
            //connect with server
            socket = connectServer(&conf);
            if(socket<0){
                gameInfoFree(&game,&player);
                deleteSemaphore(semId);
                deleteShm(shmId);
                exit(EXIT_FAILURE);
            }
            //connect successfully
            if((performConnection(socket, &game, &player, &conf,shm,semId,fd))!=0){
                closeSock(socket);
                gameInfoFree(&game,&player);
                deleteShm(shm->boardShm);
                deleteShm(shmId);
                deleteSemaphore(semId);
                exit(EXIT_FAILURE);
            }
            //all phase is end successfully,can disconnect with server now, also need to clean up memory
            closeSock(socket);
            gameInfoFree(&game,&player);
            break;
        default:
            //thinker
            //close read side of pipe
            if(0>close(fd[0])){
                fprintf(stderr,"can not close read side of pipe");
                gameInfoFree(&game,&player);
                deleteSemaphore(semId);
                deleteShm(shmId);
                exit(EXIT_FAILURE);
            }
            //handle signal like SIGCHLD and SIGUSR1
            if(0!=handleSignal(semId,shm)){
                gameInfoFree(&game,&player);
                deleteSemaphore(semId);
                deleteShm(shmId);
                exit(EXIT_FAILURE);
            }
            wait(NULL);
            deleteShm(shm->boardShm);
            deleteShm(shmId);
            deleteSemaphore(semId);
    }
    free(config_path);
    configInfoClear(&conf);
    return 0;
}

