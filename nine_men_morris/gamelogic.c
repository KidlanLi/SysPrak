#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gamelogic.h"


int captureIndex[2];


int determineMuehle(char board[7][7]) {
    int muehleNum = 0;

    if (board[0][0] != '.' && board[0][0] == board[0][3] && board[0][3] == board[0][6]) { muehleNum++; }
    if (board[0][0] != '.' && board[0][0] == board[3][0] && board[3][0] == board[6][0]) { muehleNum++; }
    if (board[6][6] != '.' && board[6][0] == board[6][3] && board[6][3] == board[6][6]) { muehleNum++; }
    if (board[6][6] != '.' && board[0][6] == board[3][6] && board[3][6] == board[6][6]) { muehleNum++; }

    if (board[1][1] != '.' && board[1][1] == board[1][3] && board[1][3] == board[1][5]) { muehleNum++; }
    if (board[1][1] != '.' && board[1][1] == board[3][1] && board[3][1] == board[5][1]) { muehleNum++; }
    if (board[5][5] != '.' && board[5][1] == board[5][3] && board[5][3] == board[5][5]) { muehleNum++; }
    if (board[5][5] != '.' && board[1][5] == board[3][5] && board[3][5] == board[5][5]) { muehleNum++; }

    if (board[2][2] != '.' && board[2][2] == board[2][3] && board[2][3] == board[2][4]) { muehleNum++; }
    if (board[2][2] != '.' && board[2][2] == board[3][2] && board[3][2] == board[4][2]) { muehleNum++; }
    if (board[4][4] != '.' && board[4][2] == board[4][3] && board[4][3] == board[4][4]) { muehleNum++; }
    if (board[4][4] != '.' && board[2][4] == board[3][4] && board[3][4] == board[4][4]) { muehleNum++; }

    if (board[0][3] != '.' && board[0][3] == board[1][3] && board[1][3] == board[2][3]) { muehleNum++; }
    if (board[3][0] != '.' && board[3][0] == board[3][1] && board[3][1] == board[3][2]) { muehleNum++; }
    if (board[6][3] != '.' && board[6][3] == board[5][3] && board[5][3] == board[4][3]) { muehleNum++; }
    if (board[3][6] != '.' && board[3][6] == board[3][5] && board[3][5] == board[3][4]) { muehleNum++; }

    if (muehleNum > 2) {
        printf("Board input error!");
        return EXIT_FAILURE;
    } else {
        return muehleNum;
    }
}


comboInfo cornerCombo(char board[7][7], int corner[2], int unit) {

    comboInfo combo;

    combo.firRow = corner[0];
    combo.firCol = corner[1];
    combo.sign = -1;
    combo.unit = unit;

    if (board[corner[0]][corner[1]] == '.') return combo;
    int row, col;

    if ((col = corner[1] - unit) - unit >= 0) {
        if ((board[corner[0]][col] == board[corner[0]][corner[1]]) &&
            (board[corner[0]][col - unit] != board[corner[0]][corner[1]])) {
            row = corner[0];
            combo.secRow = row;
            combo.secCol = col;
            combo.gapRow = row;
            combo.gapCol = col - unit;
            combo.sign = 1;
            return combo;
        }
    }

    if ((col = corner[1] + unit) + unit <= 6) {
        if ((board[corner[0]][col] == board[corner[0]][corner[1]]) &&
            (board[corner[0]][col + unit] != board[corner[0]][corner[1]])) {
            row = corner[0];
            combo.secRow = row;
            combo.secCol = col;
            combo.gapRow = row;
            combo.gapCol = col + unit;
            combo.sign = 1;
            return combo;
        }
    }

    if ((row = corner[0] - unit) - unit >= 0) {
        if ((board[row][corner[1]] == board[corner[0]][corner[1]]) &&
            (board[row - unit][corner[1]] != board[corner[0]][corner[1]])) {
            col = corner[1];
            combo.secRow = row;
            combo.secCol = col;
            combo.gapRow = row - unit;
            combo.gapCol = col;
            combo.sign = 1;
            return combo;
        }
    }

    if ((row = corner[0] + unit) + unit <= 6) {
        if ((board[row][corner[1]] == board[corner[0]][corner[1]]) &&
            (board[row + unit][corner[1]] != board[corner[0]][corner[1]])) {
            col = corner[1];
            combo.secRow = row;
            combo.secCol = col;
            combo.gapRow = row + unit;
            combo.gapCol = col;
            combo.sign = 1;
            return combo;
        }
    }
    return combo;
}


