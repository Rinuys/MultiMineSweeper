#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "stdbool.h"
#define N_MINES 1300
#define N_ROWS 66
#define N_COLS 120   // total cell = 66*120=7920
#define N_ALLCELLS 7920
#define COVER_FOR_CELL 10
#define MARK_FOR_CELL 10
#define EMPTY_CELL 0
#define MINE_CELL 9
#define DRAW_MINE 9
#define DRAW_COVER 10
#define DRAW_MARK 11
#define DRAW_WRONG_MARK 12
#define COVERED_MINE_CELL 19 //COVERED_MINE_CELL = MINE_CELL + COVER_FOR_CELL;
#define MARKED_MINE_CELL 29       //MARKED_MINE_CELL = COVERED_MINE_CELL + MARK_FOR_CELL;
bool inGame;
bool rep;
int mines_left = N_MINES;
int field[N_ALLCELLS];
int random_number(int min_num, int max_num)
{
    int result=0,low_num=0,hi_num=0;
    if(min_num<max_num)
    {
        low_num=min_num;
        hi_num=max_num+1;
        }else{
        low_num=max_num+1;
        hi_num=min_num;
    }
    //srand((unsigned)time(NULL));
    result = (rand()%(hi_num-low_num))+low_num;
    return result;
}
void new_game()
{
    int position = 0;
    int cell = 0;
    int buried = 0;
    int current_col;
    inGame = true;
    for(int i = 0; i < N_ALLCELLS; i++){
        field[i] = COVER_FOR_CELL;
    }
    while (buried < N_MINES) {
        position = random_number(1, N_ALLCELLS);
        printf("%dn", position);
        printf("%dn", buried);
        if ((position < N_ALLCELLS) &&
        (field[position] != COVERED_MINE_CELL)) {
            current_col = position % N_COLS;
            field[position] = COVERED_MINE_CELL;
            buried++;
            if (current_col > 0) {
                cell = position - 1 - N_COLS;
                if (cell >= 0)
                if (field[cell] != COVERED_MINE_CELL)
                field[cell] += 1;
                cell = position - 1;
                if (cell >= 0)
                if (field[cell] != COVERED_MINE_CELL)
                field[cell] += 1;
                cell = position + N_COLS - 1;
                if (cell < N_ALLCELLS)
                if (field[cell] != COVERED_MINE_CELL)
                field[cell] += 1;
            }
            cell = position - N_COLS;
            if (cell >= 0)
            if (field[cell] != COVERED_MINE_CELL)
            field[cell] += 1;
            cell = position + N_COLS;
            if (cell < N_ALLCELLS)
            if (field[cell] != COVERED_MINE_CELL)
            field[cell] += 1;
            if (current_col < (N_COLS - 1)) {
                cell = position - N_COLS + 1;
                if (cell >= 0)
                if (field[cell] != COVERED_MINE_CELL)
                field[cell] += 1;
                cell = position + N_COLS + 1;
                if (cell < N_ALLCELLS)
                if (field[cell] != COVERED_MINE_CELL)
                field[cell] += 1;
                cell = position + 1;
                if (cell < N_ALLCELLS)
                if (field[cell] != COVERED_MINE_CELL)
                field[cell] += 1;
            }
        }
    } // end while
}
void find_empty_cells(int j) {
    int current_col = j % N_COLS;
    int cell;
    if (current_col > 0) {
        cell = j - N_COLS - 1;
        if (cell >= 0)
        if (field[cell] > MINE_CELL) {
            field[cell] -= COVER_FOR_CELL;
            if (field[cell] == EMPTY_CELL)
            find_empty_cells(cell);
        }
        cell = j - 1;
        if (cell >= 0)
        if (field[cell] > MINE_CELL) {
            field[cell] -= COVER_FOR_CELL;
            if (field[cell] == EMPTY_CELL)
            find_empty_cells(cell);
        }
        cell = j + N_COLS - 1;
        if (cell < N_ALLCELLS)
        if (field[cell] > MINE_CELL) {
            field[cell] -= COVER_FOR_CELL;
            if (field[cell] == EMPTY_CELL)
            find_empty_cells(cell);
        }
    }
    cell = j - N_COLS;
    if (cell >= 0)
    if (field[cell] > MINE_CELL) {
        field[cell] -= COVER_FOR_CELL;
        if (field[cell] == EMPTY_CELL)
        find_empty_cells(cell);
    }
    cell = j + N_COLS;
    if (cell < N_ALLCELLS)
    if (field[cell] > MINE_CELL) {
        field[cell] -= COVER_FOR_CELL;
        if (field[cell] == EMPTY_CELL)
        find_empty_cells(cell);
    }
    if (current_col < (N_COLS - 1)) {
        cell = j - N_COLS + 1;
        if (cell >= 0)
        if (field[cell] > MINE_CELL) {
            field[cell] -= COVER_FOR_CELL;
            if (field[cell] == EMPTY_CELL)
            find_empty_cells(cell);
        }
        cell = j + N_COLS + 1;
        if (cell < N_ALLCELLS)
        if (field[cell] > MINE_CELL) {
            field[cell] -= COVER_FOR_CELL;
            if (field[cell] == EMPTY_CELL)
            find_empty_cells(cell);
        }
        cell = j + 1;
        if (cell < N_ALLCELLS)
        if (field[cell] > MINE_CELL) {
            field[cell] -= COVER_FOR_CELL;
            if (field[cell] == EMPTY_CELL)
            find_empty_cells(cell);
        }
    }
}
void re_calculate(int cCol, int cRow)
{
    bool rep = false;
    if (field[(cRow * N_COLS) + cCol] > MINE_CELL) {
        rep = true;
        if (field[(cRow * N_COLS) + cCol] <= COVERED_MINE_CELL) {
            if (mines_left > 0) {
                field[(cRow * N_COLS) + cCol] += MARK_FOR_CELL;
                mines_left--;
            } else
            printf("No marks leftn");;
            } else {
            field[(cRow * N_COLS) + cCol] -= MARK_FOR_CELL;
            mines_left++;
        }
    }
    else {
        if (field[(cRow * N_COLS) + cCol] > COVERED_MINE_CELL) {
            return;
        }
        if ((field[(cRow * N_COLS) + cCol] > MINE_CELL) &&
        (field[(cRow * N_COLS) + cCol] < MARKED_MINE_CELL)) {
            field[(cRow * N_COLS) + cCol] -= COVER_FOR_CELL;
            rep = true;
            if (field[(cRow * N_COLS) + cCol] == MINE_CELL)
            inGame = false;
            if (field[(cRow * N_COLS) + cCol] == EMPTY_CELL)
            find_empty_cells((cRow * N_COLS) + cCol);
        }
    }
}
int main(void)
{
    int i;
    int cCol = 20, cRow = 40;
    new_game();
    for(i = 0; i <= N_ALLCELLS; i++){
        printf("%d ", field[i]);
    }
    re_calculate(cCol, cRow);
    for(i = 0; i <= N_ALLCELLS; i++){
        printf("%d ", field[i]);
    }
    printf("inGame = %dn", inGame);
}