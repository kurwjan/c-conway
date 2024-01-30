#ifndef CONWAY_H
#define CONWAY_H

#include <curses.h>
#include <time.h>
#include <string.h>

#define DEAD_CELL "."
#define LIVING_CELL "#"

#define FILLED_COLOUR 1

#define KEY_S 115
#define KEY_M 109
#define KEY_SPACE 32

#define ROWS 300
#define COLUMNS 300

typedef enum
{
    PLAYING,
    PAUSED
} Mode;

typedef struct {
    int y;
    int x;
} Vector;

typedef struct
{
    int game[ROWS][COLUMNS];
    int buffer[ROWS][COLUMNS];
} Board;

typedef struct Cell {
    bool dead;
    Vector position;
    struct Cell *next;
} Cell;

typedef struct
{
    bool exit;
    Mode mode;
    Board board;
    Vector viewport;
    Vector last_viewport;
    int viewport_size;
    Cell *cell;
} State;

Cell *get_cell_at_pos(Cell *cell, Vector position);
void add_cell(Cell *cell, bool dead, Vector position);
void move_viewport(State *state, Vector direction);
void move_cursor(const State *state, Vector direction);
State init();
void kbhit(State *state);
void next_generation(State *state);
bool in_bounds(const State *state, int row, int column);
void render(State *state);
void first_render(const State *state);
void render_menu(const State *state);
int msleep(long msec);

#endif