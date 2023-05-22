#include "sharedMemory.h"
#include "gamePhase.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gamelogic.h"
#include "thinkerTool.h"

void printBoard(char board[13][24]) {
    puts("=======================================================\n");
    puts("player 1 with symbol *\n");
    puts("player 2 with symbol #\n");
    puts("free space with symbol +\n");
    puts("current board:\n");
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 24; j++) {
            printf("%c", board[i][j]);
        }
    }
    puts("========================================================\n");
}

/**
 * This method is responsible for the mapping between output board and info board.
 *
 * @param infoBoard  7*7 board used to store piece info and send to game logic to calculate next step
 * @param outputBoard  13*24 board only used as output
 */
void mappingPlayerBoardToOutputBoard(int rowOfInfoBoard, int colOfInfoBoard, char infoBoard[7][7],
                                     char outputBoard[13][24]) {
    int rowOfOutputBoard, colOfOutputBoard;
    rowOfOutputBoard = 2 * rowOfInfoBoard;
    switch (colOfInfoBoard) {
        case 0:
            colOfOutputBoard = 0;
            break;
        case 1:
        case 2:
            colOfOutputBoard = 4 * colOfInfoBoard;
            break;
        case 3:
            colOfOutputBoard = 11;
            break;
        case 4:
        case 5:
        case 6:
            colOfOutputBoard = 4 * colOfInfoBoard - 2;
            break;
    }
    if ('*' == infoBoard[rowOfInfoBoard][colOfInfoBoard] || '#' == infoBoard[rowOfInfoBoard][colOfInfoBoard]) {
        outputBoard[rowOfOutputBoard][colOfOutputBoard] = infoBoard[rowOfInfoBoard][colOfInfoBoard];
    }
}

/**
 * This method is responsible for put current piece into board according to the side,position and owner of piece.
 *
 * @param currentBoard can be currentPlayerBoard or anotherPlayerBoard
 * @param ownerSymbol * for player one, # for player 2
 */
void setBoardGrid(char currentBoard[7][7], char ownerSymbol, piece *currentPiece) {
    if (currentPiece->status == 'B') {
        if (currentPiece->position == 0) {
            //A0,B0,C0
            switch (currentPiece->side) {
                case 'A':
                    currentBoard[0][0] = ownerSymbol;
                    break;
                case 'B':
                    currentBoard[1][1] = ownerSymbol;
                    break;
                case 'C':
                    currentBoard[2][2] = ownerSymbol;
                    break;
            }
        } else if (currentPiece->position == 1) {
            //A1,B1,C1
            switch (currentPiece->side) {
                case 'A':
                    currentBoard[0][3] = ownerSymbol;
                    break;
                case 'B':
                    currentBoard[1][3] = ownerSymbol;
                    break;
                case 'C':
                    currentBoard[2][3] = ownerSymbol;
                    break;
            }
        } else if (currentPiece->position == 2) {
            //A2,B2,C2
            switch (currentPiece->side) {
                case 'A':
                    currentBoard[0][6] = ownerSymbol;
                    break;
                case 'B':
                    currentBoard[1][5] = ownerSymbol;
                    break;
                case 'C':
                    currentBoard[2][4] = ownerSymbol;
                    break;
            }
        } else if (currentPiece->position == 3) {
            //A3,B3,C3
            switch (currentPiece->side) {
                case 'A':
                    currentBoard[3][6] = ownerSymbol;
                    break;
                case 'B':
                    currentBoard[3][5] = ownerSymbol;
                    break;
                case 'C':
                    currentBoard[3][4] = ownerSymbol;
                    break;
            }
        } else if (currentPiece->position == 4) {
            //A4,B4,C4
            switch (currentPiece->side) {
                case 'A':
                    currentBoard[6][6] = ownerSymbol;
                    break;
                case 'B':
                    currentBoard[5][5] = ownerSymbol;
                    break;
                case 'C':
                    currentBoard[4][4] = ownerSymbol;
                    break;
            }
        } else if (currentPiece->position == 5) {
            //A5,B5,C5
            switch (currentPiece->side) {
                case 'A':
                    currentBoard[6][3] = ownerSymbol;
                    break;
                case 'B':
                    currentBoard[5][3] = ownerSymbol;
                    break;
                case 'C':
                    currentBoard[4][3] = ownerSymbol;
                    break;
            }
        } else if (currentPiece->position == 6) {
            //A6,B6,C6
            switch (currentPiece->side) {
                case 'A':
                    currentBoard[6][0] = ownerSymbol;
                    break;
                case 'B':
                    currentBoard[5][1] = ownerSymbol;
                    break;
                case 'C':
                    currentBoard[4][2] = ownerSymbol;
            }
        } else {
            switch (currentPiece->side) {
                //A7,B7,C7
                case 'A':
                    currentBoard[3][0] = ownerSymbol;
                    break;
                case 'B':
                    currentBoard[3][1] = ownerSymbol;
                    break;
                case 'C':
                    currentBoard[3][2] = ownerSymbol;
                    break;
            }
        }
    }
}

