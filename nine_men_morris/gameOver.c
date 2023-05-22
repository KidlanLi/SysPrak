#include "gamePhase.h"
#include "prologPhase.h"
#include "gameOver.h"
#include "thinker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printGameResult(int resultPlayerOne, int resultPlayerTwo) {
    if (1 == resultPlayerOne && 1 == resultPlayerTwo) {
        printf("\n╔════════════R═E═S═U═L═T══════════════╗\n");
        printf("║                                     ║\n");
        printf("║           No Winner this time!      ║\n");
        printf("║                                     ║\n");
        printf("╚═════════════════════════════════════╝\n");
    } else if (1 == resultPlayerOne) {
        printf("\n╔════════════R═E═S═U═L═T══════════════╗\n");
        printf("║                                     ║\n");
        printf("║           Winner is: Player 1       ║\n");
        printf("║                                     ║\n");
        printf("╚═════════════════════════════════════╝\n");
    } else if (1 == resultPlayerTwo) {
        printf("\n╔════════════R═E═S═U═L═T══════════════╗\n");
        printf("║                                     ║\n");
        printf("║           Winner is: Player 2       ║\n");
        printf("║                                     ║\n");
        printf("╚═════════════════════════════════════╝\n");
    }
}

int gameOver(int socket, int semId, sharedMemoryInfo *shm) {
    char msgBuffer[1024];
    memset(msgBuffer, 0, 1024 * sizeof(char));
    int resultPlayerOne = 0;
    int resultPlayerTwo = 0;
    int flag = 0;
    char currentPlayerBoard[7][7];
    char anotherPlayerBoard[7][7];
    char outputBoard[13][24] = {
            "+----------+----------+\n",
            "|          |          |\n",
            "|   +------+------+   |\n",
            "|   |      |      |   |\n",
            "|   |   +--+--+   |   |\n",
            "|   |   |     |   |   |\n",
            "+---+---+     +---+---+\n",
            "|   |   |     |   |   |\n",
            "|   |   +--+--+   |   |\n",
            "|   |      |      |   |\n",
            "|   +------+------+   |\n",
            "|          |          |\n",
            "+----------+----------+\n"
    };
    piece *pieceList, *pieceListCpy;
    while (1) {
        if (-1 != receiveMessageFromServer(msgBuffer, socket)) {
            if (3 == defineMsgType(msgBuffer)) {
                handlePieceList(msgBuffer, socket, semId, shm, flag);
                initBoard(currentPlayerBoard, anotherPlayerBoard);
                lockSemaphore(semId);
                pieceList = (piece *) attachShm(shm->boardShm);
                if (NULL == pieceList) {
                    signalSemaphore(semId);
                    return EXIT_FAILURE;
                }
                signalSemaphore(semId);
                pieceListCpy = pieceList;
                cpyPieceInfoToBoard(currentPlayerBoard, anotherPlayerBoard, outputBoard, semId, pieceListCpy, shm);
                printBoard(outputBoard);
            } else if (*(msgBuffer + 8) == '0') {
                // listen message +PLAYER0WON
                if (*(msgBuffer + 13) == 'Y') {
                    resultPlayerOne = 1;
                }
                memset(msgBuffer, 0, 1024 * sizeof(char));
            } else if (*(msgBuffer + 8) == '1') {
                if (*(msgBuffer + 13) == 'Y') {
                    resultPlayerTwo = 1;
                }
                memset(msgBuffer, 0, 1024 * sizeof(char));
                printGameResult(resultPlayerOne, resultPlayerTwo);
            } else if (*(msgBuffer + 2) == 'Q') {
                break;
            } else {
                fprintf(stderr, "error:%s\n", msgBuffer);
                return EXIT_FAILURE;
            }
        } else {
            fprintf(stderr, "can not receive message from server\n");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