comboInfo crossCombo(char board[7][7], int crossMid[2]) {
    comboInfo combo;

    combo.firRow = crossMid[0];
    combo.firCol = crossMid[1];
    combo.sign = -1;
    combo.unit = 1;

    if (board[crossMid[0]][crossMid[1]] == '.') return combo;
    int row, col;
    int unit = 1;
    // left
    if ((col = crossMid[1] - unit) >= 0) {
        if ((board[crossMid[0]][col] == board[crossMid[0]][crossMid[1]]) &&
            (board[crossMid[0]][col + 2 * unit] != board[crossMid[0]][crossMid[1]])) {
            row = crossMid[0];
            combo.secRow = row;
            combo.secCol = col;
            combo.gapRow = row;
            combo.gapCol = col + 2 * unit;
            combo.sign = 1;
            return combo;
        }
    }
    // right
    if ((col = crossMid[1] + unit) <= 6) {
        if ((board[crossMid[0]][col] == board[crossMid[0]][crossMid[1]]) &&
            (board[crossMid[0]][col - 2 * unit] != board[crossMid[0]][crossMid[1]])) {
            row = crossMid[0];
            combo.secRow = row;
            combo.secCol = col;
            combo.gapRow = row;
            combo.gapCol = col - 2 * unit;
            combo.sign = 1;
            return combo;
        }
    }
    // top
    if ((row = crossMid[0] - unit) >= 0) {
        if ((board[row][crossMid[1]] == board[crossMid[0]][crossMid[1]]) &&
            (board[row + 2 * unit][crossMid[1]] != board[crossMid[0]][crossMid[1]])) {
            col = crossMid[1];
            combo.secRow = row;
            combo.secCol = col;
            combo.gapRow = row + 2 * unit;
            combo.gapCol = col;
            combo.sign = 1;
            return combo;
        }
    }
    // bottom
    if ((row = crossMid[0] + unit) <= 6) {
        if ((board[row][crossMid[1]] == board[crossMid[0]][crossMid[1]]) &&
            (board[row - 2 * unit][crossMid[1]] != board[crossMid[0]][crossMid[1]])) {
            col = crossMid[1];
            combo.secRow = row;
            combo.secCol = col;
            combo.gapRow = row - 2 * unit;
            combo.gapCol = col;
            combo.sign = 1;
            return combo;
        }
    }
    return combo;
}


comboInfo determineCombo(char board[7][7]) {
    comboInfo defau;
    defau.sign = -1;

    // first look for corner
    // store the row and col of A0 A2 A4 A6 B0 B2
    int cornerRows[12] = {0, 0, 6, 6, 1, 1, 5, 5, 2, 2, 4, 4};
    int cornerCols[12] = {0, 6, 6, 0, 1, 5, 5, 1, 2, 4, 4, 2};

    comboInfo cornercombo;
    int unit = 0;
    int corner[2] = {-1, -1};

    for (int i = 0; i < 12; i++) {
        if (i >= 0 && i <= 3) { unit = 3; }
        if (i >= 4 && i <= 7) { unit = 2; }
        if (i >= 8 && i <= 11) { unit = 1; }
        corner[0] = cornerRows[i];
        corner[1] = cornerCols[i];
        cornercombo = cornerCombo(board, corner, unit);
        if (cornercombo.sign != -1) { return cornercombo; }
    }

    // look for cross
    // store the row and col of B1 B3 B5 B7
    int crossRows[4] = {1, 3, 5, 3};
    int crossCols[4] = {3, 5, 3, 1};

    comboInfo crosscombo;
    int crossMid[2] = {-1, -1};
    for (int i = 0; i < 4; i++) {
        crossMid[0] = crossRows[i];
        crossMid[1] = crossCols[i];
        crosscombo = crossCombo(board, crossMid);
        if (crosscombo.sign != -1) { return cornercombo; }
    }
    return defau;
}



