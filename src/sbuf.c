#include "sbuf.h"

sbuf* sbuf_init(char* fn) {
    sbuf* sb = malloc(sizeof(sbuf));
    if(sb == NULL) {
        return NULL;
    }

    int fd = open(fn, O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);
    if(fd == -1) {
        free(sb);
        return NULL;
    }
    fn = strdup(fn);
    if(fn == NULL) {
        close(fd);
        free(sb);
        return NULL;
    }

    sb->top = 0;
    sb->x = 0;
    sb->y = 0;
    sb->fn = fn;
    sb->fd = fd;
    sb->len = 0;
    sb->start = NULL;
    sb->curr = NULL;
    sb->end = NULL;

    char buf[256];
    str* s = str_init(NULL, 0);
    if(s == NULL) {
        sbuf_free(sb);
        return NULL;
    }

    // Read lines in.
    ssize_t slen = 0;
    while((slen = read(fd, buf, 256)) > 0) {
        size_t pos = 0; // Current pos.
        size_t spos = 0; // Start pos of current chunk.
        while(pos < slen) {
            if(iscrlf(buf[pos])) {
                if( str_append(s, buf + spos, pos - spos) &&
                    sbuf_append_str(sb, s)
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
    }

    // Add a single line if the input is empty.
    if(sb->len == 0) {
        if(!sbuf_append_str(sb, s)) {
            str_free(s);
            sbuf_free(sb);
            return NULL;
        }
    }

    return sb;
}

bool sbuf_save(sbuf* sb) {
    if(lseek(sb->fd, 0, SEEK_SET) == -1 || ftruncate(sb->fd, 0) == -1) {
        return false;
    }
    sbline* line = sb->start;
    while(line != NULL) {
        if( write(sb->fd, line->s->data, line->s->len) == -1 ||
            write(sb->fd, "\n", 1) == -1
        ) {
            return false;
        }
        line = line->r;
    }
    return true;
}

bool sbuf_append_str(sbuf* sb, str* s) {
    sbline* line = sbline_init(NULL, NULL, s);
    if(line == NULL) {
        return false;
    }

    if(sb->start == NULL) {
        sb->start = sb->curr = sb->end = line;
    } else {
        sbline_append(sb->end, line);
        sb->end = line;
    }
    ++sb->len;
    return true;
}

bool sbuf_append(sbuf* sb, unsigned char* data, size_t len) {
    str* s = str_init(data, len);
    if(s == NULL) {
        return false;
    }
    return sbuf_append_str(sb, s);
}

void sbuf_free(sbuf* sb) {
    free(sb->fn);
    close(sb->fd);
    sbline_free(sb->start, true);
    free(sb);
}

void sbuf_curr_up(sbuf* sb) {
    if(sb->y > 0) {
        --sb->y;
        sb->curr = sb->curr->l;
        if(sb->x > sb->curr->s->len) {
            sb->x = sb->curr->s->len;
        }
    }
}

void sbuf_curr_down(sbuf* sb) {
    if(sb->y < sb->len - 1) {
        ++sb->y;
        sb->curr = sb->curr->r;
        if(sb->x > sb->curr->s->len) {
            sb->x = sb->curr->s->len;
        }
    }
}

void sbuf_curr_left(sbuf* sb) {
    if(sb->x > 0) {
        --sb->x;
    } else if(sb->y > 0) {
        sb->curr = sb->curr->l;
        sb->x = sb->curr->s->len;
        --sb->y;
    }
}

void sbuf_curr_right(sbuf* sb) {
    if(sb->x < sb->curr->s->len) {
        ++sb->x;
    } else if(sb->y < sb->len - 1) {
        sb->curr = sb->curr->r;
        sb->x = 0;
        ++sb->y;
    }
}

void sbuf_curr_home(sbuf* sb) {
    sb->x = 0;
}

void sbuf_curr_end(sbuf* sb) {
    sb->x = sb->curr->s->len;
}

bool sbuf_curr_insert(sbuf* sb, unsigned char* data, size_t len) {
    return str_insert(sb->curr->s, sb->x, data, len);
}

bool sbuf_curr_backspace(sbuf* sb, size_t len) {
    while(len > 0) {
        if(sb->x == 0) {
            if(sb->y == 0) {
                break;
            }
            sbuf_curr_left(sb);
            sbuf_curr_delete(sb, 1);
            --len;
        } else {
            size_t delta = len > sb->x ? sb->x:len;
            str_cut(sb->curr->s, sb->x - delta, delta);
            len -= delta;
            sb->x -= delta;
        }
    }
    return true;
}

bool sbuf_curr_delete(sbuf* sb, size_t len) {
    while(len > 0) {
        if(sb->x == sb->curr->s->len) {
            if(sb->y == sb->len - 1) {
                break;
            }
            if(sb->curr->r == sb->end) {
                sb->end = sb->curr;
            }
            sbline_merge_next(sb->curr);
            --sb->len;
            --len;
        } else {
            size_t delta = sb->x + len > sb->curr->s->len ? sb->curr->s->len - sb->x:len;
            str_cut(sb->curr->s, sb->x, delta);
            len -= delta;
        }
    }
    return true;
}

bool sbuf_curr_split(sbuf* sb) {
    str* s = str_split(sb->curr->s, sb->x);
    if(s == NULL) {
        return NULL;
    }
    sbline* ret = sbline_init(sb->curr, sb->curr->r, s);
    if(ret == NULL) {
        str_free(s);
        return NULL;
    }
    sbline_append(sb->curr, ret);
    if(sb->curr == sb->end) {
        sb->end = ret;
    }
    ++sb->len;
}

void sbuf_render_line(sbuf* sb) {
    dprintf(STDOUT_FILENO, "\x1b[%zu;1H\x1b[2K", (size_t)(sb->y + 1));
    write(STDOUT_FILENO, sb->curr->s->data, sb->curr->s->len);
}

void sbuf_render_to_end(sbuf* sb) {
    size_t i = sb->y;
    sbline* line = sb->curr;
    while(line != NULL) {
        dprintf(STDOUT_FILENO, "\x1b[%zu;0H\x1b[2K", i + 1);
        write(STDOUT_FILENO, line->s->data, line->s->len);
        line = line->r;
        ++i;
    }
}

void sbuf_render(sbuf* sb) {
    sbline* line = sb->start;
    size_t i = 0;
    while(line != NULL) {
        dprintf(STDOUT_FILENO, "\x1b[%zu;0H\x1b[2K", i +1);
        write(STDOUT_FILENO, line->s->data, line->s->len);
        line = line->r;
        ++i;
    }
}
