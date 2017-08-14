#include "shared.h"

#include <stdbool.h>
#include <stdint.h>

char *format_tostring(FORMAT format) {
    switch (format) {
    case AIFF:
        return "AIFF";
    case WAVE:
        return "WAVE";
    }
    return "?";
}

/* see: http://stackoverflow.com/a/14791372 */
bool swap_byteorder(FORMAT format) {
    const uint32_t magic = 0x00000001;
    const uint8_t black_magic = *(uint8_t*)&magic;
    switch (format) {
    case AIFF:
        return black_magic;
    case WAVE:
        return !black_magic;
    }
    return false;
}
