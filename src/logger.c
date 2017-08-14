#include "logger.h"

#include <stdio.h>
#include <string.h>

#define WARN_COUNT 20

void error(const char *msg, const char *arg) {
    char temp[strlen(msg) + (arg ? strlen(arg) : 0)];
    if (arg)
        sprintf(temp, msg, arg);
    else
        strcpy(temp, msg);
    fprintf(stderr, "Error: %s\n", temp);
}

void warn(const char *msg, const char *arg) {
    char temp[strlen(msg) + (arg ? strlen(arg) : 0)];
    if (arg)
        sprintf(temp, msg, arg);
    else
        strcpy(temp, msg);
    fprintf(stderr, "Warning: %s\n", temp);
}

unsigned long warn_once_arr[WARN_COUNT];
size_t warn_once_ptr = 0;

/* see: http://www.cse.yorku.ca/~oz/hash.html */
unsigned long hash(const char *str) {
    unsigned long result = 5381;
    int c;
    while ((c = *str++))
        result = ((result << 5) + result) + c;
    return result;
}

void warn_once(const char *msg, const char *arg) {
    char temp[strlen(msg) + (arg ? strlen(arg) : 0)];
    if (arg)
        sprintf(temp, msg, arg);
    else
        strcpy(temp, msg);
    const unsigned long hashval = hash(temp);
    for (size_t i = 0; i < warn_once_ptr; i++)
        if (warn_once_arr[i] == hashval)
            return;
    fprintf(stderr, "Warning: %s\n", temp);
    warn_once_arr[warn_once_ptr++] = hashval;
    warn_once_ptr %= WARN_COUNT;
}
