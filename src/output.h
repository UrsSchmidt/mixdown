#ifndef OUTPUT_HEADER_FILE
#define OUTPUT_HEADER_FILE

#include "shared.h"

#include <stdint.h>
#include <stdio.h>

void fwrite_aiff(struct arguments *arguments, uint32_t data_size, uint8_t *data, FILE *file);

void fwrite_wave(struct arguments *arguments, uint32_t data_size, uint8_t *data, FILE *file);

#endif
