/*****************
* Ishay Aharoni
* 316609411
* 01
* ass4
******************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>
#include "ass4.h"

// Move logical representation
typedef struct {
    char srcPiece, destPiece, destPromotion;
    int iSrc, jSrc, iDest, jDest;
    int isWhite, isCapture, isPromotion, isCheck, isMate;
} Move;

// PGN characters
const char PAWN = 'P';
const char ROOK = 'R';
const char KNIGHT = 'N';
const char BISHOP = 'B';
const char QUEEN = 'Q';
const char KING = 'K';
const char CAPTURE = 'x';
const char PROMOTION = '=';
const char CHECK = '+';
const char MATE = '#';
const char FIRST_COL = 'a';


// FEN & Board characters
const char WHITE_PAWN = 'P';
const char WHITE_ROOK = 'R';
const char WHITE_KNIGHT = 'N';
const char WHITE_BISHOP = 'B';
const char WHITE_QUEEN = 'Q';
const char WHITE_KING = 'K';
const char BLACK_PAWN = 'p';
const char BLACK_ROOK = 'r';
const char BLACK_KNIGHT = 'n';
const char BLACK_BISHOP = 'b';
const char BLACK_QUEEN = 'q';
const char BLACK_KING = 'k';

// FEN separator for strtok()
const char SEP[] = "/";

// Board characters
const char EMPTY = ' ';

// declaration of functions
void findSrcR(Move* m, char board[][SIZE]);
void findSrcB(Move* m, char board[][SIZE]);
int isCheck(int iSrc, int jSrc, char board[][SIZE], int isWhiteTurn, Move* m);
int isWayOpen(Move* m, char board[][SIZE]);
int isWayOpenB(int iPossible, int jPossible, Move* m, char board[][SIZE]);
int isWayOpenR(int iPossible, int jPossible, Move* m, char board[][SIZE]);
int isWayOpenQ(int iPossible, int jPossible, Move* m, char board[][SIZE]);
int isPossibleMove(Move* m);
int isPossibleMoveN(int iPossible, int jPossible, Move* m);
Move analyzePgn(char board[][SIZE], char pgn[], int isWhiteTurn);

 /***********************************************************************************
 * Function name: printColumns
 * Input: char c
 * Output: int
 * Function Operation: the function convert char to digit
 ***********************************************************************************/
int toDigit(char c) {
    assert('0' <= c && c <= '9');
    return c - '0';
}

/***********************************************************************************
* Function name: printColumns
* Input: None
* Output: None
* Function Operation: the function the columns of the chess board
***********************************************************************************/
void printColumns() {
    char column = toupper(FIRST_COL);
    printf("* |");
    // loop to print the column of the board
    for (int i = 0; i < SIZE; i++) {
        if (i) {
            printf(" ");
        }
        printf("%c", column);
        column++;
    }
    printf("| *\n");
}

/***********************************************************************************
* Function name: changeBoard
* Input: None
* Output: None
* Function Operation: the function print the spacers rows
***********************************************************************************/
void printSpacers() {
    printf("* -");
    for (int i = 0; i < SIZE; i++) {
        printf("--");
    }
    printf(" *\n");
}

/***********************************************************************************
* Function name: printRow
* Input: char row[], int rowIdx
* Output: None
* Function Operation: the function print the rows of the board with the row index
***********************************************************************************/
void printRow(char row[], int rowIdx) {
    printf("%d ", rowIdx);
    int i = 0;
    // loop to print the board
    while (row[i]) {
        // print the EMPTY
        if (isdigit(row[i])) {
            int spaces = toDigit(row[i]);
            for (int j = 0; j < spaces; j++) {
                printf("|%c", EMPTY);
            }
            // print the char
        } else {
            printf("|%c", row[i]);
        }
        i++;
    }
    // end of the row
    printf("| %d\n", rowIdx);
}

/***********************************************************************************
* Function name: printBoardFromFEN
* Input: char fen[]
* Output: None
* Function Operation: the function print the board according the fen string
***********************************************************************************/
void printBoardFromFEN(char fen[]) {
    // print the up frame
    printColumns();
    printSpacers();
    int rowIdx = SIZE;
    // use strtok to split the string to rows and print each one.
    char* fenRow = strtok(fen, SEP);
    while (fenRow != NULL) {
        printRow(fenRow, rowIdx--);
        fenRow = strtok(NULL, SEP);
    }
    // print the down frame
    printSpacers();
    printColumns();
}

/***********************************************************************************
* Function name: createBoard
* Input: char board[][SIZE], char fen[]
* Output: None
* Function Operation: the function analyze the fen string and according that 
* create board SIZExSIZE  
***********************************************************************************/
void createBoard(char board[][SIZE], char fen[]) {
    
    // i is the rows index
    int i = 0;
    // j is the coulmns index
    int j = 0;
    // k is the fen array index
    int k = 0;
    int tmp, p;

    // loop to create the board from the fen array
   while (fen[k]) {

        // check if the char is digit and conert to digit
        if (isdigit(fen[k])) {
            tmp = toDigit(fen[k]);
            // loop to put the EMPTY chars in the board
            for (p = 0; p < tmp; p++) {
                board[i][j] = EMPTY;
                j++;
            }
        }
        // the separtor '/' mean to move to next row
        else if (fen[k] == '/') {
            i++;
            j = 0;
        }
        // put the piece from the fen to the board
        else {
            board[i][j] = fen[k];
            j++;
        }
        k++;
    }    
}

