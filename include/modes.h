#ifndef MODES_INCLUDED
#define MODES_INCLUDED

#include <stdint.h>
#include <peripherals\rgb_led\types.h>

void fade_lights();
void rainbow_lights();
void solid_lights();
void blink_lights();

#endif