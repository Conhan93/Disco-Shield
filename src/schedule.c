#include "schedule.h"
#include "state_queue.h"

#include <timer.h>
#include <stdlib.h>
#include <shell.h>

#define MAX_EVENTS 10

static Event schedule[MAX_EVENTS] = {0};
static uint8_t nr_elements = 0;

static uint64_t time_last_pop = 0;
static uint64_t interval = 5000;

// gets all events
Event* schedule_get_all_events() {
    
    return schedule;
}
// add event to schedule
Event* schedule_add_event(Event _event) {
    if(nr_elements >= MAX_EVENTS)
        return NULL;
    schedule[nr_elements] = _event;
    nr_elements++;

    return &schedule[nr_elements-1];
}
int schedule_remove_event(Event* _event) {
    // remove event from schedule
    for(uint8_t index = 0 ; index < MAX_EVENTS ; index++) 
        if(&schedule[index] == _event) 
            for(uint8_t index2 = index ; index2 < MAX_EVENTS - 1 ; index2++)
                schedule[index2] = schedule[index2+1];
    
    nr_elements--;
    return 1;
}
// get event
Event* schedule_get_event(uint8_t index) {
    if(index >= nr_elements || index < 0)
        return NULL;
    
    return &schedule[index];
}

uint8_t schedule_swap(Event* _event1, Event* _event2) {
    if(_event1 && _event2) {
        Event temp = *_event1;
        *_event1 = *_event2;
        *_event2 = temp;
        return 1; 
    }
    return 0;
}


uint8_t schedule_pop(Event* _out) {
    if(nr_elements < 1) 
        return 0;
    
    *_out = schedule[0];

    return schedule_remove_event(&schedule[0]);
}

uint8_t schedule_nr_events() {
    return nr_elements;
}
// set update interval
void schedule_set_interval(uint64_t _interval) { interval = _interval; }
void schedule_update() {

    uint64_t now = millis();

    if(!schedule_nr_events())
        return;

    if(now - time_last_pop > interval) {

        state_queue_push_back(SCHEDULE_POP);
      
        time_last_pop = now;
    }
}