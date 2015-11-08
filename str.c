#include "str.h"

str* str_init(unsigned char* data, size_t len) {
    str* s = malloc(sizeof(str));
    if(s == NULL) {
        return NULL;
    }

    s->data = NULL;
    s->len = 0;
    s->cap = 0;
    if(data != NULL && !str_insert(s, 0, data, len)) {
        str_free(s);
        return NULL;
    }

    return s;
}

bool str_write(str* s, size_t pos, unsigned char* data, size_t len, bool overwrite) {
    size_t req_len;
    if(!overwrite) {
        req_len = s->len + len;
    } else {
        req_len = pos + len > s->len ? pos + len:s->len;
    }

    if(req_len > s->cap) {
        size_t cap_len = req_len + 32;
        unsigned char* tmp = realloc(s->data, cap_len);
        if(tmp == NULL) {
            return false;
        }
        s->data = tmp;
        s->cap = cap_len;
    }

    if(!overwrite) {
        memmove(s->data + pos + len, s->data + pos, s->len - pos);
    }
    memcpy(s->data + pos, data, len);
    s->len = req_len;
    return true;
}

bool str_overwrite(str* s, size_t pos, unsigned char* data, size_t len) {
    return str_write(s, pos, data, len, true);
}

bool str_insert(str* s, size_t pos, unsigned char* data, size_t len) {
    return str_write(s, pos, data, len, false);
}

bool str_cut(str* s, size_t pos, size_t len) {
    if(pos + len > s->len) {
        len = s->len - pos;
    }
    if(pos + len < s->len) {
        memmove(s->data + pos, s->data + pos + len, s->len - pos - len);
    }
    s->len -= len;
    return true;
}

bool str_join(str* s, str* t) {
    bool ret = str_append(s, t->data, t->len);
    if(ret) {
        str_free(t);
    }
    return ret;
}

str* str_split(str* s, size_t pos) {
    str* ret = str_init(s->data + pos, s->len - pos);
    if(ret == NULL) {
        return NULL;
    }
    str_cut(s, pos, s->len - pos);
    return ret;
}

bool str_append(str* s, unsigned char* data, size_t len) {
    return str_insert(s, s->len, data, len);
}

void str_free(str* s) {
    free(s->data);
    free(s);
}