/***********************************************************************************
* Function name: printBoard
* Input: char board[][SIZE]
* Output: None
* Function Operation: the function print the board like we were asked
***********************************************************************************/
void printBoard(char board[][SIZE]) {
    // k is index of the fen[]
    // i is index of rows in board
    // j is index of colmun in board
    int k = 0;
    int i = 0;
    int j = 0;

    // isEmpty will help us to know if the previos square was empty
    int isEmpty = 0;
    char fen[SIZE*SIZE+SIZE];
        // loop to convert the board to the fen string
        for (i = 0; i < SIZE; i++) {
            for (j = 0; j < SIZE; j++) {
                // piece in the board, put it into fen and k++
                if (board[i][j] != EMPTY) {
                    fen[k] = board[i][j];
                    k++;
                    isEmpty = 0;
                }
                // the square is empty and the before was not empty or new row
                else if (isEmpty == 0) {
                    fen[k] = '1';
                    isEmpty++;
                    k++;
                }
                // the square is empty and also before square was empty
                else {
                    k--;
                    fen[k] = fen[k]+1;
                    k++;
                }                
            }
            // put the sep into the fen in the and of every row beside of the last
            if (i!= SIZE-1) {
                fen[k] = '/';
                isEmpty = 0;
                k++;
            }
           
        }
        // close the string with /0 ,and print the board from fen
        fen[k] = 0;       
    printBoardFromFEN (fen);
}

/***********************************************************************************
* Function name: changeBoard
* Input: int iSrc, int jSrc, Move* m, char board[][SIZE]
* Output: None
* Function Operation: the function change the board according the move
***********************************************************************************/
void changeBoard(int iSrc, int jSrc, Move* m, char board[][SIZE]) {
    int isPromotion = m->isPromotion;
    // if it's not a promotion, make the move and change the board
    if (isPromotion == -1) {
        board[m->iDest][m->jDest] = board[iSrc][jSrc];
        board[iSrc][jSrc] = EMPTY;
    }
    // else this is promotion, so make the move and the promotion
    else {
        board[m->iDest][m->jDest] = m->destPromotion;
        board[iSrc][jSrc] = EMPTY;
    }
}

/***********************************************************************************
* Function name: findSrcQ
* Input: Move* m, char board[][SIZE]
* Output: None
* Function Operation: the function search for iSrc and jSrc for queen
***********************************************************************************/
void findSrcQ(Move *m, char board[][SIZE]){
    // queen can move like rook and bishop together; first check the rook movement
    findSrcR(m, board);   

    if (m->iSrc == -1) {
        // then if it's not a rook movement, check the bishop movement
        findSrcB(m, board);
    }
}

/***********************************************************************************
* Function name: findSrcK
* Input: Move* m, char board[][SIZE]
* Output: None
* Function Operation: the function search for iSrc and jSrc for king
***********************************************************************************/
void findSrcK(Move* m, char board[][SIZE]) {
    int i,j;
    // find the king on the board
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++){
            if (board[i][j] == m->srcPiece) {
                m->iSrc = i;
                m->jSrc = j;
           }
        }
    }    
}

/***********************************************************************************
* Function name: findSrcN
* Input: Move* m, char board[][SIZE]
* Output: None
* Function Operation: the function search for iSrc and jSrc for knight
***********************************************************************************/
void findSrcN(Move* m, char board[][SIZE]) {
    int isWhiteTurn = m->isWhite;
    // find all the knights, the one who can move legally to dest, is the src square
     for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if ((board[i][j] == m->srcPiece) &&
                    (isPossibleMoveN(i, j, m) == 1) &&
                    (!isCheck(i, j, board, isWhiteTurn, m))) {
                    // if we didn't get iSrc and jSrc from the pgn
                    if (m->iSrc == m->jSrc) {
                        m->iSrc = i;
                        m->jSrc = j;
                    }
                    // if we get the iSrc from the pgn
                    else if (i == m->iSrc) {
                        m->jSrc = j;
                    }
                    // if we get the jSrc from the pgn
                    else if (j == m->jSrc) {
                        m->iSrc = i;
                    }
                }
            }
      }       
}

/***********************************************************************************
* Function name: findSrcWP
* Input: Move* m, char board[][SIZE]
* Output: None
* Function Operation: the function search for iSrc and jSrc for white pawn
***********************************************************************************/
void findSrcWP(Move* m, char board[][SIZE]) {
    int i = m->iDest;
    int j = m->jDest;
    int isWhite = m->isWhite;
    int fourthRow = SIZE - 4;
    // if it's capture and the pwan is in the next row
    if ((m->isCapture == 1)) {
        if ((board[i + 1][m->jSrc] == WHITE_PAWN)) {
            m->iSrc = i + 1;
        }
    }
    // if the dest is the fourthRow; check if the pawn is in the next row or two after, and can move legally to dest 
    else if (i == fourthRow) {
        if((board[i+1][j]==WHITE_PAWN) && !isCheck(i+1,j,board,isWhite,m)){
            m->iSrc = i + 1;
            m->jSrc = j;
        }
        else if ((board[i + 2][j] == WHITE_PAWN) && !isCheck(i+2,j,board, isWhite, m) && board[i + 1][j] == EMPTY) {
            m->iSrc = i + 2;
            m->jSrc = j;
        }
    }
    // not the fourthRow; so the pawn can be only the row after
    else {
        if((board[i+1][j]==WHITE_PAWN) && (!isCheck(i,j,board, isWhite, m))){
            m->iSrc = i + 1;
            m->jSrc = j;
        }
    }
}

