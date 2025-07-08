#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

struct grid{
    int rows;
    int cols;
    char **current;  // griglia corrente
    char **next;     // griglia successiva
};

/*** Creazione della griglia ***/
void createGrid(int rows, int cols, struct grid* myGrid){
    myGrid->rows = rows;
    myGrid->cols = cols;

    myGrid->current = malloc(sizeof(char*) * rows);
    myGrid->next = malloc(sizeof(char*) * rows);
    for(int i = 0; i < myGrid->rows; i++){
        myGrid->current[i] = malloc(sizeof(char) * cols);
        myGrid->next[i] = malloc(sizeof(char) * cols);
    }
}

/*** Inizializza tutta la griglia come morta ***/
void setAllDead(struct grid* myGrid){
    for(int i = 0; i < myGrid->rows; i++){
        for(int j = 0; j < myGrid->cols; j++){
            myGrid->current[i][j] = '*';
            myGrid->next[i][j] = '*';
        }
    }
}

/*** Corregge le coordinate per gestire il wrap-around ***/
int fixCoord(int coord, int max){
    int result = coord % max;
    if(result < 0){
        result += max;
    }
    return result;
}

/*** Conta le cellule vive adiacenti ***/
int countAlives(int x, int y, struct grid* myGrid){
    int counter = 0;

    /*** Controlla tutti gli 8 vicini ***/
    for(int dx = -1; dx <= 1; dx++){
        for(int dy = -1; dy <= 1; dy++){
            if(dx == 0 && dy == 0) continue;  // Salta la cella centrale

            int nx = fixCoord(x + dx, myGrid->rows);
            int ny = fixCoord(y + dy, myGrid->cols);

            if(myGrid->current[nx][ny] == 'X') counter++;
        }
    }

    return counter;
}

/*** Applica le regole del Game of Life ***/
void lifeChanges(struct grid* myGrid){
    for(int i = 0; i < myGrid->rows; i++){
        for(int j = 0; j < myGrid->cols; j++){
            int aliveNeighbors = countAlives(i, j, myGrid);
            bool isAlive = (myGrid->current[i][j] == 'X');

            // Regole del Game of Life
            if(isAlive){
                // Una cellula viva sopravvive se ha 2 o 3 vicini vivi
                if(aliveNeighbors == 2 || aliveNeighbors == 3){
                    myGrid->next[i][j] = 'X';
                } else {
                    myGrid->next[i][j] = '*';
                }
            } else {
                // Una cellula morta nasce se ha esattamente 3 vicini vivi
                if(aliveNeighbors == 3){
                    myGrid->next[i][j] = 'X';
                } else {
                    myGrid->next[i][j] = '*';
                }
            }
        }
    }
}

/*** Scambia le griglie current e next ***/
void swapGrids(struct grid* myGrid){
    char** temp = myGrid->current;
    myGrid->current = myGrid->next;
    myGrid->next = temp;
}

/*** Mostra la griglia ***/
void showGrid(struct grid* myGrid){
    printf("\x1b[H\x1b[2J\x1b[3JGame of Life\n\n");
    for(int i = 0; i < myGrid->rows; i++){
        for(int j = 0; j < myGrid->cols; j++){
            printf("%c ", myGrid->current[i][j]);
        }
        printf("\n");
    }
}

/*** Imposta una cellula viva o morta ***/
void setGrid(int x, int y, struct grid* myGrid, bool state){
    x = fixCoord(x, myGrid->rows);
    y = fixCoord(y, myGrid->cols);

    if(state){
        myGrid->current[x][y] = 'X';
    } else {
        myGrid->current[x][y] = '*';
    }
}

/*** Libera la memoria ***/
void freeGrid(struct grid* myGrid){
    for(int i = 0; i < myGrid->rows; i++){
        free(myGrid->current[i]);
        free(myGrid->next[i]);
    }
    free(myGrid->current);
    free(myGrid->next);
}

int main(int argc, char *argv[]){
    struct grid myGrid;

    if (argc >= 3){
        if(atoi(argv[1]) > 0 && atoi(argv[2]) > 0){
            myGrid.rows = atoi(argv[1]);
            myGrid.cols = atoi(argv[2]);
        } else {
            printf("I valori devono essere maggiori di 0!\n");
            exit(1);
        }
    } else {
        printf("Esegui il programma specificando il numero di righe e colonne da utilizzare!\n");
        exit(1);
    }

    createGrid(myGrid.rows, myGrid.cols, &myGrid);
    setAllDead(&myGrid);

    // Imposta un pattern iniziale (oscillatore)
    setGrid(6, 7, &myGrid, 1);
    setGrid(7, 7, &myGrid, 1);
    setGrid(8, 7, &myGrid, 1);

    // Loop principale
    while(1){
        showGrid(&myGrid);
        lifeChanges(&myGrid);
        swapGrids(&myGrid);
        sleep(1);
    }

    freeGrid(&myGrid);
    return 0;
}