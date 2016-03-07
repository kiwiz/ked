#include "sbline.h"

sbline* sbline_init(sbline* l, sbline* r, str* s) {
    sbline* line = malloc(sizeof(sbline));
    if(line == NULL) {
        return NULL;
    }

    line->l = l;
    line->r = r;
    line->s = s;

    return line;
}

void sbline_free(sbline* line, bool recurse) {
    str_free(line->s);
    if(recurse) {
        if(line->l != NULL) {
            sbline_free(line->l, true);
        }
        if(line->r != NULL) {
            sbline_free(line->r, true);
        }
    }
    free(line);
}

void sbline_prepend(sbline* line, sbline* other) {
    if(line->l) {
        other->l = line->l;
        line->l->r = other;
    }
    line->l = other;
    other->r = line;
}

void sbline_append(sbline* line, sbline* other) {
    if(line->r) {
        other->r = line->r;
        line->r->l = other;
    }
    line->r = other;
    other->l = line;
}

bool sbline_merge_next(sbline* line) {
    if(line->r == NULL) {
        return true;
    }
    if(!str_append(line->s, line->r->s->data, line->r->s->len)) {
        return false;
    }
    sbline* n = line->r->r;
    sbline_free(line->r, false);
    line->r = n;
    if(n != NULL) {
        n->l = line;
    }
    return true;
}
