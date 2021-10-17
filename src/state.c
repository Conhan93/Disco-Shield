#include "state.h"
#include "state_queue.h"
#include "schedule.h"
#include "modes.h"

#include "peripherals/potentiometer/pot.h"
#include "peripherals/switches/switch.h"
#include "peripherals/rgb_led/rgb_led.h"

typedef void (*event_operation)();
typedef void (*led_mode)();

static RunningState running_state = RUNNING;

static MODE mode = FADE;

static void change_mode();
static void run_event_from_schedule();
static void read_pot_val();
static void serial_decrement_rate();
static void serial_increment_rate();


static event_operation events[8] = 
    {&led_change_colour, &change_mode, &read_pot_val, &run_event_from_schedule,
     &serial_increment_rate, &serial_decrement_rate, &led_change_colour, &change_mode};

static led_mode modes[4] = 
    {&fade_lights, &rainbow_lights, &solid_lights, &blink_lights};

void state_update() {
    // run mode
    modes[mode]();

    // run queued events
    STATE_EVENT cur_event;
    while(state_queue_pop(&cur_event)) {
        events[cur_event]();
    }
}

void state_stop() { running_state = STOPPED; }
void state_start() { running_state = RUNNING; }
RunningState state_get() { return running_state; }
void state_set_mode(MODE _mode) { mode = _mode; }
static void change_mode() {

    switch (mode)
        {
        case FADE:
            mode = RAINBOW;
            break;
        case RAINBOW:
            mode = NORMAL;
            break;
        case NORMAL:
            mode = BLINK;
            break;
        case BLINK:
            mode = FADE;
            led_transition_to_fade();
        default:
            mode = FADE;
            break;
        }
}
static void read_pot_val() {
    led_set_rate(pot_get());
}
static void serial_decrement_rate() {
    led_set_rate(constrain(led_get_rate() - 50, 0, 2000)); 
}
static void serial_increment_rate() {
    led_set_rate(constrain(led_get_rate() + 50, 0, 2000)); 
}

static void run_event_from_schedule() {
    Event current_event;

    if(schedule_pop(&current_event)) {
        // update
        state_set_mode(current_event.mode);

        led_set_colour(current_event.colour);

        schedule_set_interval((uint64_t)current_event.duration * 1000);
        led_set_rate(1023 - ((1023/100)*current_event.rate));
        
        led_dim();
      }
}
MODE state_get_current_mode() {return mode;}