void
cpyPieceInfoToBoard(char currentPlayerBoard[7][7], char anotherPlayerBoard[7][7], char outputBoard[13][24],int semId,
                    piece *pieceList,
                    sharedMemoryInfo *shm) {
    lockSemaphore(semId);
    //loop all pieces and store into board
    for (int i = 0; i < shm->amountOfPiece; i++) {
        //determine the owner of current piece
        char ownerSymbol;
        if (pieceList->ownerOfPiece == 0) {
            ownerSymbol = '*';
        } else {
            ownerSymbol = '#';
        }
        //determine put current piece in currentPlayerBoard or anotherPlayerBoard
        if (pieceList->ownerOfPiece == shm->anotherPlayerId) {
            setBoardGrid(anotherPlayerBoard,ownerSymbol,pieceList);
        } else {
            setBoardGrid(currentPlayerBoard,ownerSymbol,pieceList);
        }
        pieceList++;
    }

    signalSemaphore(semId);

    for (int rowOfInfoBoard = 0; rowOfInfoBoard < 7; rowOfInfoBoard++) {
        for (int colOfInfoBoard = 0; colOfInfoBoard < 7; colOfInfoBoard++) {
            mappingPlayerBoardToOutputBoard(rowOfInfoBoard, colOfInfoBoard, currentPlayerBoard, outputBoard);
            mappingPlayerBoardToOutputBoard(rowOfInfoBoard, colOfInfoBoard, anotherPlayerBoard, outputBoard);
        }
    }
}

/**
 * This method is responsible for init two 7*7 board,which are currentPlayerBoard and anotherPlayerBoard.
 */
void initBoard(char currentPlayerBoard[7][7], char anotherPlayerBoard[7][7]) {
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            anotherPlayerBoard[i][j] = '.';
            currentPlayerBoard[i][j] = '.';
        }
    }
}

/**
 *This method helps to check if all pieces of 2 player has already placed on board
 * @return 0 when current player has not placed all his pieces on the board, 1 when all pieces has placed on board
 */
int checkIfPieceAvailable(piece *pieceList, sharedMemoryInfo *shm, int semId) {
    lockSemaphore(semId);
    for (int i = 0; i < shm->amountOfPiece; i++) {
        if (shm->anotherPlayerId != pieceList->ownerOfPiece) {
            if ('A' == pieceList->status) {
                return 0;
            }
        }
        pieceList++;
    }
    signalSemaphore(semId);
    return 1;
}

/**
 * This method helps to determine how much pieces current player have.
 */
int amountOfPieceInPlayerBoard(char board[7][7]) {
    int count = 0;
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            if ('*' == board[i][j] || '#' == board[i][j]) {
                count++;
            }
        }
    }
    return count;
}

