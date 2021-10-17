#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

#include <peripherals\rgb_led\types.h>



typedef struct {
    MODE mode;
    RGB colour;
    uint8_t duration;
    uint8_t rate;
} Event;

Event construct_event(MODE _mode, RGB _col, uint8_t duration,
     uint8_t _rate);

#endif