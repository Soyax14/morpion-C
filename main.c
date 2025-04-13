
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>

#define SIZE 3

char board[SIZE][SIZE];
int cursor_x = 0, cursor_y = 0;

enum Mode { PVP, PVC, CVC };
enum Mode game_mode;

void init_board() {
    for (int y = 0; y < SIZE; y++)
        for (int x = 0; x < SIZE; x++)
            board[y][x] = ' ';
}

void print_board() {
    system("clear");
    printf("=== MORPION ===\nMode : %s\n\n", game_mode == PVP ? "Joueur vs Joueur" :
                                              game_mode == PVC ? "Joueur vs Bot" :
                                                                 "Bot vs Bot");
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            if (cursor_y == y && cursor_x == x)
                printf("[%c]", board[y][x]);
            else
                printf(" %c ", board[y][x]);
            if (x < SIZE - 1) printf("|");
        }
        printf("\n");
        if (y < SIZE - 1) printf("---+---+---\n");
    }
}

char get_winner() {
    for (int i = 0; i < SIZE; i++) {
        if (board[i][0] != ' ' &&
            board[i][0] == board[i][1] &&
            board[i][1] == board[i][2])
            return board[i][0];
        if (board[0][i] != ' ' &&
            board[0][i] == board[1][i] &&
            board[1][i] == board[2][i])
            return board[0][i];
    }
    if (board[0][0] != ' ' &&
        board[0][0] == board[1][1] &&
        board[1][1] == board[2][2])
        return board[0][0];
    if (board[0][2] != ' ' &&
        board[0][2] == board[1][1] &&
        board[1][1] == board[2][0])
        return board[0][2];
    return ' ';
}

int is_full() {
    for (int y = 0; y < SIZE; y++)
        for (int x = 0; x < SIZE; x++)
            if (board[y][x] == ' ')
                return 0;
    return 1;
}

char get_key() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    if (ch == 27 && getchar() == 91) {
        ch = getchar();
        switch (ch) {
            case 'A': ch = 'U'; break;
            case 'B': ch = 'D'; break;
            case 'C': ch = 'R'; break;
            case 'D': ch = 'L'; break;
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

void move_cursor(char key) {
    switch (key) {
        case 'U': if (cursor_y > 0) cursor_y--; break;
        case 'D': if (cursor_y < SIZE - 1) cursor_y++; break;
        case 'L': if (cursor_x > 0) cursor_x--; break;
        case 'R': if (cursor_x < SIZE - 1) cursor_x++; break;
    }
}

void bot_move(char symbol) {
    int x, y;
    do {
        x = rand() % SIZE;
        y = rand() % SIZE;
    } while (board[y][x] != ' ');
    board[y][x] = symbol;
}

void choose_mode() {
    int selected = 0;
    const char *modes[] = {"Joueur vs Joueur", "Joueur vs Bot", "Bot vs Bot"};
    while (1) {
        system("clear");
        printf("=== Choisissez un mode ===\n");
        for (int i = 0; i < 3; i++) {
            printf("%s%s%s\n", i == selected ? "> " : "  ", modes[i], i == selected ? " <" : "");
        }
        char key = get_key();
        if (key == 'U') selected = (selected + 2) % 3;
        else if (key == 'D') selected = (selected + 1) % 3;
        else if (key == '\n') {
            game_mode = selected;
            return;
        }
    }
}

void play_game() {
    init_board();
    int turn = 0;
    while (1) {
        print_board();
        char symbol = (turn % 2 == 0) ? 'X' : 'O';
        if ((game_mode == CVC) || (game_mode == PVC && turn % 2 == 1)) {
            usleep(300000);
            bot_move(symbol);
        } else {
            while (1) {
                char key = get_key();
                if (key == 'U' || key == 'D' || key == 'L' || key == 'R')
                    move_cursor(key);
                else if (key == '\n' && board[cursor_y][cursor_x] == ' ') {
                    board[cursor_y][cursor_x] = symbol;
                    break;
                }
                print_board();
            }
        }
        if (get_winner() != ' ') {
            print_board();
            printf("\nLe joueur '%c' a gagné !\n", get_winner());
            break;
        }
        if (is_full()) {
            print_board();
            printf("\nMatch nul !\n");
            break;
        }
        turn++;
    }
}

int main() {
    srand(time(NULL));
    choose_mode();
    play_game();
    return 0;
}
