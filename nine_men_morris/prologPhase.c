#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "game-info.h"
#include "player-info.h"
#include "config-info.h"
#include "prologPhase.h"



int receiveMessageFromServer(char* msgBuffer, int socket){
   int currentBytes;
    int returnVal;
    //with for loop can read message that sent by server byte by byte, because the length of the message is unsure
    //the loop can only stop when recv() method has failed or read the end of the message, which  ends with \n
    //it will also stop when the buffer for saving message is already full
    for (currentBytes = 0; currentBytes < 1024 - 2; currentBytes++) {
        returnVal = recv(socket, msgBuffer + currentBytes, 1, 0);
        if (returnVal< 0) {
            fprintf(stderr, "can not receive message from server");
            return -1;
        }
        if (0 == returnVal || ('\n' == msgBuffer[currentBytes])) {
            break;
        }
    }
    return EXIT_SUCCESS;
}

// this method helps to check type of methods
int startWith(char *msgBuffer,char startLetter){
    //message can be started with + or - , if started with + then need to see type of message
    if(*msgBuffer=='+'){
        //msgBuffer point to the first element of the char array,so msgBuffer+2 always point to the third element
        if(*(msgBuffer+2)==startLetter){
            return 1;
        } else{
            return 2;
        }
    } else if(*msgBuffer=='-'){
        return 3;
    } else{
        return 4;
    }
}

//check if client gets both main and major server version and if the main version of server is 3
int checkVersion(char *msgBuffer){
    int mainVersion,majorVersion;
    int matchNumber=sscanf(msgBuffer,"+ MNM Gameserver v%d.%d",&mainVersion,&majorVersion);
    // need to get both main and major version
    if(matchNumber!=2){
        fprintf(stderr,"failure by get server version\n");
        return EXIT_FAILURE;
    }
    if(mainVersion!=3){
        fprintf(stderr,"wrong server version, server main version should be 3\n");
        return EXIT_FAILURE;
    }
    //clean the buffer so can store the next message
    memset(msgBuffer,0,1024*sizeof (char ));
    return EXIT_SUCCESS;
}

//check if the game kind name that sent by server is correct
int checkGameKindName(char *msgBuffer,configInfo *config){
    char *containStr;
    //check if message contains the game kind name
    containStr=strstr(msgBuffer,config->gameKindName);
    memset(msgBuffer,0,1024*sizeof (char ));
    if(containStr){
        //containStr!=null, means contains
        return EXIT_SUCCESS;
    } else{
        fprintf(stderr,"the game kind name was wrong");
        return EXIT_FAILURE;
    }
}

//check the total game number that sent by server
int checkTotalNumber(char *msgBuffer){
    //check if total number is 2
    int returnValue=strcmp(msgBuffer,"+ TOTAL 2\n");
    memset(msgBuffer,0,1024*sizeof (char ));
    if(returnValue==0){
        return EXIT_SUCCESS;
    } else{
        fprintf(stderr,"wrong total number\n");
        return EXIT_FAILURE;
    }
}

//store the game name that sent by server
int storeGameName(gameInfo *game, char *msgBuffer){
    int varSize= (int)strlen(msgBuffer)-3;
    //create space to store game name
    game->gameName=(char *) malloc((varSize+1)*sizeof (char ));
    if(!(game->gameName)){
        fprintf(stderr,"no space to store game name");
        return EXIT_FAILURE;
    }
    strncpy(game->gameName,msgBuffer+2,varSize);
    *(game->gameName+varSize)='\0';
    memset(msgBuffer,0,1024*sizeof (char ));
    return EXIT_SUCCESS;
}

//store the player name and actual id that sent by server
int storePlayerInfo(playerInfo *player,char *msgBuffer) {
    int varSize= (int)strlen(msgBuffer)-9;
    char temp[1];
    player->playerName=(char *) malloc((varSize+1)*sizeof (char ));
    if(!(player->playerName)){
        fprintf(stderr,"no space to store player name");
        return EXIT_FAILURE;
    } else{
        //get the player information sent by server and store into player name and player id
        strncpy(temp,msgBuffer+6,1);
        player->playerId= atoi(temp);
        strncpy(player->playerName,msgBuffer+8,varSize);
        *(player->playerName+varSize)='\0';
        memset(msgBuffer,0,1024*sizeof (char ));
    }
    return EXIT_SUCCESS;
}

//store the information of another player
int storeAnotherPlayerInfo(char *msgBuffer,playerInfo *player){
    int varSize= (int)strlen(msgBuffer)-7;
    char tempStatus[1];
    char tempId[1];
    player->anotherPlayerName=(char *) malloc((varSize+1)*sizeof (char ));
    if(!(player->anotherPlayerName)){
        fprintf(stderr,"no space to store another player name");
        return EXIT_FAILURE;
    } else{
        strncpy(tempId,msgBuffer+2,1);
        player->anotherPlayerId= atoi(tempId);
        strncpy(player->anotherPlayerName,msgBuffer+4,varSize);
        *(player->anotherPlayerName+varSize)='\0';
        strncpy(tempStatus,msgBuffer+9,1);
        player->anotherPlayerStatus= atoi(tempStatus);
        memset(msgBuffer,0,1024*sizeof (char ));
    }
    return EXIT_SUCCESS;
}