/***********************************************************************************
* Function name: findSrcR
* Input: Move* m, char board[][SIZE]
* Output: None
* Function Operation: the function search for iSrc and jSrc for rook
***********************************************************************************/
void findSrcR(Move* m, char board[][SIZE]) {
    int iSearch = m->iDest;
    int jSearch = m->jDest;
    int isWhiteTurn = m->isWhite;
    int j;
    int i;
    // if we get iSrc but not jSrc
    if (m->iSrc != -1) {
        // check the row; if we find the rook that can move legally to dest so that will be jSrc
        int iCorrect = m->iSrc;
        for (j = 0; j < SIZE; j++) {
            if ((board[iCorrect][j] == m->srcPiece) &&
                (isWayOpenR(iCorrect, j, m, board) == 1) &&
                (!isCheck(iCorrect, j, board, isWhiteTurn, m))) {
                m->jSrc = j;
            }
        }
    }
    // if we get jSrc but not iSrc
    else if (m->jSrc != -1) {
        // check the column; if we find the rook that can move legally to dest so that will be iSrc
        int jCorrect = m->jSrc;
        for (i = 0; i < SIZE; i++) {
            if ((board[i][jCorrect] == m->srcPiece) &&
                (isWayOpenR(i, jCorrect, m, board) == 1) &&
                (!isCheck(i, jCorrect, board,isWhiteTurn, m))) {
                m->iSrc = i;
            }
        }
    }
    // if we need to find iSrc and jSrc
    else { 
        // check the row; if we find the rook that can move legally to dest so that will be Src
        for (j = 0; j < SIZE; j++) {
            if ((board[iSearch][j] == m->srcPiece) &&
                (isWayOpenR(iSearch, j, m, board) == 1) &&
                ((isCheck(iSearch, j, board, isWhiteTurn, m)) == 0)) {
                m->iSrc = iSearch;
                m->jSrc = j;
                return;
            }
        }
        // check the column; if we find the rook that can move legally to dest so that will be Src
        for (i = 0; i < SIZE; i++) {
            if ((board[i][jSearch] == m->srcPiece) &&
                    (isWayOpenR(i, jSearch, m,board) == 1) &&
                    (isCheck(i, jSearch, board, isWhiteTurn, m) == 0)) {
                    m->iSrc = i;
                    m->jSrc = jSearch;
            }
        }        
    }
}

/***********************************************************************************
* Function name: findSrcBP
* Input: Move* m, char board[][SIZE]
* Output: None
* Function Operation: the function search for iSrc and jSrc for black pawn
***********************************************************************************/
void findSrcBP(Move* m, char board[][SIZE]) {
    int i = m->iDest;
    int j = m->jDest;
    int fourthBlackRow = 3;
    int isWhite = m->isWhite;

    // if it's capture and the pwan is in the line before
    if ((m->isCapture == 1)) {
        if ((board[i - 1][m->jSrc] == BLACK_PAWN)) {
            m->iSrc = i - 1;
        }
    }
    // if the dest is the fourthBlackRow; check if the pawn is in the row before or two before
    else if (i == fourthBlackRow) {
        if ((board[i - 1][j] == BLACK_PAWN) && !isCheck(i-1, j, board, isWhite, m)) {
            m->iSrc = i - 1;
            m->jSrc = j;
        }
        else if ((board[i - 2][j] == BLACK_PAWN) && !isCheck(i-2, j, board, isWhite, m) && board[i - 1][j] == EMPTY) {
            m->iSrc = i - 2;
            m->jSrc = j;
        }
    }
    else {
        // the dest is the others rows; so the pawn only row before
        if ((board[i - 1][j] == BLACK_PAWN) && (!isCheck(i-1, j, board, isWhite, m))) {
            m->iSrc = i - 1;
            m->jSrc = j;
        }
    }
}
   
