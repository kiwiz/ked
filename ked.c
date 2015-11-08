#define _BSD_SOURCE 1

#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

#include "sbuf.h"

#define RET_ISATTY 1
#define RET_OPENFILE 2

// Setup/teardown terminal for editing.
void setup_term(struct termios* orig_t) {
    tcgetattr(STDOUT_FILENO, orig_t);
    struct termios raw_t;
    cfmakeraw(&raw_t);
    tcsetattr(STDOUT_FILENO, TCSAFLUSH, &raw_t);
}
void teardown_term(struct termios* orig_t) {
    tcsetattr(STDOUT_FILENO, TCSAFLUSH, orig_t);
}

void process(sbuf** sb_arr) {
    size_t curr_buf = 0;

    sbuf* sb = sb_arr[0];
    //printf("\x1b[2J");
    size_t w = 10;
    size_t h = 10;
    char c[16];
    while(1) {
        sbuf_render(sb, sb->top);
        if(read(STDIN_FILENO, c, 16) <= 0) {
            break;
        }
        switch(c[0]) {
            case 0x1b:
                sb->y += 1;
                break;
        }
        printf("\n\n\n\n\n%d %d %d\n", c[0], c[1], c[2]);
        dprintf("\x1b[%zu;%zuH", sb->x, sb->y);
        break;
    }
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

    struct termios orig_t;
    setup_term(&orig_t);

    sbuf* sb_arr[2];
    sb_arr[0] = sbuf_init(argv[1]);
    sb_arr[1] = NULL;

    process(sb_arr);

    teardown_term(&orig_t);
}
