#ifndef POT_H_INCLUDED
#define POT_H_INCLUDED

#include <stdint.h>

#define abs(a) (a > 0 ? a : -(a))

uint32_t pot_get();
void pot_update();

#endif