/***********************************************************************************
* Function name: findSrcB
* Input: Move* m, char board[][SIZE]
* Output: None
* Function Operation: the function search for iSrc and jSrc for bishop
***********************************************************************************/
void findSrcB(Move* m, char board[][SIZE]) {    
    int isWhiteTurn = m->isWhite;
    int iSource = m->iSrc;
    int iDestenition = m->iDest;
    int jDestenition = m->jDest;
    
    // if we get iSrc but not jsrc
    if ((m->iSrc != -1)&&(m->jSrc == -1)) {
        // steps is the absolute value of the difrrenece between idestenition and iSource
        // the bishop could be only in two square; jPlus or jMinu
        int steps = abs(iDestenition - iSource);
        int jPlus = jDestenition + steps;
        int jMinu = jDestenition - steps;
        // search the bishop in jPlus; bishop that can move to dst without make check or blocked is jSrc
        if (((jDestenition + steps) >= 0) &&  ((jDestenition + steps) < SIZE) &&
            (board[iSource][jPlus] == (m->srcPiece)) &&
                isWayOpenB(iSource,jPlus,m,board) && !isCheck(iSource, jPlus, board, isWhiteTurn,m)) {
            m->jSrc = (m->jDest + steps);
            return;
        }
        // search the bishop in jMinu; bishop that can move to dst without make check or blocked is jSrc
        if (((m->jDest - steps) >= 0) && ((m->jDest - steps) < SIZE) &&
            (board[iSource][jMinu] == (m->srcPiece)) &&
            isWayOpenB(iSource,jMinu,m,board) && !isCheck(iSource, jMinu, board, isWhiteTurn, m)) {
            m->jSrc = (m->jDest - steps);
            return;
        }
    }

    // if we get jSrc but not iSrc
    else  if ((m->iSrc == -1) && (m->jSrc != -1)) {
        int jSource = m->jSrc;
        int jDestenition = m->jDest;       
        // steps is the absolute value of the difrrenece between jdestenition and jSource
        // the bishop could be only in two square; iPlus or iMinu
        int steps = abs(jDestenition - jSource);
        int iPlus = iDestenition + steps;
        int iMinu = iDestenition - steps;
        // search the bishop in iPlus; bishop that can move to dst without make check or blocked is iSrc
        if (((iDestenition + steps) >= 0) && ((iDestenition + steps) < SIZE) &&
            (board[iPlus][jSource] == (m->srcPiece)) &&
            isWayOpenB(iPlus,jSource,m,board) && !isCheck(iPlus, jSource, board, isWhiteTurn, m)) {
            m->iSrc = iPlus;
            return;
        }
        // search the bishop in iMlus; bishop that can move to dst without make check or blocked is iSrc
        if (((iMinu) >= 0) && ((iMinu) < SIZE) &&
            (board[iMinu][jSource] == (m->srcPiece)) &&
            isWayOpenB(iMinu,jSource,m,board) && !isCheck(iMinu, jSource, board, isWhiteTurn, m)) {
            m->iSrc = iMinu;
            return;
        }
    }
    // if we don't know jSrc and iSrc
    else  if ((m->iSrc == -1) && (m->jSrc == -1)) {
       int i = m->iDest;
       int j = m->jDest;
       // loop for search the bishop while i and j in size; if can move legally then it's the src
        while ((i>=0) && (i<SIZE) && (j>=0) && (j<SIZE)) {
            if ((board[i][j] == m->srcPiece) && (isWayOpenB(i,j,m,board)) && !isCheck(i, j, board, isWhiteTurn, m)) {
                m->iSrc = i;
                m->jSrc = j;
                return;
            }
            // continue check in down-right side
            i++; 
            j++;
        }

        i = m->iDest;
        j = m->jDest;
        if (m->iSrc == -1) {
            // loop for search the bishop while i and j in size; if can move legally then it's the src
            while ((i >= 0) && (i < SIZE) && (j >= 0) && (j < SIZE)) {
                if ((board[i][j] == m->srcPiece) && (isWayOpenB(i,j,m,board)) && !isCheck(i, j, board, isWhiteTurn, m)) {
                    m->iSrc = i;
                    m->jSrc = j;
                    return;
                }
                // continue check in down-left side
                i++;
                j--;
            }
        }
        i = m->iDest;
        j = m->jDest;
        if (m->iSrc == -1) {
            // loop for search the bishop while i and j in size; if can move legally then it's the src
            while ((i >= 0) && (i < SIZE) && (j >= 0) && (j < SIZE)) {
                if ((board[i][j] == m->srcPiece) && (isWayOpenB(i,j,m,board)) && !isCheck(i, j, board, isWhiteTurn, m)) {
                    m->iSrc = i;
                    m->jSrc = j;
                    return;
                }
                // continue check in up-right side
                i--;
                j++;
            }
        }
        i = m->iDest;
        j = m->jDest;
        if (m->iSrc == -1) {
            // loop for search the bishop while i and j in size; if can move legally then it's the src
            while ((i >= 0) && (i < SIZE) && (j >= 0) && (j < SIZE)) {
                if ((board[i][j] == m->srcPiece) && (isWayOpenB(i,j,m,board)) && !isCheck(i, j, board, isWhiteTurn, m)) {
                    m->iSrc = i;
                    m->jSrc = j;
                    return;
                }
                // continue check in up-left side
                i--;
                j--;
            }
        }
    }
}

/***********************************************************************************
* Function name: isWayOpenB
* Input: int iPossible, int jPossible, Move* m, char board[][SIZE]
* Output: int
* Function Operation: the function check if there is a path clear for a bishop
*  movement between two given 'square'
***********************************************************************************/
int isWayOpenB(int iPossible, int jPossible, Move* m, char board[][SIZE]){
    int iDest = m->iDest;
    int jDest = m->jDest;
    int iSteps, jSteps;
    // define the steps according the idest and jdest, and ipossible and jpossible    
    if (jDest > jPossible) {
        jSteps = -1;
    }
    else {
        jSteps = 1;
    }
    if (iDest > iPossible) {
        iSteps = -1;
    }
    else {
        iSteps = 1;
    }

    // start to check from idest and jdest, not include
    iDest = iDest + iSteps;
    jDest = jDest + jSteps;
    // loop till iPossible and jPossible, if it is not EMPTY, then the way is not clear and return 0
    while (iDest!=iPossible) {
        if (board[iDest][jDest] != EMPTY) {
            return 0;
        }
        iDest = iDest + iSteps;
        jDest = jDest + jSteps;
    }
    // else the way is clear
    return 1;
}

/***********************************************************************************
* Function name: isWayOpenR
* Input: int iPossible, int jPossible, Move* m, char board[][SIZE]
* Output: int
* Function Operation: the function check if there is a path clear for a rook
*  movement between two given 'square'
***********************************************************************************/
int isWayOpenR(int iPossible, int jPossible, Move* m, char board[][SIZE]) {
    int iDest = m->iDest;
    int jDest = m->jDest;
    int distance;

    // check the row movement
    if (iPossible == iDest) {
        // start to check from jDest to jPossible, so if jDest < jpossible then the step is 1
        if (jDest < jPossible) {
            distance = 1;
        }
        // else the step is -1
        else {            
            distance = -1;
        }    
        // start from the square after the dest
        jDest = jDest + distance;
        // loop till jPossible, if it is not EMPTY, then the way is not clear and return 0
        while (jDest != jPossible ) {
            if (board[iPossible][jDest] != EMPTY) {
                return 0;
            }
            jDest = jDest + distance;
        }
        // else the way is clear
        return 1;
    }
    
    // check the column movement
    if (jPossible == jDest) {
        // start to check from iDest to iPossible, so if iDest < ipossible then the step is 1
        if (iDest < iPossible) {
            distance = 1;
        }
        // else the step is -1
        else {
            distance = -1;
        }
        // start from the square after the dest
        iDest = iDest + distance;
        // loop till iPossible, if it is not EMPTY, then the way is not clear and return 0
        while (iDest != iPossible) {
            if (board[iDest][jPossible] != EMPTY) {
                return 0;
            }
            iDest = iDest + distance;
        }
        // else the way is clear
        return 1;
    }
}

