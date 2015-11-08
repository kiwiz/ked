#ifndef _sbuf
#define _sbuf

#include <unistd.h>
#include <stdio.h>

#include "str.h"
#include "util.h"

typedef struct sbline {
    struct sbline *l, *r;
    str* s;
    size_t len;
} sbline;

sbline* sbline_init(sbline* l, sbline* r, str* s) {
    sbline* line = malloc(sizeof(sbline));
    if(line == NULL) {
        return NULL;
    }

    line->l = l;
    line->r = r;
    line->s = s;
    line->len = 0;
    if(l != NULL) {
        line->len += l->len;
    }
    if(r != NULL) {
        line->len += r->len;
    }

    return line;
}

void sbline_free(sbline* line, bool recurse) {
    str_free(line->s);
    if(line->s == NULL && recurse) {
        if(line->l) {
            sbline_free(line->l, true);
        }
        if(line->r) {
            sbline_free(line->r, true);
        }
    }
    free(line);
}

typedef struct {
    unsigned int top;
    unsigned int x, y;
    char* fn;
    int fd;
    sbline* lines;
    size_t len;
} sbuf;

sbline* sbuf_get(sbuf* sb, size_t pos) {
    size_t spos = 0;
    sbline* line = sb->lines;
    while(line != NULL && line->s == NULL) {
        if(line->l != NULL && line->l->len < spos) {
            
        }
    }

    return line;
}

bool sbuf_insert(sbuf* sb, str* s, size_t lpos) {
    sbline* line = sbline_init(NULL, NULL, s);
    if(line == NULL) {
        return false;
    }

    line->l = NULL;
    line->r = NULL;
    line->s = s;
    line->len = 0;

    if(sb->lines == NULL) {
        sb->lines = line;
    } else {
        sbline* parent = sb->lines;
        while(parent->r != NULL) {
            parent = parent->r;
            line->l = parent;
        }
        parent->r = line;
    }

    ++sb->len;
    return true;
}

bool sbuf_append(sbuf* sb, str* s) {
    return sbuf_insert(sb, s, sb->len);
}

void sbuf_free(sbuf* sb) {
    free(sb->fn);
    close(sb->fd);
    if(sb->lines != NULL) {
        sbline_free(sb->lines, true);
    }
    free(sb);
}

sbuf* sbuf_init(char* fn) {
    sbuf* sb = malloc(sizeof(sbuf));
    if(sb == NULL) {
        return NULL;
    }

    int fd = open(fn, O_RDWR | O_APPEND | O_CREAT);
    if(fd == -1) {
        free(sb);
        return NULL;
    }
    fn = strdup(fn);
    if(fn == NULL) {
        free(sb);
        return NULL;
    }

    sb->top = 0;
    sb->x = 0;
    sb->y = 0;
    sb->fn = fn;
    sb->fd = fd;
    sb->len = 0;
    sb->lines = NULL;

    char buf[256];
    ssize_t slen;
    str* s = str_init(NULL, 0);
    if(s == NULL) {
        sbuf_free(sb);
        return NULL;
    }

    do {
        ssize_t slen = read(fd, buf, 256);
        if(slen <= 0) {
            break;
        }

        size_t pos = 0;
        size_t spos = 0;
        while(pos < slen) {
            if(iscrlf(buf[pos])) {
                if( str_append(s, buf + spos, pos - spos) &&
                    sbuf_append(sb, s)
                ) {
                    s = str_init(NULL, 0);
                    if(s == NULL) {
                        sbuf_free(sb);
                        return NULL;
                    }
                    spos = pos + 1;
                } else {
                    str_free(s);
                    sbuf_free(sb);
                    return NULL;
                }
            }
            ++pos;
        }
    } while(slen > 0);

    if(sb->len > 0) {
        if(!sbuf_append(sb, s)) {
            str_free(s);
            sbuf_free(sb);
            return NULL;
        }
    }

    return sb;
}

bool sbuf_remove(sbuf* sb, size_t pos) {

}

void sbuf_render(sbuf* sb, size_t pos) {
    sbline* line = sbuf_get(sb, pos);
    if(line == NULL) {
        return;
    }
    size_t i = 0;
    while(line != NULL) {
        dprintf(STDOUT_FILENO, "\x1b[%zu;0H\x1b[2K", i +1);
        write(STDOUT_FILENO, line->s->data, line->s->len);
        line = line->r;
        ++i;
    }
}

#endif
