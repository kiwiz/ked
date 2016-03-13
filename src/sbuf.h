#ifndef k_sbuf
#define k_sbuf

#define _POSIX_C_SOURCE 200809

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#include "sbline.h"
#include "util.h"

typedef struct {
    int top;
    unsigned int x, y;
    char* fn;
    int fd;
    sbline* start;
    sbline* curr;
    sbline* end;
    size_t len;
} sbuf;

bool sbuf_save(sbuf* sb);
bool sbuf_append_str(sbuf* sb, str* s);
bool sbuf_append(sbuf* sb, unsigned char* s, size_t len);
void sbuf_free(sbuf* sb);
void sbuf_curr_up(sbuf* sb);
void sbuf_curr_down(sbuf* sb);
void sbuf_curr_left(sbuf* sb);
void sbuf_curr_right(sbuf* sb);
void sbuf_curr_home(sbuf* sb);
void sbuf_curr_end(sbuf* sb);
bool sbuf_curr_backspace(sbuf* sb, size_t len);
bool sbuf_curr_delete(sbuf* sb, size_t len);
bool sbuf_curr_split(sbuf* sb);
bool sbuf_curr_insert(sbuf* sb, unsigned char* data, size_t len);
void sbuf_render_line(sbuf* sb);
void sbuf_render_to_end(sbuf* sb);
void sbuf_render(sbuf* sb);

#endif