/***********************************************************************************
* Function name: isWayOpenQ
* Input: int iPossible, int jPossible, Move* m, char board[][SIZE]
* Output: int
* Function Operation: the function check if there is a path clear for a queen
*  movement between two given 'square'
***********************************************************************************/
int isWayOpenQ(int iPossible, int jPossible, Move* m, char board[][SIZE]) {
    int iDest = m->iDest;
    int jDest = m->jDest;

    // queen can move like rook and bishop together; first check if the rook movement is clear 
    if ((iDest == iPossible) || (jDest == jPossible)) {
        return isWayOpenR(iPossible, jPossible, m, board);
    }
    // else check the if the bishop movement is clear   
        return isWayOpenB(iPossible, jPossible, m, board);    
}

/***********************************************************************************
* Function name: isPossibleMoveK
* Input: int iPossible, int jPossible, Move* m
* Output: int
* Function Operation: the function check if there is a possible movement for king
* between two given 'square'
***********************************************************************************/
int isPossibleMoveK(int iPossible, int jPossible, Move* m) {
    int iDest = m->iDest;
    int jDest = m->jDest;

    // king movement is one step to every side. so if the diffrence in absolute value > 1, then it is not possible
    if ((abs(iDest - iPossible) > 1) || (abs(jDest - jPossible) > 1)) {
        return 0;
    } 
    // else this is possible movement   
        return 1;   
}

/***********************************************************************************
* Function name: isPossibleMoveB
* Input: int iPossible, int jPossible, Move* m
* Output: int
* Function Operation: the function check if there is a possible movement for bishop
* between two given 'square'
***********************************************************************************/
int isPossibleMoveB(int iPossible, int jPossible, Move* m) {
    int iDest = m->iDest;
    int jDest = m->jDest;
    // bishop movement is through the diagonal, so must be true |i_1-i_2|==|j_1-j_2|
    if( abs((iDest - iPossible)) == abs((jDest - jPossible))) {
        return 1;
    }  
        return 0;  
}

/***********************************************************************************
* Function name: isPossibleMoveN
* Input: int iPossible, int jPossible, Move* m
* Output: int
* Function Operation: the function check if there is a possible movement for knight
* between two given 'square'
***********************************************************************************/
int isPossibleMoveN(int iPossible, int jPossible, Move* m) {
    int iDest = m->iDest;
    int jDest = m->jDest;
    // knight movement is two steps in j and one step in i, or one step in j and two steps in i
    if ((abs(iDest - iPossible) == 2) && (abs(jDest - jPossible) == 1)) {
        return 1;
    }
    if ((abs(iDest - iPossible) == 1) && (abs(jDest - jPossible) == 2)) {
        return 1;
    }
    // else is not possible movement
    return 0;
}

/***********************************************************************************
* Function name: isPossibleMoveR
* Input: int iPossible, int jPossible, Move* m
* Output: int
* Function Operation: the function check if there is a possible movement for rook
* between two given 'square'
***********************************************************************************/
int isPossibleMoveR(int iPossible, int jPossible, Move* m) {
    // rook movement is through coulmns or rows; if both are not even then is not possible
    if ((m->iDest != iPossible) && (m->jDest != jPossible)) {
        return 0;
    }    
        return 1;       
}

/***********************************************************************************
* Function name: isPossibleMoveWP
* Input: int iPossible, int jPossible, Move* m
* Output: int
* Function Operation: the function check if there is a possible movement for white 
* pawn between two given 'square'
***********************************************************************************/
int isPossibleMoveWP(int iPossible, int jPossible, Move* m) {
    int iDest = m->iDest;
    int jDest = m->jDest;
   const int fourthRow = SIZE - 4; 
   // if it's capture, then the pawn must be on the close diagonal
   if (m->isCapture == 1) {
       if (((iPossible - iDest) == 1) && (abs(jPossible - jDest) == 1)) {
           return 1;
       }
       // else it's not possible movement
       return 0;
   }

   // if the pawn is in a diffrent coulmn or the movement is in the wrong way return 0
   if ((iDest >= iPossible) || (jDest != jPossible)) {
       return 0;
   } 

   // if idest is 'fourthrow', then the pawn can move two steps, no more
    if (iDest==fourthRow) {
        if ((iPossible - iDest) > 2) {
            return 0;
        }
            // one or two steps is possible movement        
            return 1;        
    }
    
    // if idest is not 'fourthrow', then the pawn can only one step
    else {
        if ((iPossible - iDest) > 1) {
            return 0;
        }        
            return 1;        
    }
}

