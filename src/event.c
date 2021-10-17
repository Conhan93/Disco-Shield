#include "event.h"



Event construct_event(MODE _mode, RGB _col, uint8_t duration,
     uint8_t _rate) {
    Event new_event;

    new_event.mode = _mode;
    new_event.colour = _col;
    new_event.duration = duration;
    new_event.rate = _rate;

    return new_event;
}