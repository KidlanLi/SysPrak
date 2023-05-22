#include "prologPhase.h"
#include "sharedMemory.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "gamePhase.h"
#include <sys/socket.h>
#include <signal.h>
#include <sys/epoll.h>
#include <stdbool.h>
#include <unistd.h>

int defineMsgType(char *msgBuffer) {
    //message can be started with + or - , if started with + then need to see type of message
    if (*msgBuffer == '+') {
        if (*(msgBuffer + 2) == 'M') {
            //+ MOVE
            return 1;
        } else if (*(msgBuffer + 2) == 'C') {
            //+ CAPTURE
            return 2;
        } else if (*(msgBuffer + 2) == 'P' && *(msgBuffer + 7) == 'L') {
            //+ PIECELIST
            return 3;
        } else if (*(msgBuffer + 2) == 'E') {
            //+ ENDPIECELIST
            return 4;
        } else if (*(msgBuffer + 2) == 'O') {
            //+ OKTHINK
            return 5;
        } else if (*(msgBuffer + 2) == 'W') {
            //+ WAIT
            return 6;
        } else if (*(msgBuffer + 2) == 'G') {
            //+ GAMEOVER
            return 7;
        } else {
            //+ PIECE
            return 8;
        }
    } else if (*msgBuffer == '-') {
        // - ERROR
        return 9;
    } else {
        return 10;
    }
}

void storeMaxMoveTime(char *msgBuffer, sharedMemoryInfo *shm, int semId) {
    int varSize = (int) strlen(msgBuffer) - 8;
    char temp[varSize+1];
    strncpy(temp, msgBuffer + 7, varSize);
    temp[varSize]='\0';
    lockSemaphore(semId);
    shm->maxTime = atoi(temp);
    signalSemaphore(semId);
    memset(msgBuffer, 0, 1024 * sizeof(char));
}

void storeCapturePiece(char *msgBuffer, sharedMemoryInfo *shm, int semId) {
    char temp[2];
    strncpy(temp, msgBuffer + 10, 1);
    temp[1] = '\0';
    lockSemaphore(semId);
    shm->captureFlag = atoi(temp);
    signalSemaphore(semId);
    memset(msgBuffer, 0, 1024 * sizeof(char));
}