/***********************************************************************************
* Function name: isPossibleMoveBP
* Input: int iPossible, int jPossible, Move* m
* Output: int
* Function Operation: the function check if there is a possible movement for black
* pawn between two given 'square'
***********************************************************************************/
int isPossibleMoveBP(int iPossible, int jPossible, Move* m) {
    int iDest = m->iDest;
    int jDest = m->jDest;
    const int blackFourthRow = 3;

    // if it's capture, then the pawn must be on the close diagonal
    if (m->isCapture == 1) {
        if (((iDest - iPossible) == 1) && (abs(jPossible - jDest) == 1)) {
            return 1;
        }
        // else it's not possible movement
        return 0;
    }

    // if the pawn is in a diffrent coulmn or the movement is in the wrong way return 0
    if ((iDest <= iPossible) || (jDest != jPossible)) {
        return 0;
    }

    // if idest is 'black fourthrow', then the pawn can move two steps, no more
    if (iDest == blackFourthRow) {
        if ((iDest - iPossible) > 2) {
            return 0;
        }
            // one or two steps is possible movement        
            return 1;        
    }
    // if idest is not 'black fourthrow', then the pawn can only one step
    else {
        if ((iDest - iPossible) > 1) {
            return 0;
        }        
            return 1;        
    }
}

/***********************************************************************************
* Function name: isPossibleMoveQ
* Input: int iPossible, int jPossible, Move* m
* Output: int
* Function Operation: the function check if there is a possible movement for queen
* between two given 'square'
***********************************************************************************/
int isPossibleMoveQ(int iPossible, int jPossible, Move* m) {
    // queen can move like rook and bishop together; first check the rook movement
    if (isPossibleMoveR(iPossible, jPossible, m) ){
        return 1;
    }
    // then if it's not a rook movement, check the bishop movement
    if (isPossibleMoveB(iPossible, jPossible, m)) {
        return 1;
    }
        // else there is not possible movement to the queen    
        return 0;    
}

/***********************************************************************************
* Function name: isCorrectCheck
* Input: Move* m, char board[][SIZE]
* Output: int
* Function Operation: the function check if there is a discrepancy between
* the board and the move about 'check'
***********************************************************************************/
int isCorrectCheck(Move* m, char board[][SIZE]) {    
    // need to check if the move make a 'check', so use the function 'isCheck' on the opposite color
    int isWhiteTurn = m->isWhite;    
    int isThreat = 1 - isWhiteTurn;
    int iSrc = m->iSrc;
    int jSrc = m->jSrc;

    // checking if there is a discrepancy between the board and the move
    if (m->isCheck == 1) {
        if (isCheck(iSrc, jSrc, board, isThreat, m)) {
            return 1;
        }
        return 0;
    }
    if (m->isCheck != 1) {
        if(isCheck(iSrc, jSrc, board, isThreat, m)) {
            return 0;
        }
        return 1;    
    }
}

/***********************************************************************************
* Function name: isCheck
* Input: int iSrc, int jSrc, char board[][SIZE], int isWhiteTurn, Move* m
* Output: int
* Function Operation: check if after the move it will be a check
***********************************************************************************/
int isCheck(int iSrc, int jSrc, char board[][SIZE], int isWhiteTurn, Move* m) {
    // iking and jking will present the king square
    int iKing, jKing;

    // copy the board and move the object to dest wanted
    char copyBoard[SIZE][SIZE] = { 0 };
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            copyBoard[i][j] = board[i][j];
        }
    }
    changeBoard(iSrc, jSrc, m, copyBoard);

    // find_king
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if ((isWhiteTurn == 1) && (copyBoard[i][j] == WHITE_KING)) {
                iKing = i;
                jKing = j;
            }
            else if ((isWhiteTurn == 0) && (copyBoard[i][j] == BLACK_KING)) {
                iKing = i;
                jKing = j;
            }
        }
    }
    // for all item in board, find the opposite color
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if ((isWhiteTurn == 1) && (copyBoard[i][j] > 'a') && (copyBoard[i][j] < 's')) {
                // make a struct move name 'attack', and fill the fields
                Move attack;
                attack.iSrc = i;
                attack.jSrc = j;
                attack.iDest = iKing;
                attack.jDest = jKing;
                attack.srcPiece = copyBoard[i][j];
                attack.isCapture = 1;
                // if it's possible movement and the way is clear, then is check and return 1
                if ((isPossibleMove(&attack) == 1) && (isWayOpen(&attack, copyBoard)) == 1) {
                    return 1;
                }

            }
            else if ((isWhiteTurn == 0) && (copyBoard[i][j] > 'A') && (copyBoard[i][j] < 'S')) {
                // make a struct move name 'attack', and fill the fields
                Move attack;
                attack.iSrc = i;
                attack.jSrc = j;
                attack.iDest = iKing;
                attack.jDest = jKing;
                attack.srcPiece = copyBoard[i][j];
                attack.isCapture = 1;
                // if it's possible movement and the way is clear, then is check and return 1
                if ((isPossibleMove(&attack) == 1) && (isWayOpen(&attack, copyBoard) == 1)) {
                    return 1;
                }
            }
        }
    }
    // else it's not check and return 0
    return 0;
}

/***********************************************************************************
* Function name: isWayOpen
* Input: Move* m, char board[][SIZE]
* Output: int
* Function Operation: check if the way is clear, every piece by his
* appropriate function
***********************************************************************************/
int isWayOpen(Move* m, char board[][SIZE]) {
    // check if the way is clear, every piece by his appropriate function. if it is not clear, return 0
    int iSrc = m->iSrc;
    int jSrc = m->jSrc;
    // bishop
     if ((m->srcPiece == WHITE_BISHOP) || (m->srcPiece == BLACK_BISHOP)) {
        if (isWayOpenB(iSrc, jSrc, m, board) == 0) {            
            return 0;
        }
    }
     // rook
    else if ((m->srcPiece == WHITE_ROOK) || (m->srcPiece == BLACK_ROOK)) {
        if (isWayOpenR(iSrc, jSrc, m, board) == 0) {            
            return 0;
        }
    }    
     // queen
    else if ((m->srcPiece == WHITE_QUEEN) || (m->srcPiece == BLACK_QUEEN)) {
         if (isWayOpenQ(iSrc, jSrc, m, board) == 0) {            
             return 0;
         }
     }
     // else is legal and return 1
     return 1;
}

