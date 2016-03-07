#ifndef _sbline
#define _sbline

#include <unistd.h>
#include <stdio.h>

#include "str.h"
#include "util.h"

typedef struct sbline {
    struct sbline *l, *r;
    str* s;
} sbline;

sbline* sbline_init(sbline* l, sbline* r, str* s);
void sbline_free(sbline* line, bool recurse);
void sbline_prepend(sbline* line, sbline* other);
void sbline_append(sbline* line, sbline* other);
bool sbline_merge_next(sbline* line);

#endif
