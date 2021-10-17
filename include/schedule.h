#ifndef SCHEDULE_H_INCLUDED
#define SCHEDULE_H_INCLUDED

#include "event.h"

Event* schedule_get_all_events();
Event* schedule_get_event(uint8_t index);
uint8_t schedule_nr_events();

Event* schedule_add_event(Event _event);
int schedule_remove_event(Event* _event);

uint8_t schedule_pop(Event* _out);
uint8_t schedule_swap(Event* _event1, Event* _event2);

void schedule_update();
void schedule_set_interval(uint64_t);

#endif