/***********************************************************************************
* Function name: isCorrectCapture
* Input: Move* m
* Output: int
* Function Operation: the function check if there is a discrepancy between
* the board and the move about capture
***********************************************************************************/
int isCorrectCapture(Move* m) {
    char destPiece = m->destPiece;
    char srcPiece = m->srcPiece;
    // if there is a discrepancy between the board and the move return 0
    if ((m->isCapture == 1) && (destPiece == EMPTY)) {
        return 0;
    }
    else if ((m->isCapture != 1) && (destPiece != EMPTY)) {
        return 0;
    }
    // else is legal and return 1
    return 1;
}

/***********************************************************************************
* Function name: isPossibleMove
* Input: Move* m
* Output: int
* Function Operation: the function check if the movement is possible, 
* every piece by his appropriate function. 
***********************************************************************************/
int isPossibleMove(Move* m) {    
    int iSrc = m->iSrc;
    int jSrc = m->jSrc;
    // check if the movement is possible, every piece by his appropriate function. if it is not possible, return 0
    if ((m->srcPiece == WHITE_QUEEN) || (m->srcPiece == BLACK_QUEEN)) {
        if (isPossibleMoveQ(iSrc, jSrc, m) == 0) {
            return 0;
        }
    }
    else if ((m->srcPiece == WHITE_KING) || (m->srcPiece == BLACK_KING)) {
        if (isPossibleMoveK(iSrc, jSrc, m) == 0) {
            return 0;
        }
    }
    else if ((m->srcPiece == WHITE_BISHOP) || (m->srcPiece == BLACK_BISHOP)) {
        if (isPossibleMoveB(iSrc, jSrc, m) == 0) {
            return 0;
        }
    }
    else if ((m->srcPiece == WHITE_ROOK) || (m->srcPiece == BLACK_ROOK)) {
        if (isPossibleMoveR(iSrc, jSrc, m) == 0) {
            return 0;
        }
    }
    else if ((m->srcPiece == WHITE_KNIGHT) || (m->srcPiece == BLACK_KNIGHT)) {
        if (isPossibleMoveN(iSrc, jSrc, m) == 0) {
            return 0;
        }
    }
    else if (m->srcPiece == WHITE_PAWN) {
        if (isPossibleMoveWP(iSrc, jSrc, m) == 0) {
            return 0;
        }
    }
    else if (m->srcPiece == BLACK_PAWN) {
        if (isPossibleMoveBP(iSrc, jSrc, m) == 0) {
            return 0;
        }
    }
    // else this is possible movement
    return 1;
}

/***********************************************************************************
* Function name: isCorrectPromotion
* Input: Move* m
* Output: int
* Function Operation: the function check if there is a discrepancy between 
* the board and the move about promotion
***********************************************************************************/
int isCorrectPromotion(Move* m) {  
    // promotion could be only: in the lastRow for the white, in the firstRow for the black
    const int lastRow = 0;
    const int firstRow = SIZE - 1;
    // if it's whiteTurn and there is a discrepancy between the board and the move
    if (m->isWhite == 1) {
        if (((m->iDest != lastRow) && (m->isPromotion == 1)) || ((m->iDest == lastRow) && (m->isPromotion != 1))) {
            return 0;
        }
    }
    // if it's blackTurn and there is a discrepancy between the board and the move
    else {
        if (((m->iDest != firstRow) && (m->isPromotion == 1)) || ((m->iDest == firstRow) && (m->isPromotion != 1))) {
            return 0;
        }
    }
    // else is legal so return 1
    return 1;
}

/***********************************************************************************
* Function name: isCaughtSquare
* Input: Move* m
* Output: int
* Function Operation: the function check if the dest piece is caught by 
* the same color
***********************************************************************************/
int isCaughtSquare(Move* m) {
    // if the destPiece is empty, then the square is not caught so return 0
    if (m->destPiece == EMPTY) {
        return 0;
    }
    // if the dest piece is caught by the same color return 1
    else if ((m->isWhite == 1) && (m->destPiece < 'S') && (m->destPiece > 'A')) {
        return 1;
    }
    else if ((m->isWhite == 0) && (m->destPiece < 's') && (m->destPiece > 'a')) {
        return 1;
    }
    // if the srcPiece is pawn, and it is capture but not from the diagonal return 1
    else if ((m->srcPiece == WHITE_PAWN) || (m->srcPiece == BLACK_PAWN)) {
        if ((m->isCapture == 1) && (m->jSrc == m->jDest)) {
            return 1;
        }
    }
    // else is legal and the square is not caught so return 0
    return 0;
}

