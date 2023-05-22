

#ifndef NINE_MEN_MORRIS_GAMELOGIC_H
#define NINE_MEN_MORRIS_GAMELOGIC_H

typedef struct {
    int firRow;
    int firCol;
    int secRow;
    int secCol;

    int gapRow;
    int gapCol;

    int sign;
    int unit;
}comboInfo;


typedef struct {
    int row;
    int col;
}stepInfo;


typedef struct {
    int fromRow;
    int fromCol;
    int toRow;
    int toCol;
}moveInfo;
stepInfo put(char yourBoard[7][7], char myBoard[7][7]);
stepInfo eat(char yourBoard[7][7]);
moveInfo move(char yourBoard[7][7], char myBoard[7][7]);
moveInfo jump(char yourBoard[7][7], char myBoard[7][7]);
#endif //NINE_MEN_MORRIS_GAMELOGIC_H
