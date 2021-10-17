#include "state_queue.h"

#define QUEUE_MAX 20

static STATE_EVENT queue[QUEUE_MAX];
static uint8_t nr_elements = 0;

static void remove_element(STATE_EVENT* _event);

uint8_t state_queue_pop(STATE_EVENT* _out) {
    // if queue empty
    if(!nr_elements)
        return 0;
       
    *_out = queue[0];
    // remove event from queue'
    remove_element(&queue[0]);
    return 1;
}
uint8_t state_queue_push_back(STATE_EVENT _event) {
    // if queue full
    if(nr_elements >= QUEUE_MAX)
        return 0;

    queue[nr_elements] = _event;
    nr_elements++;
    return 1;
}
static void remove_element(STATE_EVENT* _event) {
    
    for(uint8_t index = 0 ; index < QUEUE_MAX ; index++) 
        if(&queue[index] == _event) 
            for(uint8_t index2 = index ; index2 < QUEUE_MAX - 1 ; index2++)
                queue[index2] = queue[index2+1];
    
    nr_elements--;
}