/***********************************************************************************
* Function name: search
* Input: Move* m, char board[][SIZE]
* Output: None
* Function Operation: the function search for iSrc and jSrc on the board, 
* by using appropriate function to every piece
***********************************************************************************/
void search(Move* m, char board[][SIZE]) {
    // find iSrc and jSrc, every piece by his appropriate function
    if ((m->srcPiece == WHITE_QUEEN) || (m->srcPiece == BLACK_QUEEN)) {
        findSrcQ(m, board);
    }
    else if ((m->srcPiece == WHITE_KING) || (m->srcPiece == BLACK_KING)) {
        findSrcK(m, board);
    }
    else if ((m->srcPiece == WHITE_BISHOP) || (m->srcPiece == BLACK_BISHOP)) {
        findSrcB(m, board);
    }
    else if ((m->srcPiece == WHITE_ROOK) || (m->srcPiece == BLACK_ROOK)) {
        findSrcR(m, board);
    }
    else if ((m->srcPiece == WHITE_KNIGHT) || (m->srcPiece == BLACK_KNIGHT)) {
        findSrcN(m, board);
    }
    else if (m->srcPiece == WHITE_PAWN) {
        findSrcWP(m, board);
    }
    else if (m->srcPiece == BLACK_PAWN) {
        findSrcBP(m, board);
    }    
}

/***********************************************************************************
* Function name: analyzePgn
* Input: char board[][SIZE], char pgn[], int isWhiteTurn
* Output: Move - (struct)
* Function Operation: the function analyze the pgn string and fill all the Move 
* fields respectively
***********************************************************************************/
Move analyzePgn(char board[][SIZE], char pgn[], int isWhiteTurn) {
    Move m = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,};
    m.isWhite = isWhiteTurn;   
    int k = 0;
    int notPawn = 0;
    // analyze the pgn; first char is the src piece
    if ((pgn[0] == 'R') || (pgn[0] == 'K') || (pgn[0] == 'Q') || (pgn[0] == 'B') || (pgn[0] == 'N')) {
        m.srcPiece = pgn[0];
        notPawn = 1;
    }
    else {
        m.srcPiece = 'P';
    }
    // if its black then change to lowercup
    if (isWhiteTurn == 0) {
        m.srcPiece = m.srcPiece + 32;
    }
    // read the pgn from the end to beginnig in loop, and complete the struct
    int length = strlen(pgn);
    for (k = length - 1; k >= 0; k--) {
        // complete the check and mate field
        if (pgn[k] == MATE) {
            m.isMate = 1;
            m.isCheck = 1;
        }
        else if (pgn[k] == CHECK) {
            m.isCheck = 1;
        }
        // if there is a promotion
        else if ((pgn[k] > 'A') && (pgn[k] < 'S') && (notPawn == 0)) {
            m.isPromotion = 1;
            m.iDest = (SIZE - 1) * (1 - m.isWhite);
            m.destPromotion = pgn[k] + 32 * (1 - m.isWhite);
            k--;
        }
        // complete the iDest
        else if ((pgn[k] > '0') && (pgn[k] <= SIZE + '0') && (m.iDest == -1)) {
            m.iDest = SIZE - (pgn[k] - '0');
        }
        // complete the jDest
        else if ((pgn[k] >= 'a') && (pgn[k] <= 'i') && (m.jDest == -1)) {
            m.jDest = (pgn[k] - FIRST_COL);
        }
        // if there is capture
        else if (pgn[k] == CAPTURE) {
            m.isCapture = 1;
        }
        // if there is iSrc
        else if ((pgn[k] > '0') && (pgn[k] <= SIZE + '0')) {
            m.iSrc = SIZE - (pgn[k] - '0');
        }
        // if there is jSrc
        else if ((pgn[k] >= 'a') && (pgn[k] <= 'i')) {
            m.jSrc = pgn[k] - FIRST_COL;

        }
    }
    // destPiece is the piece/empty on the dest squre
    m.destPiece = board[m.iDest][m.jDest];
    return m;

}

/***********************************************************************************
* Function name: makeMove
* Input: char board[][SIZE], char pgn[], int isWhiteTurn
* Output: int
* Function Operation: the function check all the conditions to legal move, 
* if it's illegal return 0 , else change the board and return 1
***********************************************************************************/
int makeMove(char board[][SIZE], char pgn[], int isWhiteTurn) {
   // analyze the pgn and put it into move m
    Move m = analyzePgn(board, pgn, isWhiteTurn);
        // if the square is caught by the same color
        if (isCaughtSquare(&m) == 1) {
            return 0;
        }
        // find iSrc and jSrc if neccery. if we couldn't find then return 0
        if ((m.iSrc == -1) || (m.jSrc == -1)) {
            search(&m, board);
            if ((m.iSrc == -1) || (m.jSrc == -1)) {
                return 0;
            }                
        }
        // if we get iSrc and jSrc from pgn, need to check if the piece is realy there
        else {
            if (board[m.iSrc][m.jSrc] != m.srcPiece) {
                return 0;
            }
        }
        // after we find the source square, put it into int iSrc and jSrc
        int iSrc = m.iSrc;
        int jSrc = m.jSrc;      
        // check if the promotion is correct
        if ((m.srcPiece == WHITE_PAWN) || (m.srcPiece == BLACK_PAWN)) {
            if (isCorrectPromotion(&m) == 0) {
                return 0;
            }
        }
        // check if the piece can move this movement 
        if (isPossibleMove(&m) == 0) {
            return 0;
        }                
        // check if the capture is correct
        if (isCorrectCapture(&m) == 0) {
            return 0;
       }               
        // check if the way is open and if not return 0
        if (isWayOpen(&m, board) == 0) {
            return 0;
       }                 
        // check if after the move it will be a check, and if yes return 0
        if (isCheck(iSrc, jSrc, board, isWhiteTurn, &m) == 1) {            
            return 0;
        }
        // check if the check is correct
        if (isCorrectCheck(&m, board) == 0) {
            return 0;
        }        
        // if the function didn't return till now, then change the board and return 1
        changeBoard(iSrc, jSrc,&m, board);
        return 1;
}


