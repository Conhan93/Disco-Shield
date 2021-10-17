#include "modes.h"
#include "serial.h"
#include "schedule.h"
#include "state.h"

#include <timer.h>
#include <peripherals\switches\switch.h>
#include <peripherals\potentiometer\pot.h>
#include <peripherals\rgb_led\rgb_led.h>

#include <shell.h>

void init_shell();
void check_peripherals();
void init_peripherals();

int main(void) {


  int pot_val = 0, pot_in = 0;

  init_timer();
  init_peripherals();

  init_shell();


  while(1) {

    // read serial and pass to shell
    if(serial_available())
      shell_receive_char(serial_getc());

    if(state_get() == STOPPED)
      continue;
    
    schedule_update();

    check_peripherals();

    state_update();
  }
}
void check_peripherals() {
    pot_update();
    led_update();
    switch_update();
}
void init_peripherals() {
    USART_Init();
    init_rgb_led();
    init_switch();
}
void init_shell() {
  
  // Configure shell
  sShellImpl shell_impl = {
    .send_char = serial_putc,
  };
  shell_boot(&shell_impl);
}