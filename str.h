#ifndef _str
#define _str

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    unsigned char* data;
    size_t len;
    size_t cap;
} str;

str* str_init(unsigned char* data, size_t len);
bool str_overwrite(str* s, size_t pos, unsigned char* data, size_t len);
bool str_insert(str* s, size_t pos, unsigned char* data, size_t len);
bool str_append(str* s, unsigned char* data, size_t len);
void str_cut(str* s, size_t pos, size_t len);
void str_join(str* s, str* t);
str* str_split(str* s, size_t pos);
void str_free(str* s);

#endif
