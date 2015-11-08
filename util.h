#ifndef _util
#define _util

#include <stdbool.h>

bool iscrlf(char b) {
    return b == '\r' || b == '\n';
}

#endif
