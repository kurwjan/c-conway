#include "conway.h"

#include <stdlib.h>

int main(void) {
    State state = init();

    first_render(&state);

    // GAME LOOP
    while (state.exit == false) {
        if (state.mode == PLAYING) {
            msleep(150);
            next_generation(&state);
        }

        render(&state);
        kbhit(&state);
    }
}

// Initilise everything like NCurses and return a State.
State init() {
    initscr();
    start_color();

    init_pair(1, COLOR_BLACK, COLOR_WHITE);

    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    Board board;
    memset(board.game, 0, sizeof(board.game));
    memset(board.buffer, 0, sizeof(board.buffer));

    const State state = {
        false,
        PAUSED,
        board,
        {0, 0},
{0, 0},
        50,
        NULL
    };

    return state;
}

void kbhit(State *state) {
    static bool viewport = false;

    const int key = getch();

    if (key != ERR) {
        if (key == KEY_S) {
            if (state->mode == PLAYING) {
                state->mode = PAUSED;
            } else {
                state->mode = PLAYING;
            }
        }

        if (state->mode == PAUSED) {
            if (key == KEY_M) {
                viewport = !viewport;
            }

            if (key == KEY_DOWN) {
                const Vector direction = {1, 0};
                if (viewport) {
                    move_viewport(state, direction);
                } else {
                    move_cursor(state, direction);
                }
            } else if (key == KEY_UP) {
                const Vector direction = {-1, 0};
                if (viewport) {
                    move_viewport(state, direction);
                } else {
                    move_cursor(state, direction);
                }
            } else if (key == KEY_RIGHT) {
                const Vector direction = {0, 1};
                if (viewport) {
                    move_viewport(state, direction);
                } else {
                    move_cursor(state, direction);
                }
            } else if (key == KEY_LEFT) {
                const Vector direction = {0, -1};
                if (viewport) {
                    move_viewport(state, direction);
                } else {
                    move_cursor(state, direction);
                }
            } else if (key == KEY_SPACE) {
                int x, y;
                getyx(stdscr, y, x);
                const Cell *cell = get_cell_at_pos(state->cell, {y + state->viewport.y, x + state->viewport.x});
                add_cell(state->cell, cell->dead, cell->position);
                //state->board.game[state->viewport.y + y][state->viewport.x + x] = !state->board.game[state->viewport.y + y][state->viewport.x + x];
            }
        }
    }
}

void delete_all_cells(Cell **cell) {
    Cell *current = *cell;

    while (current != NULL) {
        Cell *next = current->next;
        free(current);
        current = next;
    }

    *cell = NULL;
}

void add_cell(Cell *cell, const bool dead, const Vector position) {
    if (cell == NULL) {
        cell = (Cell *) malloc(sizeof(Cell));
        cell->dead = dead;
        cell->position = position;
        cell->next = NULL;
    } else {
        add_cell(cell->next, dead, position);
    }
}

Cell *get_cell_at_pos(Cell *cell, const Vector position) {
    if (cell != NULL && (cell->position.x == position.x) && (cell->position.y == position.y)) {
        return cell;
    }

    if (cell == NULL || cell->next == NULL) {
        if (cell == NULL || ((cell->position.y >= 0 && cell->position.y <= ROWS) && (cell->position.x >= 0 && cell->position.x <= COLUMNS))) {
            static Cell *dead = NULL;
            dead = (Cell *) malloc(sizeof(Cell));
            dead->next = NULL;
            dead->dead = true;
            dead->position = position;
            return dead;
        }
    }

    return get_cell_at_pos(cell->next, position);
}

void move_cursor(const State *state, const Vector direction) {
    Vector position;
    getyx(stdscr, position.y, position.x);

    position.x += direction.x;
    position.y += direction.y;

    if ((position.x >= 0 && position.y >= 0) && (position.x < state->viewport_size && position.y < state->viewport_size)) {
        move(position.y, position.x);
    }
}

void move_viewport(State *state, const Vector direction) {
    Vector position;
    position.x = state->viewport.x + direction.x;
    position.y = state->viewport.y + direction.y;

    if ((position.x >= 0 && position.y >= 0) && (position.x < state->viewport_size && position.y < state->viewport_size)) {
        state->viewport.x = position.x;
        state->viewport.y = position.y;
    }
}

void next_generation(State *state, Cell *cell) {
    if (cell == NULL) {
        return;
    }

    Vector position = cell->position;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            get_cell_at_pos()
        }
    }

    for (int row = 0; row < state->viewport_size; row++) {
        for (int column = 0; column < state->viewport_size; column++) {
            int live_cells = 0;

            // Just a shortcut
            const Vector calculated = {row + state->viewport.y, column + state->viewport.x};

            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    if (!(i == 0 && j == 0) && in_bounds(state, row+i, column+j) && state->board.buffer[calculated.y + i][calculated.x + j] == 1) {
                        live_cells++;
                    }
                }
            }

            if (state->board.buffer[calculated.y][calculated.x] == 1) {
                if (live_cells < 2 || live_cells > 3) {
                    state->board.game[calculated.y][calculated.x] = 0;
                }
            } else {
                if (live_cells == 3) {
                    state->board.game[calculated.y][calculated.x] = 1;
                }
            }
        }
    }
}

bool in_bounds(const State *state, const int row, const int column) {
    return ((row >= 0 && row <= state->viewport_size) && (column >= 0 && column <= state->viewport_size));
}

void first_render(const State *state) {
    for (int row = 0; row < state->viewport_size; row++) {
        for (int column = 0; column < state->viewport_size; column++) {
            mvprintw(state->viewport.y + row, state->viewport.x + column, ".");
        }
    }

    move(0, 0);
}

void render_menu(const State *state) {
    if (state->mode == PLAYING) {
        attron(COLOR_PAIR(FILLED_COLOUR));
        mvprintw(state->viewport_size + 1, 0, "(S) Stop");
        attroff(COLOR_PAIR(FILLED_COLOUR));
        printw(" ");
    } else {
        mvprintw(state->viewport_size + 1, 0, "(S) Start");
    }

    mvprintw(state->viewport_size + 3, 0, "Viewport offset: %d, %d ", state->viewport.y, state->viewport.x);
}

void render(State *state) {
    Vector mouse_origin;
    getyx(stdscr, mouse_origin.y, mouse_origin.x);

    for (int row = 0; row < state->viewport_size; row++) {
        for (int column = 0; column < state->viewport_size; column++) {
            const Vector calculated = {state->viewport.y + row, state->viewport.x + column};

            // Force render if viewport was moved
            if ((state->last_viewport.x != state->viewport.x) || (state->last_viewport.y != state->viewport.y)) {
                if (state->board.game[calculated.y][calculated.x] == 1) {
                    mvprintw(row, column, LIVING_CELL);
                } else {
                    mvprintw(row, column, DEAD_CELL);
                }
            } else {
                if (state->board.game[calculated.y][calculated.x] != state->board.buffer[calculated.y][calculated.x]) {
                    if (state->board.game[calculated.y][calculated.x] == 1) {
                        mvprintw(row, column, LIVING_CELL);
                    } else {
                        mvprintw(row, column, DEAD_CELL);
                    }
                }
            }
        }
    }

    render_menu(state);

    move(mouse_origin.y, mouse_origin.x);
    refresh();

    state->last_viewport = state->viewport;
    memcpy(state->board.buffer, state->board.game, sizeof(state->board.buffer));
}

int msleep(long msec)
{
    struct timespec ts;

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    return nanosleep(&ts, &ts);
}