stepInfo put(char yourBoard[7][7], char myBoard[7][7]) {
    stepInfo newStep;
    comboInfo combo;
    combo = determineCombo(myBoard);
    if (combo.sign != -1) {
        if (yourBoard[combo.gapRow][combo.gapCol] == '.') {
            newStep.row = combo.gapRow;
            newStep.col = combo.gapCol;
            return newStep;
        }
    }
    combo = determineCombo(yourBoard);
    if (combo.sign != -1) {
        if (myBoard[combo.gapRow][combo.gapCol] == '.') {
            newStep.row = combo.gapRow;
            newStep.col = combo.gapCol;
            return newStep;
        }
    }
    //put in a random place that are free
    int rows[3][8] = {{0, 0, 0, 3, 6, 6, 6, 3},
                      {1, 1, 1, 3, 5, 5, 5, 3},
                      {2, 2, 2, 3, 4, 4, 4, 3}};
    int cols[3][8] = {{0, 3, 6, 6, 6, 3, 0, 0},
                      {1, 3, 5, 5, 5, 3, 1, 1},
                      {2, 3, 4, 4, 4, 3, 2, 2}};
    int i = rand() % 3;
    int j = rand() % 8;

    while (myBoard[rows[i][j]][cols[i][j]] != '.' || yourBoard[rows[i][j]][cols[i][j]] != '.') {
        i = rand() % 3;
        j = rand() % 8;
    }
    newStep.row = rows[i][j];
    newStep.col = cols[i][j];
    return newStep;

}

stepInfo eat(char yourBoard[7][7]) {
    stepInfo food;
    comboInfo combo;
    combo = determineCombo(yourBoard);
    if (combo.sign != -1) {
        food.row = combo.firRow;
        food.col = combo.firCol;
        return food;

    }
    int row = rand() % 7;
    int col = rand() % 7;
    while (yourBoard[row][col] == '.') {
        row = rand() % 7;
        col = rand() % 7;
    }
    food.row = row;
    food.col = col;
    return food;
}


