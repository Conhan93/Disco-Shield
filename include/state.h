#ifndef STATE_H_INCLUDED
#define STATE_H_INCLUDED

#include "peripherals/switches/switch.h"
#include "peripherals/rgb_led/types.h"

#define constrain(a, l ,h) (a < l ? l : a > h ? h : a)

typedef enum {
    RUNNING,
    STOPPED
} RunningState;

typedef enum  {
    KEY_1_PRESSED,
    KEY_2_PRESSED,
    POT_MOVED,
    SCHEDULE_POP,
    UI_RATE_INC,
    UI_RATE_DEC,
    UI_CHANGE_COLOUR,
    UI_CHANGE_MODE
}STATE_EVENT;

void state_update();


// set mode
void state_set_mode(MODE _mode);
// set running state to stopped
void state_stop();
// set running state to running
void state_start();

// Return running_state
RunningState state_get();
MODE state_get_current_mode();

#endif