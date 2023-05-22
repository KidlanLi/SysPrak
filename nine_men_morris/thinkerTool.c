#include <stddef.h>

char *turnPositionInfoToChar(int row, int col) {
    char *position = NULL;
    if (row == 0) {
        switch (col) {
            case 0:
                position = "A0";
                break;
            case 3:
                position = "A1";
                break;
            case 6:
                position = "A2";
                break;
            default:
                break;
        }
    } else if (row == 1) {
        switch (col) {
            case 1:
                position = "B0";
                break;
            case 3:
                position = "B1";
                break;
            case 5:
                position = "B2";
                break;
            default:
                break;
        }
    } else if (row == 2) {
        switch (col) {
            case 2:
                position = "C0";
                break;
            case 3:
                position = "C1";
                break;
            case 4:
                position = "C2";
                break;
            default:
                break;
        }
    } else if (row == 3) {
        switch (col) {
            case 0:
                position = "A7";
                break;
            case 1:
                position = "B7";
                break;
            case 2:
                position = "C7";
                break;
            case 6:
                position = "A3";
                break;
            case 5:
                position = "B3";
                break;
            case 4:
                position = "C3";
                break;
            default:
                break;
        }
    } else if (row == 4) {
        switch (col) {
            case 4:
                position = "C4";
                break;
            case 3:
                position = "C5";
                break;
            case 2:
                position = "C6";
                break;
            default:
                break;
        }
    } else if (row == 5) {
        switch (col) {
            case 5:
                position = "B4";
                break;
            case 3:
                position = "B5";
                break;
            case 1:
                position = "B6";
                break;
            default:
                break;
        }
    } else if (row == 6) {
        switch (col) {
            case 6:
                position = "A4";
                break;
            case 3:
                position = "A5";
                break;
            case 0:
                position = "A6";
                break;
            default:
                break;
        }
    }
    return position;
}