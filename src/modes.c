#include "modes.h"
#include <util/delay.h>
#include <math.h>

#include "peripherals/rgb_led/rgb_led.h"

// generate fade pattern by calculating a single sine wave
void fade_lights() {

    rgb_value_t* rgb = led_get_rbg();
    uint8_t period = led_get_period();

    rgb[led_get_colour()] = 127*sin((((2*M_PI)/255)*(period))+(2/M_PI)) + 128;

}

// generate rainbow pattern with phase shifted sine waves
void rainbow_lights() {

    rgb_value_t* rgb = led_get_rbg();
    uint8_t period = led_get_period();

    // prime sinuswave phase
    rgb[RED] = 127*sin((((2*M_PI)/255)*(period))+((2*M_PI)/3)) + 128;
    // prime phase shifted left one-third a period
    rgb[GREEN] = 127*sin((((2*M_PI)/255)*(period))+((4*M_PI)/3)) + 128;

    // prime phase shifted left two-thirds a period
    rgb[BLUE] = 127*sin((((2*M_PI)/255)*(period))+((2*M_PI))) + 128;
    
}
// Shine solid light with current colour only
void solid_lights() {

    rgb_value_t* rgb = led_get_rbg();

    // all colours off
    rgb[RED] = rgb[GREEN] = rgb[BLUE] = 0;
    // desired colour on
    rgb[led_get_colour()] = 255;
}
// switch between shining solid colours
void blink_lights() {

    rgb_value_t* rgb = led_get_rbg();

    led_change_colour();
    RGB current_colour = led_get_colour();
    
    // all colours off
    led_dim();
    // desired colour on
    rgb[current_colour] = 255;
}