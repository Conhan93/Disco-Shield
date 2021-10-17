#ifndef STATE_QUEUE_INCLUDED
#define STATE_QUEUE_INCLUDED

#include "state.h"

uint8_t state_queue_pop(STATE_EVENT* _out);
uint8_t state_queue_push_back(STATE_EVENT _event);

#endif