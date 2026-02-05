#include "header.h"  
#include <stdio.h>    
#include <stdlib.h>   
#include <string.h>   
#include <time.h>    

//  Create a dynamic board 
char** createBoard(int size) {
    // Allocate memory for an array of row pointers
    char** board = (char**)malloc(size * sizeof(char*));

    // Allocate each row and initialize cells with spaces
    for (int i = 0; i < size; i++) {
        board[i] = (char*)malloc(size * sizeof(char));
        for (int j = 0; j < size; j++) {
            board[i][j] = ' ';  // Empty cell
        }
    }
    return board;  // Return pointer to the 2D board
}

//  Free allocated memory 
void freeBoard(char** board, int size) {
    for (int i = 0; i < size; i++) free(board[i]);  // Free each row
    free(board);  // Free array of pointers itself
}

//  Display board nicely 
void displayBoard(char** board, int size) {
    printf("\n   ");
    for (int j = 0; j < size; j++) printf(" %d  ", j + 1); // Print column numbers
    printf("\n");

    for (int i = 0; i < size; i++) {
        // Print horizontal separator between rows
        printf("   ");
        for (int j = 0; j < size; j++) {
            printf("---");
            if (j < size - 1) printf("+");
        }
        printf("\n");

        // Print row number and symbols
        printf("%d |", i + 1);
        for (int j = 0; j < size; j++) {
            printf(" %c ", board[i][j]);
            if (j < size - 1) printf("|");
        }
        printf("\n");
    }

    // Print bottom border of the board
    printf("   ");
    for (int j = 0; j < size; j++) {
        printf("---");
        if (j < size - 1) printf("+");
    }
    printf("\n\n");
}

//  Check if a move is valid 
int isValidMove(char** board, int size, int row, int col) {
    // Valid if row & col are within bounds and cell is empty
    return row >= 0 && row < size && col >= 0 && col < size && board[row][col] == ' ';
}

//  Log a move to a file 
void logMove(FILE* logFile, Player player, int row, int col) {
    // Write move details in a text file
    fprintf(logFile, "%s (%c) moved to [%d,%d]\n", player.name, player.symbol, row + 1, col + 1);
}

//  Check if a player has won 
int checkWin(char** board, int size, char symbol) {
    int win;

    // Check all rows
    for (int i = 0; i < size; i++) {
        win = 1;
        for (int j = 0; j < size; j++) {
            if (board[i][j] != symbol) { win = 0; break; }
        }
        if (win) return 1;
    }

    // Check all columns
    for (int j = 0; j < size; j++) {
        win = 1;
        for (int i = 0; i < size; i++) {
            if (board[i][j] != symbol) { win = 0; break; }
        }
        if (win) return 1;
    }

    // Check main diagonal
    win = 1;
    for (int i = 0; i < size; i++) if (board[i][i] != symbol) { win = 0; break; }
    if (win) return 1;

    // Check anti-diagonal
    win = 1;
    for (int i = 0; i < size; i++) if (board[i][size - i - 1] != symbol) { win = 0; break; }
    return win;
}

//  Check for draw 
int checkDraw(char** board, int size) {
    // If any cell is empty, it's not a draw
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            if (board[i][j] == ' ') return 0;
    return 1;
}

//  Get move from player 
void getPlayerMove(Player player, int size, int* row, int* col, char** board) {
    if (player.isComputer) {  // Computer chooses random valid move
        do {
            *row = rand() % size;
            *col = rand() % size;
        } while (!isValidMove(board, size, *row, *col));
        printf("%s (%c) played at [%d,%d]\n", player.name, player.symbol, *row + 1, *col + 1);
    } else {  // Human player input
        while (1) {
            printf("%s (%c), enter your move (row and column) or 0 0 to exit: ", player.name, player.symbol);
            if (scanf("%d %d", row, col) != 2) {
                while (getchar() != '\n');  // Clear invalid input
                printf("Invalid input. Try again.\n");
                continue;
            }

            if (*row == 0 && *col == 0) { *row = -2; *col = -2; return; }  // Exit condition

            (*row)--; (*col)--;  // Convert 1-indexed to 0-indexed
            if (isValidMove(board, size, *row, *col)) break;
            else printf("Invalid move. Try again.\n");
        }
    }
}

//  Main game loop 
void playGame(int size, int numPlayers, Player players[]) {
    char** board = createBoard(size);  // Create the game board
    FILE* logFile = fopen("game_log.txt", "a");  // Open log file to append moves
    if (!logFile) { perror("Error opening log file"); exit(1); }

    int currentPlayer = 0, row, col;

    while (1) {
        displayBoard(board, size);  // Show board to players
        getPlayerMove(players[currentPlayer], size, &row, &col, board);  // Get move from current player

        if (row == -2 && col == -2) {  // Player chose to exit
            printf("Game exited by player.\n");
            break;
        }

        board[row][col] = players[currentPlayer].symbol;  // Update board
        logMove(logFile, players[currentPlayer], row, col);  // Record move

        if (checkWin(board, size, players[currentPlayer].symbol)) {  // Check for win
            displayBoard(board, size);
            printf("%s wins!\n", players[currentPlayer].name);
            break;
        }

        if (checkDraw(board, size)) {  // Check for draw
            displayBoard(board, size);
            printf("It's a draw!\n");
            break;
        }

        currentPlayer = (currentPlayer + 1) % numPlayers;  // Next player
    }

    fclose(logFile);  // Close the log file
    freeBoard(board, size);  // Free dynamically allocated memory
}

//  Main function 
int main() {
    srand(time(NULL));  // Seed random number generator
    int size, mode;
    Player players[MAX_PLAYERS];

    printf("Welcome to Modular Tic-Tac-Toe!\n");

    // Get board size (3-10)
    do {
        printf("Enter board size (3-10): ");
        scanf("%d", &size);
    } while (size < 3 || size > 10);

    // Game mode selection
    printf("Select Game Mode:\n1. Two-player\n2. User vs Computer\n3. Three-player\n0. Exit\nChoice: ");
    scanf("%d", &mode);

    if (mode == 0) { printf("Exiting the game. Goodbye!\n"); return 0; }

    if (mode == 1) {  // Two-player mode
        strcpy(players[0].name, "Player1"); players[0].symbol = 'X'; players[0].isComputer = 0;
        strcpy(players[1].name, "Player2"); players[1].symbol = 'O'; players[1].isComputer = 0;
        playGame(size, 2, players);
    } else if (mode == 2) {  // User vs Computer
        strcpy(players[0].name, "User"); players[0].symbol = 'X'; players[0].isComputer = 0;
        strcpy(players[1].name, "Computer"); players[1].symbol = 'O'; players[1].isComputer = 1;
        playGame(size, 2, players);
    } else if (mode == 3) {  // Three-player mode
        for (int i = 0; i < 3; i++) {
            printf("Enter name for Player %d: ", i + 1);
            scanf("%19s", players[i].name);
            players[i].symbol = (i == 0) ? 'X' : (i == 1) ? 'O' : 'Z';
            printf("Is %s a computer? (1 for Yes, 0 for No): ", players[i].name);
            scanf("%d", &players[i].isComputer);
        }
        playGame(size, 3, players);
    } else { printf("Invalid mode selected.\n"); }

    return 0;
}