moveInfo move(char yourBoard[7][7], char myBoard[7][7]) {
    moveInfo target;
    comboInfo combo;
    int fromRow, fromCol, toRow, toCol;
    combo = determineCombo(myBoard);

    if (combo.sign != -1) {
        toRow = combo.gapRow;
        toCol = combo.gapCol;
        if ((combo.firRow == 1 && combo.firCol == 3) ||
            (combo.firRow == 3 && (combo.firCol == 1 || combo.firCol == 5)) ||
            (combo.firRow == 5 && combo.firCol == 3)) {
            if ((combo.gapRow >= 2 && combo.gapRow <= 4) &&
                (combo.gapCol >= 2 && combo.gapCol <= 4)) {
                combo.unit = 1;
            } else {
                combo.unit = 3;
            }
        }

        if (combo.firRow == combo.gapRow) {
            fromCol = combo.gapCol;
            if ((fromRow = combo.gapRow - combo.unit) >= 0 && myBoard[fromRow][fromCol] != '.') {
                target.fromRow = fromRow;
                target.fromCol = fromCol;
                target.toRow = toRow;
                target.toCol = toCol;
                return target;
            }

            if ((fromRow = combo.gapRow + combo.unit) <= 6 && myBoard[fromRow][fromCol] != '.') {
                target.fromRow = fromRow;
                target.fromCol = fromCol;
                target.toRow = toRow;
                target.toCol = toCol;
                return target;
            }
        }


        if (combo.firCol == combo.gapCol) {
            fromRow = combo.gapRow;
            if ((fromCol = combo.gapCol - combo.unit) >= 0 && myBoard[fromRow][fromCol] != '.') {
                target.fromRow = fromRow;
                target.fromCol = fromCol;
                target.toRow = toRow;
                target.toCol = toCol;
                return target;
            }
            if ((fromCol = combo.gapCol + combo.unit) >= 0 && myBoard[fromRow][fromCol] != '.') {
                target.fromRow = fromRow;
                target.fromCol = fromCol;
                target.toRow = toRow;
                target.toCol = toCol;
                return target;
            }
        }
    }


    int rows[3][8] = {{0, 0, 0, 3, 6, 6, 6, 3},
                      {1, 1, 1, 3, 5, 5, 5, 3},
                      {2, 2, 2, 3, 4, 4, 4, 3}};
    int cols[3][8] = {{0, 3, 6, 6, 6, 3, 0, 0},
                      {1, 3, 5, 5, 5, 3, 1, 1},
                      {2, 3, 4, 4, 4, 3, 2, 2}};

    for (int unit = 3; unit > 0; unit--) {
//        do {
//            fromRow = unit * ((rand() % 3) + (3 - unit));
//            fromCol = unit * ((rand() % 3) + (3 - unit));
//        } while (fromRow == 3 && fromCol == 3);
        for (int i = 0; i < 8; ++i) {
            // condition: myBoard[fromRow][fromCol] != '.' && yourBoard[toRow][toCol] == '.' && myBoard[toRow][toCol] == '.';
            fromRow = rows[3 - unit][i];
            fromCol = cols[3 - unit][i];

//        while (myBoard[fromRow][fromCol] == '.' || (yourBoard[toRow][toCol] != '.' || myBoard[toRow][toCol] != '.')) {
//            do {
//                fromRow = unit * ((rand() % 3) + (3 - unit));
//                fromCol = unit * ((rand() % 3) + (3 - unit));
//            } while (fromRow != 3 || fromCol != 3);


            toRow = fromRow;

            toCol = fromCol - unit;
            if (toCol >= 0 && (toCol != 3 || toRow != 3) && (fromRow != 3 || fromCol != 3) &&
                myBoard[fromRow][fromCol] != '.' && yourBoard[toRow][toCol] == '.' && myBoard[toRow][toCol] == '.' &&
                toRow <= 3 + unit && toRow >= 3 - unit && toCol <= 3 + unit && toCol >= 3 - unit) {
                target.fromRow = fromRow;
                target.fromCol = fromCol;
                target.toRow = toRow;
                target.toCol = toCol;
                return target;
            }

            toCol = fromCol + unit;
            if (toCol <= 6 && (toCol != 3 || toRow != 3) && (fromRow != 3 || fromCol != 3) &&
                myBoard[fromRow][fromCol] != '.' && yourBoard[toRow][toCol] == '.' && myBoard[toRow][toCol] == '.' &&
                toRow <= 3 + unit && toRow >= 3 - unit && toCol <= 3 + unit && toCol >= 3 - unit) {
                target.fromRow = fromRow;
                target.fromCol = fromCol;
                target.toRow = toRow;
                target.toCol = toCol;
                return target;
            }

            toCol = fromCol;
            toRow = fromRow - unit;
            if (toRow >= 0 && (toCol != 3 || toRow != 3) && (fromRow != 3 || fromCol != 3) &&
                myBoard[fromRow][fromCol] != '.' && yourBoard[toRow][toCol] == '.' && myBoard[toRow][toCol] == '.' &&
                toRow <= 3 + unit && toRow >= 3 - unit && toCol <= 3 + unit && toCol >= 3 - unit) {
                target.fromRow = fromRow;
                target.fromCol = fromCol;
                target.toRow = toRow;
                target.toCol = toCol;
                return target;
            }
            toRow = fromRow + unit;
            if (toRow >= 0 && (toCol != 3 || toRow != 3) && (fromRow != 3 || fromCol != 3) &&
                myBoard[fromRow][fromCol] != '.' && yourBoard[toRow][toCol] == '.' && myBoard[toRow][toCol] == '.' &&
                toRow <= 3 + unit && toRow >= 3 - unit && toCol <= 3 + unit && toCol >= 3 - unit) {
                target.fromRow = fromRow;
                target.fromCol = fromCol;
                target.toRow = toRow;
                target.toCol = toCol;
                return target;
            }
        }
    }
    target.fromRow = -1;
    target.fromCol = -1;
    target.toRow = -1;
    target.toCol = -1;
    return target;
}


