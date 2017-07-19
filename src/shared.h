#ifndef SHARED_HEADER_FILE
#define SHARED_HEADER_FILE

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    AIFF,
    WAVE
} FORMAT;

struct arguments {
    /* options */
    uint32_t bps, channels;
    FORMAT format;
    double length;
    bool reverse;
    uint32_t samplerate;
    bool verbose, nowarn;
    /* arguments */
    char* input;
    char* output;
};

char* format_tostring(FORMAT format);

bool swap_byteorder(FORMAT format);

#endif
