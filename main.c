#include <curses.h>
#include <string.h>
#include <time.h>
#include <errno.h>

const int WIDTH = 100;
const int HEIGHT = 50;

int GAME[50][100] = {0};
int BUFFER[50][100] = {0};
int GEN = 0;
bool LOOP = false;

void kbhit(void);
void next_generation(void);
bool in_bounds(int row, int cell);
void print(void);
int msleep(long msec);

int main(void) {
    initscr();

    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    // GAME LOOP
    while (true) {
        kbhit();
        while (LOOP) {
            print();
            msleep(250);
            next_generation();
            GEN++;
            kbhit();
        }
    }
}

void kbhit(void) {
    const int ch = getch();

    if (ch != ERR) {
        if (ch == 112 || ch == 80) {
            LOOP = false;
        } else if (ch == 115 || ch == 83) {
            LOOP = true;
        }

        if (LOOP == false) {
            if (ch == KEY_DOWN) {
                int x, y;
                getyx(stdscr, y, x);
                y++;
                if (y >= 0 && y < HEIGHT) {
                    move(y, x);
                }
            } else if (ch == KEY_UP) {
                int x, y;
                getyx(stdscr, y, x);
                y--;
                if (y >= 0 && y < HEIGHT) {
                    move(y, x);
                }
            } else if (ch == KEY_RIGHT) {
                int x, y;
                getyx(stdscr, y, x);
                x++;
                if (x >= 0 && x < WIDTH) {
                    move(y, x);
                }
            } else if (ch == KEY_LEFT) {
                int x, y;
                getyx(stdscr, y, x);
                x--;
                if (x >= 0 && x < WIDTH) {
                    move(y, x);
                }
            } else if (ch == 32) {
                int x, y;
                getyx(stdscr, y, x);
                if (GAME[y][x] == 0) {
                    GAME[y][x] = 1;
                    print();
                } else {
                    GAME[y][x] = 0;
                    print();
                }
            }
        }
    }
}

void next_generation(void) {
    for (int row = 0; row < HEIGHT; row++) {
        for (int cell = 0; cell < WIDTH; cell++) {
            int live_cells = 0;

            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    if (!(i == 0 && j == 0) && in_bounds(row+i, cell+j) && BUFFER[row+i][cell+j] == 1) {
                        live_cells++;
                    }
                }
            }

            if (BUFFER[row][cell] == 1) {
                if (live_cells < 2 || live_cells > 3) {
                    GAME[row][cell] = 0;
                }
            } else {
                if (live_cells == 3) {
                    GAME[row][cell] = 1;
                }
            }
        }
    }
}

bool in_bounds(const int row, const int cell) {
    return ((row >= 0 && row <= HEIGHT) && (cell >= 0 && cell <= WIDTH));
}

void print(void) {
    int y, x;
    getyx(stdscr, y, x);

    for (int row = 0; row < HEIGHT; row++) {
        for (int cell = 0; cell < WIDTH; cell++) {
            if (GAME[row][cell] == 1) {
                mvprintw(row, cell, "#");
            } else {
                mvprintw(row, cell, ".");
            }
        }
    }
    printw("\nGEN: %d", GEN);

    move(y, x);
    refresh();

    memcpy(BUFFER, GAME, sizeof(BUFFER));
}

int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}