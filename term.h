#ifndef k_term
#define k_term

#define _BSD_SOURCE 1
#define _POSIX_C_SOURCE 200809

#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <termios.h>

typedef struct {
    size_t w;
    size_t h;
    size_t x;
    size_t y;
    void (*mouse_func)(int);
    struct termios orig_t;
} term;

#define KT_NORM 0
#define KT_CTRL 1
#define KT_MOVE 2

#define KTM_UP 0
#define KTM_DOWN 1
#define KTM_LEFT 2
#define KTM_RIGHT 3
#define KTM_HOME 4
#define KTM_END 5
#define KTM_PGUP 6
#define KTM_PGDOWN 7

#define KTC_BACKSPACE 0
#define KTC_DEL 1
#define KTC_TAB 2
#define KTC_ENTER 3
#define KTC_SAVE 4
#define KTC_COPY 5
#define KTC_CUT 6
#define KTC_PASTE 7
#define KTC_UNDO 8
#define KTC_REDO 9

typedef struct {
    unsigned char code;
    unsigned char type;
} key;

term* term_init();
void term_free(term* t);
void term_update(term* t);
bool term_get_key(term* t, key* k);
void term_save_cursor(term* t);
void term_restore_cursor(term* t);
void term_move_cursor(term* t, size_t x, size_t y);
void term_clear(term* t);

#endif
