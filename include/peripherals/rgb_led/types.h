#ifndef TYPES_INCLUDED
#define TYPES_INCLUDED

#include <stdint.h>

typedef enum {
  RED,
  GREEN,
  BLUE
} RGB;

typedef enum {
  FADE,
  RAINBOW,
  NORMAL,
  BLINK
} MODE;

typedef uint8_t rgb_value_t;

#endif