moveInfo jump(char yourBoard[7][7], char myBoard[7][7]) {
    moveInfo target;
    comboInfo myCombo;
    int rows[3][8] = {{0, 0, 0, 3, 6, 6, 6, 3},
                      {1, 1, 1, 3, 5, 5, 5, 3},
                      {2, 2, 2, 3, 4, 4, 4, 3}};
    int cols[3][8] = {{0, 3, 6, 6, 6, 3, 0, 0},
                      {1, 3, 5, 5, 5, 3, 1, 1},
                      {2, 3, 4, 4, 4, 3, 2, 2}};

    int fromRow, fromCol, toRow, toCol;
    myCombo = determineCombo(myBoard);
    if (myCombo.sign != -1 && yourBoard[myCombo.gapRow][myCombo.gapCol] == '.' &&
        myBoard[myCombo.gapRow][myCombo.gapCol] == '.') {
        target.toRow = myCombo.gapRow;
        target.toCol = myCombo.gapCol;
        for (int unit = 3; unit > 0; unit--) {
            for (int i = 0; i < 8; ++i) {
                fromRow = rows[3 - unit][i];
                fromCol = cols[3 - unit][i];
                if ((myCombo.firRow != fromRow || myCombo.firCol != fromCol) &&
                    (myCombo.secRow != fromRow || myCombo.secCol != fromCol) &&
                    myBoard[fromRow][fromCol] != '.') {
                    target.fromRow = fromRow;
                    target.fromCol = fromCol;
                    printf("！！！from[%d,%d]to jump\n", target.fromRow, target.fromCol);
                }
            }
        }
        return target;
    }


    comboInfo yourCombo;
    yourCombo = determineCombo(yourBoard);
    if (yourCombo.sign != -1 && yourBoard[yourCombo.gapRow][yourCombo.gapCol] == '.' &&
        myBoard[yourCombo.gapRow][yourCombo.gapCol] == '.') {
        target.toRow = yourCombo.gapRow;
        target.toCol = yourCombo.gapCol;
        for (int unit = 3; unit > 0; unit--) {
            for (int i = 0; i < 8; ++i) {
                fromRow = rows[3 - unit][i];
                fromCol = cols[3 - unit][i];
                if (myBoard[fromRow][fromCol] != '.') {
                    target.fromRow = fromRow;
                    target.fromCol = fromCol;
                }
            }
        }
        return target;
    }


    for (int unit = 3; unit > 0; unit--) {
        for (int i = 0; i < 8; ++i) {
            fromRow = rows[3 - unit][i];
            fromCol = cols[3 - unit][i];
            if (myBoard[fromRow][fromCol] != '.') {
                target.fromRow = fromRow;
                target.fromCol = fromCol;
            }
        }
    }
    for (int unit = 3; unit > 0; unit--) {
        for (int i = 0; i < 8; ++i) {
            toRow = rows[3 - unit][i];
            toCol = cols[3 - unit][i];
            if (myBoard[toRow][toCol] == '.' && yourBoard[toRow][toCol] == '.') {
                target.toRow = toRow;
                target.toCol = toCol;
            }
        }
    }
    return target;
}


