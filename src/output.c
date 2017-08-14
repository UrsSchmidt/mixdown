#include "output.h"

#include "shared.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define SWAP(x,y) t=c[(x)];c[(x)]=c[(y)];c[(y)]=t;

void fwrite_int16(bool swap, const uint16_t s, FILE *file) {
    if (swap) {
        const uint16_t x = (( s       & 0xff) << 8)
                         | (((s >> 8) & 0xff));
        fwrite(&x, 2, 1, file);
    } else {
        fwrite(&s, 2, 1, file);
    }
}

void fwrite_int32(bool swap, const uint32_t i, FILE *file) {
    if (swap) {
        const uint32_t x = (( i        & 0xff) << 24)
                         | (((i >>  8) & 0xff) << 16)
                         | (((i >> 16) & 0xff) <<  8)
                         | (((i >> 24) & 0xff));
        fwrite(&x, 4, 1, file);
    } else {
        fwrite(&i, 4, 1, file);
    }
}

void fwrite_long_double(bool swap, const long double ld, FILE *file) {
    if (swap) {
        uint8_t *c = (uint8_t*)&ld;
        uint8_t t;
        SWAP(0, 9);
        SWAP(1, 8);
        SWAP(2, 7);
        SWAP(3, 6);
        SWAP(4, 5);
        fwrite(c, 10, 1, file);
    } else {
        fwrite(&ld, 10, 1, file);
    }
}

void fwrite_aiff(struct arguments *arguments, uint32_t data_size, uint8_t *data, FILE *file) {
    const bool swap = swap_byteorder(AIFF);
    const long double samplerate = arguments->samplerate;
    fwrite("FORM", 4, 1, file);
    fwrite_int32(swap, data_size + 46, file);
    fwrite("AIFFCOMM", 8, 1, file);
    fwrite_int32(swap, 18, file);
    fwrite_int16(swap, arguments->channels, file);
    fwrite_int32(swap, arguments->length * arguments->samplerate, file);
    fwrite_int16(swap, arguments->bps, file);
    fwrite_long_double(swap, samplerate, file);
    fwrite("SSND", 4, 1, file);
    fwrite_int32(swap, data_size + 8, file);
    fwrite_int32(swap, 0, file);
    fwrite_int32(swap, 0, file);
    fwrite(data, 1, data_size, file);
}

void fwrite_wave(struct arguments *arguments, uint32_t data_size, uint8_t *data, FILE *file) {
    const bool swap = swap_byteorder(WAVE);
    const uint16_t block_align = arguments->channels * (arguments->bps / 8);
    fwrite("RIFF", 4, 1, file);
    fwrite_int32(swap, data_size + 36, file);
    fwrite("WAVEfmt ", 8, 1, file);
    fwrite_int32(swap, 16, file);
    fwrite_int16(swap, 1, file);
    fwrite_int16(swap, arguments->channels, file);
    fwrite_int32(swap, arguments->samplerate, file);
    fwrite_int32(swap, arguments->samplerate * block_align, file);
    fwrite_int16(swap, block_align, file);
    fwrite_int16(swap, arguments->bps, file);
    fwrite("data", 4, 1, file);
    fwrite_int32(swap, data_size, file);
    fwrite(data, 1, data_size, file);
}