//if client gets the correct server version, it should send client version back(also with main version of 3)
int sendVersion(int socket, char *msgBuffer){
    //here don't need to clean the message buffer at first because already cleaned in checkVersion method
    int answerSize=strlen("VERSION 3.1\n");
    char tempVersion[13]="VERSION 3.1\n";
    if((send(socket,tempVersion,answerSize,0))<0){
        fprintf(stderr,"send client version to server failed\n");
        return EXIT_FAILURE;
    }
    // fprintf(stderr,"send client version to server successfully\n");
    return EXIT_SUCCESS;
}

//send game id to server after get the message: + Client version accepted - please send Game-ID to join
int sendGameId(int socket, char *msgBuffer,gameInfo *game){
    memset(msgBuffer,0,1024*sizeof (char ));
    char des[20]="ID ";
    //combine game id and \n with "ID "
    strcat(des,game->gameID);
    strcat(des,"\n");
    if((send(socket,des, strlen(des),0))<0){
        fprintf(stderr,"send game id to server failed\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

//send the player id that selected by client to server(selected with command -p)
int sendPlayerId(playerInfo *player,int socket, char * msgBuffer){
    memset(msgBuffer,0,1024*sizeof (char ));
    //when player id is -1 means client did not type the player id that he want through -p
    char des[10]="PLAYER";
    if(player->playerId==-1){
        strcat(des,"\n");
    } else{
        char source[2];
        strcat(des," ");
        sprintf(source, "%d", player->playerId);
        strcat(des,source);
        strcat(des,"\n");
    }
    if((send(socket,des, strlen(des),0))<0){
        fprintf(stderr,"send player id to server failed\n");
        return EXIT_FAILURE;
    }
    memset(msgBuffer,0,1024*sizeof (char ));
    return EXIT_SUCCESS;
}

//check the type and correctness of the message and store the information which was needed
int prolog(int socket,gameInfo *game,playerInfo *player,configInfo *config){
    char msgBuffer[1024];
    memset(msgBuffer,0,1024*sizeof (char ));
    while(true){
        if(-1 !=receiveMessageFromServer(msgBuffer,socket)){
            if(1==startWith(msgBuffer,'M')){
                //if the server version right then send the client version back
                if(1==checkVersion(msgBuffer)){
                    return EXIT_FAILURE;
                } else{
                    if(1==sendVersion(socket,msgBuffer)){
                        return EXIT_FAILURE;
                    }
                }
            } else if(1==startWith(msgBuffer,'C')){
                if(1==sendGameId(socket,msgBuffer,game)){

                    return EXIT_FAILURE;
                }
            } else if(1==startWith(msgBuffer,'P')){
                //once the message start with PLAYING, then server will definitely send a message with game name,so need
                //to read message again without giving feedback to server
                if(0==checkGameKindName(msgBuffer,config)){
                    if(-1!=receiveMessageFromServer(msgBuffer,socket)){
                        if(0==storeGameName(game,msgBuffer)){
                            //send the player id that pick by client to server
                            if(1== sendPlayerId(player,socket,msgBuffer)){
                                return EXIT_FAILURE;
                            }
                        }
                    } else{
                        return EXIT_FAILURE;
                    }
                } else{
                    return EXIT_FAILURE;
                }
            } else if(1== startWith(msgBuffer,'Y')) {
                if(0==storePlayerInfo(player,msgBuffer)){
                    // after send the player name and id server will definitely send a message with total number of
                    // player, so need to read message again without giving feedback to server
                    if(-1!=receiveMessageFromServer(msgBuffer,socket)){
                        if(0!=checkTotalNumber(msgBuffer)){
                            return EXIT_FAILURE;
                        }
                    }
                    //if the code run to here means server will definitely send a message with status of another player
                    // so need to read message again
                    if(-1!= receiveMessageFromServer(msgBuffer,socket)){
                        if(1==storeAnotherPlayerInfo(msgBuffer,player)){
                            return EXIT_FAILURE;
                        }
                    }
                }
            }else if(1== startWith(msgBuffer,'E')){
                //end of the prolog phase, clear the message buffer last time and jump out of the loop
                memset(msgBuffer,0,1024*sizeof (char ));
                break;
            }else if(1== startWith(msgBuffer,'T')) {
                fprintf(stderr, "error:%s,may caused by time out\n", msgBuffer);
                return EXIT_FAILURE;
            }else if (1== startWith(msgBuffer, 'A'))  {
                // printf("Successfully");
                continue;
            }else {
                fprintf(stderr,"unknown message: %s\n",msgBuffer);
                return EXIT_FAILURE;
            }
        } else{
            fprintf(stderr,"can not receive message from server");
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}


