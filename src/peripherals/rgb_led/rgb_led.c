#include <timer.h>
#include <peripherals\rgb_led\rgb_led.h>
#include "avr/io.h"
#include "state.h"



// pins
#define BLUE_LED PB1
#define GREEN_LED PB2
#define RED_LED PB3

// current colour and rgb values for colour
static RGB current_colour = RED;
static rgb_value_t rgb[3] = {0,0,0};

// used for rate or speed
static uint64_t last_mil_led = 0;
static uint64_t rate_milliseconds = 0;

// period used for wave functions
static uint8_t period;


void led_dim() { rgb[RED] = rgb[GREEN] = rgb[BLUE] = 0; }
// advances to the next colour
void led_change_colour() {
  // save intensity value
  rgb_value_t cur_colour_val = rgb[current_colour];

  led_dim();
            
  current_colour = current_colour == RED ? BLUE :
    current_colour == BLUE ? GREEN : RED;
            
  rgb[current_colour] = cur_colour_val;

}

// SETTERS
void led_set_colour(RGB _colour) {current_colour = _colour;}
void led_set_rate(uint64_t rate) {rate_milliseconds = constrain(rate, 0 , 2000);}
uint8_t led_get_period() {return period; }
// GETTERS
RGB led_get_colour() {return current_colour; }
rgb_value_t* led_get_rbg() { return rgb; }
uint64_t led_get_rate() {return rate_milliseconds; }

// starts off the fade at solid color(if blink previous)
void led_transition_to_fade() { period = rgb[current_colour]; }

static void pmw_init() {

  /*
  * Set Timer Counter Control Register 1A
  *  To fast pwm mode with OCR1A and B(low and high) set at bottom(inverted)
  */
  TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << COM1A0) | (1 << COM1B0) | (1 << WGM10);
  TCCR1B |= (1 << CS10) | (1 << WGM12);
  /*
  * Set Timer Counter Control Register 2A
  *  To fast pwm mode with OCR2A set at bottom(inverted)
  */
  TCCR2A |= (1 << COM2A1) | (1 << COM2A0) | (1 << WGM20) | (1 << WGM21);
  TCCR2B |= (1 << CS20);

}

void init_rgb_led() {

    // set output port directions
    DDRB |= (1 << RED_LED) | (1 << GREEN_LED) | (1 << BLUE_LED);

    pmw_init();
}

// sends rgb values to output compare registers
static void set_diode() {
  OCR1A = rgb[BLUE];
  OCR1B = rgb[GREEN];
  OCR2A = rgb[RED];
}
void led_update() {
  
    if(millis() - last_mil_led > rate_milliseconds) {
      set_diode();

      if(state_get_current_mode() != NORMAL)
        period++;
      else
        period = 0;

      last_mil_led = millis();
    }
}