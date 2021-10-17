#ifndef RGB_LED_H_INCLUDED
#define RGB_LED_H_INCLUDED

#include "types.h"

void init_rgb_led();

void led_update();



void led_set_rate(uint64_t);
void led_set_colour(RGB _colour);

void led_change_colour();
void led_transition_to_fade();


RGB led_get_colour();
rgb_value_t* led_get_rbg();
uint8_t led_get_period();
uint64_t led_get_rate();

void led_dim();

#endif