int sendThinking(int socket, char *msgBuffer) {
    //clean the message buffer so it can receive next message
    memset(msgBuffer, 0, 1024 * sizeof(char));
    int answerSize = strlen("THINKING\n");
    char tempAnswer[10] = "THINKING\n";
    if ((send(socket, tempAnswer, answerSize, 0)) < 0) {
        fprintf(stderr, "send message thinking to server failed\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int sendSequenceToServer(int socket, sharedMemoryInfo *shm, int semId) {
    char sequence[1024];
    char message[1024];
    memset(sequence, 0, 1024 * sizeof(char));
    int varSize;
    ssize_t readResult = 0;
    lockSemaphore(semId);
    if (-1 == (readResult = read(shm->pipeRead, sequence, 1024))) {
        fprintf(stderr, "Failed to read message from pipe, %ld\n", readResult);
        return EXIT_FAILURE;
    }
    signalSemaphore(semId);
    memset(message, 0, 1024 * sizeof(char));
    if (0 > (varSize = snprintf(message, 1024, "PLAY %s\n", sequence))) {
        fprintf(stderr, "can not put sequence into message buffer\n");
        return EXIT_FAILURE;
    }
    if (0 > send(socket, message, varSize, 0)) {
        fprintf(stderr, "can not send sequence to server\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int handleOkThinking(int semId, sharedMemoryInfo *shm, int socket, char *msgBuffer) {
    memset(msgBuffer, 0, 1024 * sizeof(char));

    //wake up thinker
    lockSemaphore(semId);
    shm->thinkFlag = 1;
    kill(shm->thinkerId, SIGUSR1);
    signalSemaphore(semId);

    //epoll
    int epollFd, pipeFd, fds;
    struct epoll_event event, epollEvents[10];
    memset(&event, 0, sizeof(struct epoll_event));
    epollFd = epoll_create1(0);
    if (epollFd == -1) {
        fprintf(stderr, "can not generate an epoll instance\n");
        return EXIT_FAILURE;
    }
    //listen the socket(server)
    event.events = EPOLLIN | EPOLLHUP;
    event.data.fd = socket;
    if (-1 == epoll_ctl(epollFd, EPOLL_CTL_ADD, socket, &event)) {
        fprintf(stderr, "can not add socket to epoll\n");
        return EXIT_FAILURE;
    }
    lockSemaphore(semId);
    pipeFd = shm->pipeRead;
    signalSemaphore(semId);
    event.data.fd = pipeFd;
    //listen the pipe(move calculated and send to pipe by thinker)
    if (-1 == epoll_ctl(epollFd, EPOLL_CTL_ADD, pipeFd, &event)) {
        fprintf(stderr, "can not add pipe to epoll\n");
        return EXIT_FAILURE;
    }
    fds = epoll_wait(epollFd, epollEvents, 10, -1);
    if (fds == -1) {
        fprintf(stderr, "can not get ready file descriptors\n");
        return EXIT_FAILURE;
    }
    for (int currentFd = 0; currentFd < fds; currentFd++) {
        if (epollEvents[currentFd].data.fd == socket) {
            if (epollEvents[currentFd].events & EPOLLHUP) {
                fprintf(stderr, "connection with server was interrupted\n");
                return EXIT_FAILURE;
            }
            //- time out
            fprintf(stderr, "get time out message from server before read from pipe\n");
            return EXIT_FAILURE;
        } else if (epollEvents[currentFd].data.fd == pipeFd) {
            if (epollEvents[currentFd].events & EPOLLIN) {
                break;
            }
            if (epollEvents[currentFd].events & EPOLLHUP) {
                fprintf(stderr, "connection between server was interrupted,can not get message from pipe\n");
                return EXIT_FAILURE;
            }
        }
    }

    //send sequence(mover or capture or jump or put) to server
    if (1 == sendSequenceToServer(socket, shm, semId)) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int handlePieceList(char *msgBuffer, int socket, int semId, sharedMemoryInfo *shm, int flag) {
    piece *pieceList;
    int boardShmId, loopNum;
    int varSize = (int) strlen(msgBuffer) - 15;
    char tempAmountOfPlayer[2], tempAmountOfPiece[varSize + 1];
    strncpy(tempAmountOfPlayer, msgBuffer + 12, 1);
    tempAmountOfPlayer[1] = '\0';
    strncpy(tempAmountOfPiece, msgBuffer + 14, varSize);
    tempAmountOfPiece[varSize] = '\0';
    lockSemaphore(semId);
    shm->amountOfPiece = atoi(tempAmountOfPlayer) * atoi(tempAmountOfPiece);
    loopNum = shm->amountOfPiece;
    boardShmId = creatShm(shm->amountOfPiece * sizeof(piece));
    signalSemaphore(semId);
    if (boardShmId == -1) {
        fprintf(stderr, "can not create board shared memory\n");
        return EXIT_FAILURE;
    }

    lockSemaphore(semId);
    //with this id can attach the shared memory with piece info so can create a board
    shm->boardShm = boardShmId;
    shm->numberOfPlayers = atoi(tempAmountOfPlayer);
    pieceList = (piece *) attachShm(boardShmId);
    //create board shared memory failed
    if (NULL == pieceList) {
        deleteShm(boardShmId);
        signalSemaphore(semId);
        return EXIT_FAILURE;
    }
    signalSemaphore(semId);

    //clear message buffer before read all piece information
    memset(msgBuffer, 0, 1024 * sizeof(char));

    //handle each +PIECE message, read the information of each piece and store in shared memory
    for (int i = 0; i < loopNum; i++) {
        if (-1 != receiveMessageFromServer(msgBuffer, socket)) {
            if (8 == defineMsgType(msgBuffer)) {
                piece *currentPiece = malloc(sizeof(piece));
                //default value of status is current piece already on board, means neither available nor been captured,
                //should print on board
                currentPiece->status = 'B';
                int varSize = (int) strlen(msgBuffer) - 12;
                char tempOwnerOfPiece[2], tempPieceNumber[2], tempPosition[varSize + 1];
                //get piece info and store in struct currentPiece
                strncpy(tempOwnerOfPiece, msgBuffer + 7, 1);
                strncpy(tempPieceNumber, msgBuffer + 9, 1);
                strncpy(tempPosition, msgBuffer + 11, varSize);
                tempOwnerOfPiece[1] = '\0';
                tempPieceNumber[1] = '\0';
                tempPosition[varSize] = '\0';
                currentPiece->ownerOfPiece = atoi(tempOwnerOfPiece);
                currentPiece->pieceNumber = atoi(tempPieceNumber);
                //A(available) or C(been captured)
                if (varSize == 1) {
                    currentPiece->status = tempPosition[0];
                } else {
                    //A0...A7,B0...B7,C0..C7
                    currentPiece->side = tempPosition[0];
                    currentPiece->position = atoi(&tempPosition[1]);
                }
                //copy current piece info to shared memory
                if (pieceList + i != memcpy(pieceList + i, currentPiece, sizeof(piece))) {
                    fprintf(stderr, "can not copy piece information to shared memory\n");
                    //free(currentPiece);
                    return EXIT_FAILURE;
                }
                free(currentPiece);
            } else {
                fprintf(stderr, "current message is not about pieces\n");
                return EXIT_FAILURE;
            }
        } else {
            fprintf(stderr, "can not read piece information\n");
            return EXIT_FAILURE;
        }
        memset(msgBuffer, 0, 1024 * sizeof(char));
    }
    //check if all piece info has received, when yes, following message should start with E(ENDPIECELIST)
    if (flag == 1) {
        if (-1 != receiveMessageFromServer(msgBuffer, socket)) {
            if (4 == defineMsgType(msgBuffer)) {
                if (0 != sendThinking(socket, msgBuffer)) {
                    return EXIT_FAILURE;
                }
            }
        }
        if (-1 != receiveMessageFromServer(msgBuffer, socket)) {
            if (5 == defineMsgType(msgBuffer)) {
                if (0 != handleOkThinking(semId, shm, socket, msgBuffer)) {
                    return EXIT_FAILURE;
                }
            }
        }
    } else {
        if (-1 != receiveMessageFromServer(msgBuffer, socket)) {
            if (4 == defineMsgType(msgBuffer)) {
                return EXIT_SUCCESS;
            }
        }
    }

    return EXIT_SUCCESS;
}



int handleWait(int socket,char *msgBuffer){
    memset(msgBuffer,0,1024*sizeof (char ));
    int answerSize= strlen("OKWAIT\n");
    char tempAnswer[8]="OKWAIT\n";
    if((send(socket,tempAnswer,answerSize,0))<0){
        fprintf(stderr,"send message OKWAIT to server failed\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int gamePhase(int socket, int semId, sharedMemoryInfo *shm) {
    char msgBuffer[1024];
    memset(msgBuffer, 0, 1024 * sizeof(char));
    int flag = 1;
    while (true) {
        if (-1 != receiveMessageFromServer(msgBuffer, socket)) {
            if (1 == defineMsgType(msgBuffer)) {
                storeMaxMoveTime(msgBuffer, shm, semId);
            } else if (2 == defineMsgType(msgBuffer)) {
                storeCapturePiece(msgBuffer, shm, semId);
            } else if (3 == defineMsgType(msgBuffer)) {
                if (1 == handlePieceList(msgBuffer, socket, semId, shm, flag)) {
                    return EXIT_FAILURE;
                }
            } else if(6 == defineMsgType(msgBuffer)){
                if(1==handleWait(socket,msgBuffer)){
                    //idle
                    return EXIT_FAILURE;
                }
            } else if(7 == defineMsgType(msgBuffer)){
                //game over
                break;
            } else{
                fprintf(stderr,"error:%s\n",msgBuffer);
            }
        } else {
            fprintf(stderr, "can not receive message from server\n");
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

