#define _BSD_SOURCE 1
#define _POSIX_C_SOURCE 200809

#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "term.h"
#include "sbuf.h"

#define RET_ISATTY 1
#define RET_OPENFILE 2

void process(term* t, sbuf** sb_arr) {
    size_t curr_buf = 0;
    sbuf* sb = sb_arr[0];

    term_save_cursor(t);
    term_clear(t);
    sbuf_render(sb);
    term_move_cursor(t, sb->x, sb->y);

    key k;
    bool ok = true;
    while(ok && term_get_key(t, &k)) {
        switch(k.type) {
        case KT_NORM:
            sbuf_curr_insert(sb, &(k.code), 1);
            sbuf_curr_right(sb);
            sbuf_render_line(sb);
            term_move_cursor(t, sb->x, sb->y);
            break;
        case KT_CTRL:
            switch(k.code) {
            case KTC_BACKSPACE:
                sbuf_curr_backspace(sb, 1);
                sbuf_render_to_end(sb); // TEMP: Whether to render rest depends on where cursor is
                term_move_cursor(t, sb->x, sb->y);
                break;
            case KTC_DEL:
                sbuf_curr_delete(sb, 1);
                sbuf_render_to_end(sb); // TEMP: Whether to render rest depends on where cursor is
                term_move_cursor(t, sb->x, sb->y);
                break;
            case KTC_TAB:
                break;
            case KTC_ENTER:
                sbuf_curr_split(sb);
                sbuf_render_to_end(sb);
                sbuf_curr_down(sb);
                sbuf_curr_home(sb);
                term_move_cursor(t, sb->x, sb->y);
                break;
            case KTC_SAVE:
                sbuf_save(sb);
                ok = false;
                break;
            }
            break;
        case KT_MOVE:
            switch(k.code) {
            case KTM_UP:
                sbuf_curr_up(sb);
                term_move_cursor(t, sb->x, sb->y);
                break;
            case KTM_DOWN:
                sbuf_curr_down(sb);
                term_move_cursor(t, sb->x, sb->y);
                break;
            case KTM_LEFT:
                sbuf_curr_left(sb);
                term_move_cursor(t, sb->x, sb->y);
                break;
            case KTM_RIGHT:
                sbuf_curr_right(sb);
                term_move_cursor(t, sb->x, sb->y);
                break;
            case KTM_HOME:
                sbuf_curr_home(sb);
                term_move_cursor(t, sb->x, sb->y);
                break;
            case KTM_END:
                sbuf_curr_end(sb);
                term_move_cursor(t, sb->x, sb->y);
                break;
            case KTM_PGUP:
                sbuf_curr_up(sb);
                sbuf_curr_up(sb);
                sbuf_curr_up(sb);
                sbuf_curr_up(sb);
                sbuf_curr_up(sb);
                term_move_cursor(t, sb->x, sb->y);
                break;
            case KTM_PGDOWN:
                sbuf_curr_down(sb);
                sbuf_curr_down(sb);
                sbuf_curr_down(sb);
                sbuf_curr_down(sb);
                sbuf_curr_down(sb);
                term_move_cursor(t, sb->x, sb->y);
                break;
            }
            break;
        }
    }

    term_restore_cursor(t);
}

int main(int argc, char** argv) {
    if(argc < 2) {
        printf("Usage: %s <file>\n", argv[0]);
        exit(0);
    }

    if(isatty(STDOUT_FILENO) == 0) {
        printf("STDOUT is not a TTY\n");
        exit(RET_ISATTY);
    }

    term* t = term_init();

    sbuf* sb_arr[2];
    sb_arr[0] = sbuf_init(argv[1]);
    sb_arr[1] = NULL;

    process(t, sb_arr);

    term_free(t);
}