int determineSequence(int pipeFd, piece *pieceList, int semId, sharedMemoryInfo *shm, char anotherPlayerBoard[7][7],
                       char currentPlayerBoard[7][7]) {
    stepInfo stepSequence;
    moveInfo moveSequence;
    char *stepPosition = malloc(sizeof(char) * 1024);
    char *movePosition = malloc(sizeof(char) * 1024);
    lockSemaphore(semId);
    if (0 == shm->captureFlag) {
        signalSemaphore(semId);
        int availablePiece = checkIfPieceAvailable(pieceList, shm, semId);
        //case:don't need to capture piece and some piece is still not on board, need to put them on board
        if (0 == availablePiece) {
            stepSequence = put(anotherPlayerBoard, currentPlayerBoard);
            strcpy(stepPosition, turnPositionInfoToChar(stepSequence.row, stepSequence.col));
            if (dprintf(pipeFd, "%s", stepPosition) < 0) {
                fprintf(stderr, "failed to write put sequence to pipe\n");
                return EXIT_FAILURE;
            }
        } else if (1 == availablePiece &&
                   3 < amountOfPieceInPlayerBoard(currentPlayerBoard)) {
            //case: don't need to capture piece this turn,all pieces has placed on board, there are still more than
            // three of player's own stones on the board
            moveSequence = move(anotherPlayerBoard, currentPlayerBoard);
            strcpy(movePosition, turnPositionInfoToChar(moveSequence.fromRow, moveSequence.fromCol));
            strcat(movePosition, ":");
            strcat(movePosition, turnPositionInfoToChar(moveSequence.toRow, moveSequence.toCol));
            if (dprintf(pipeFd, "%s", movePosition) < 0) {
                fprintf(stderr, "failed to write move sequence to pipe\n");
                return EXIT_FAILURE;
            }
        } else if (1 == availablePiece &&
                   3 == amountOfPieceInPlayerBoard(currentPlayerBoard)) {
            //case: don't need to capture piece this turn,all pieces has placed on board, there are only
            // three of player's own stones on the board
            moveSequence = jump(anotherPlayerBoard, currentPlayerBoard);
            strcpy(movePosition, turnPositionInfoToChar(moveSequence.fromRow, moveSequence.fromCol));
            strcat(movePosition, ":");
            strcat(movePosition, turnPositionInfoToChar(moveSequence.toRow, moveSequence.toCol));
            if (dprintf(pipeFd, "%s", movePosition) < 0) {
                fprintf(stderr, "failed to write move sequence to pipe\n");
                return EXIT_FAILURE;
            }
        }
    } else {
        //case:need to capture a piece
        signalSemaphore(semId);
        stepSequence = eat(anotherPlayerBoard);
        strcpy(stepPosition, turnPositionInfoToChar(stepSequence.row, stepSequence.col));
        if (dprintf(pipeFd, "%s", stepPosition) < 0) {
            fprintf(stderr, "failed to write capture sequence to pipe\n");
            return EXIT_FAILURE;
        }
    }
    signalSemaphore(semId);
    free(movePosition);
    free(stepPosition);
    return EXIT_SUCCESS;
}

int think(int semId, sharedMemoryInfo *shm) {
    char outputBoard[13][24]= {
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
    char currentPlayerBoard[7][7];
    char anotherPlayerBoard[7][7];

    initBoard(currentPlayerBoard, anotherPlayerBoard);

    piece *pieceList, *pieceListCpy;
    lockSemaphore(semId);
    pieceList = (piece *) attachShm(shm->boardShm);
    if (NULL == pieceList) {
        signalSemaphore(semId);
        return EXIT_FAILURE;
    }
    pieceListCpy = pieceList;
    int pipeFd = shm->pipeWrite;
    signalSemaphore(semId);
    cpyPieceInfoToBoard(currentPlayerBoard, anotherPlayerBoard, outputBoard,semId, pieceListCpy, shm);
    printBoard(outputBoard);
    determineSequence(pipeFd,pieceListCpy,semId,shm,anotherPlayerBoard,currentPlayerBoard);
    return EXIT_SUCCESS;
}
