#include "term.h"

term* term_init() {
    term* t = malloc(sizeof(term));
    if(t == NULL) {
        return NULL;
    }

    t->x = 0;
    t->y = 0;
    term_update(t);

    tcgetattr(STDOUT_FILENO, &(t->orig_t));
    struct termios raw_t;
    cfmakeraw(&raw_t);
    tcsetattr(STDOUT_FILENO, TCSAFLUSH, &raw_t);

    return t;
}

void term_free(term* t) {
    tcsetattr(STDOUT_FILENO, TCSAFLUSH, &(t->orig_t));

    free(t);
}

void term_update(term* t) {
    //FIXME;
    t->w = 10;
    t->h = 10;
}

#define DBGLOG dprintf(STDERR_FILENO, "%d:", c); fflush(stdout);

bool term_get_key(term* t, key* k) {
    unsigned char c;

    if(read(STDOUT_FILENO, &c, 1) == 0) {
        return false;
    }
    DBGLOG

    // Printable character
    if(c >= 0x20 && c <= 0x7e) {
        k->type = KT_NORM;
        k->code = c;
        return true;
    }

    // Control sequence
    switch(c) {
    case 0x0d:
        k->type= KT_CTRL;
        k->code = KTC_ENTER;
        return true;

    case 0x13:
        k->type = KT_CTRL;
        k->code = KTC_SAVE;
        return true;

    case 0x7f:
        k->type = KT_CTRL;
        k->code = KTC_BACKSPACE;
        return true;

    case 0x1b:
        if(read(STDOUT_FILENO, &c, 1) == 0) {
            return false;
        }
        DBGLOG

        if(c != '[') {
            return false;
        }

        if(read(STDOUT_FILENO, &c, 1) == 0) {
            return false;
        }
        DBGLOG

        switch(c) {
            case 'A':
                k->type = KT_MOVE;
                k->code = KTM_UP; break;
            case 'B':
                k->type = KT_MOVE;
                k->code = KTM_DOWN; break;
            case 'C':
                k->type = KT_MOVE;
                k->code = KTM_RIGHT; break;
            case 'D':
                k->type = KT_MOVE;
                k->code = KTM_LEFT; break;
            case '1':
                k->type = KT_MOVE;
                k->code = KTM_HOME; goto trailing_tilde;
            case '4':
                k->type = KT_MOVE;
                k->code = KTM_END; goto trailing_tilde;
            case '5':
                k->type = KT_MOVE;
                k->code = KTM_PGUP; goto trailing_tilde;
            case '6':
                k->type = KT_MOVE;
                k->code = KTM_PGDOWN; goto trailing_tilde;
            case '3':
                k->type = KT_CTRL;
                k->code = KTC_DEL; goto trailing_tilde;

            default:
                return false;
        }

        return true;
    }

    /*
    if(c < 0x20) {
        k->type = KT_CTRL;
        k->code = c;
        return true;
    }*/

    return false;

trailing_tilde:
    if(read(STDOUT_FILENO, &c, 1) == 0) {
        return false;
    }
    switch(c) {
    case '~':
        return true;
    }
    return false;
}

void term_move_cursor(term* t, size_t x, size_t y) {
    dprintf(STDOUT_FILENO, "\x1b[%zu;%zuH", y + 1, x + 1);
}

//void term_print_line(size_t start, size_t end, bool trunc, data, len);

void term_save_cursor(term* t) {
    write(STDOUT_FILENO, "\x1b""7\x1b[?47h", 8);
}

void term_restore_cursor(term* t) {
    write(STDOUT_FILENO, "\x1b[?47l\x1b""8", 8);
}

void term_clear(term* t) {
    write(STDOUT_FILENO, "\x1b""c", 2);
}
