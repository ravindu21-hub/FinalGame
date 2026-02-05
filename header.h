#ifndef GAME_H
#define GAME_H

#include <stdio.h>

#define MAX_PLAYERS 3

#define MAX_NAME_LEN 20

typedef struct {
    char name[MAX_NAME_LEN]; 
    char symbol;             
    int isComputer;      
} Player;

char** createBoard(int size);
void freeBoard(char** board, int size);
void displayBoard(char** board, int size);
int isValidMove(char** board, int size, int row, int col);
void logMove(FILE* logFile, Player player, int row, int col);
int checkWin(char** board, int size, char symbol);
int checkDraw(char** board, int size);
void getPlayerMove(Player player, int size, int* row, int* col, char** board);
void playGame(int size, int numPlayers, Player players[]);

#endif