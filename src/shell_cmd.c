#include <shell.h>
#include "schedule.h"
#include "state.h"
#include "state_queue.h"
#include "avr/pgmspace.h"

#include "stdio.h"
#include "string.h"
#include "math.h"

// status messsages
static void shell_fail(uint16_t code);
static void shell_ok();

/////// - helpers
static inline uint8_t char_to_uint8(char digit);
static uint8_t string_to_uint8(char* str, uint8_t* _out);

static void mode_to_text(char* buf, MODE _mode);
static void colour_to_text(char* buf, RGB _mode);
static MODE text_to_mode(char* buf);
static RGB text_to_colour(char* buf);

static const char mode_table[][8] = {"FADE", "RAINBOW", "SOLID", "BLINK"};
static const char colour_table[][6] = {"RED", "GREEN", "BLUE"};
/////

// display schedule on terminal
int cli_cmd_view(int argc, char** argv) {
    if(argc != 1) {
      shell_fail(1);
      return;
    }
    if(!schedule_nr_events()) {
      shell_fail(2);
      return;
    }
    Event* schedule = schedule_get_all_events();

    char buf[128], mode_buf[10], col_buf[10];
    Event _event;

    for(uint8_t index = 0 ; index < schedule_nr_events() ; index++) {
      _event = schedule[index];

      mode_to_text(mode_buf, _event.mode);
      colour_to_text(col_buf, _event.colour);

      sprintf(buf,"%d\t%s\t%d\t%d\t%s", index+1, mode_buf, _event.duration,
          _event.rate, col_buf);

      shell_put_line(buf);
    }
    shell_ok();
    return 0;
}
// add event to schedule
int cli_cmd_add(int argc, char** argv) {
    if(argc != 5) {
      shell_fail(1);
      return;
    }

    uint8_t rate, duration;

    if(!(string_to_uint8(argv[4], &rate))) {
        shell_fail(2);
        return;
    }
     if(!(string_to_uint8(argv[3], &duration))) {
        shell_fail(3);
        return;
    }

    Event _event = construct_event(text_to_mode(argv[1]), text_to_colour(argv[2]),
      duration, rate);

    if(!schedule_add_event(_event)) {
      shell_fail(4);
      return;
    }

    shell_ok();
    return 0;
}
// modify(overwrite) an event in schedule
int cli_cmd_modify(int argc, char** argv) {
    if(argc != 6) {
      shell_fail(1);
      return;
    }

    uint8_t index; 
    if(!string_to_uint8(argv[1], &index)) {
      shell_fail(2);
      return;
    }
    Event* _event = NULL;
    if(!(_event = schedule_get_event(index - 1))) {
        shell_fail(3);
        return;
    }

    uint8_t rate, duration;

    if(!(string_to_uint8(argv[5], &rate))) {
        shell_fail(4);
        return;
    }
     if(!(string_to_uint8(argv[4], &duration))) {
        shell_fail(5);
        return;
    }

    *_event = construct_event(text_to_mode(argv[2]), text_to_colour(argv[3]),
      duration, rate);

    

    shell_ok();
    return 0;
}
// move events in schedule
int cli_cmd_move(int argc, char** argv) {
  if(argc != 3) {
    shell_fail(1);
    return;
  }

  int index1, index2;

  if(!string_to_uint8(argv[1], &index1)) {
    shell_fail(2);
    return;
  }
  if(!string_to_uint8(argv[2], &index2)) {
    shell_fail(3);
    return;
  }

  Event* _event1 = schedule_get_event(index1-1);
  Event* _event2 = schedule_get_event(index2-1);
  if(!_event1) {
    shell_fail(4);
    return;
  }
  if(!_event2) {
    shell_fail(5);
    return;
  }

  if(!schedule_swap(_event1, _event2)) {
    shell_fail(6);
    return;
  }

  shell_ok();
  return 0;
}
// delete an event in schedule
int cli_cmd_remove_event(int argc, char** argv) {
    if(argc != 2) {
      shell_fail(1);
      return;
    }

    uint8_t index = char_to_uint8(argv[1][0]) - 1;
    if(index > 9 || index < 0) {
      shell_fail(2);
      return;
    }

    Event* _event = schedule_get_event(index);
    if(!_event) {
      shell_fail(3);
      return;
    }

    if(!schedule_remove_event(_event)) {
      shell_fail(4);
      return;
    }

    shell_ok();
    return 0;
}
int cli_cmd_start(int argc, char** argv) {
    state_start();
    shell_ok();
    return 0;
}
int cli_cmd_stop(int argc, char** argv) {
    state_stop();
    shell_ok();
    return 0;
}
int cli_cmd_inc(int argc, char** argv) {
  state_queue_push_back(UI_RATE_DEC);
  return 0;
}
int cli_cmd_dec(int argc, char** argv) {
  state_queue_push_back(UI_RATE_INC);
  return 0;
}
int cli_cmd_next_colour(int argc, char** argv) {
  state_queue_push_back(UI_CHANGE_COLOUR);
  return 0;
}
int cli_cmd_change_mode(int argc, char** argv) {
  state_queue_push_back(UI_CHANGE_MODE);
  return 0;
}
// prints FAIL with fail code to serial
static void shell_fail(uint16_t code) {
    char buf[10];
    sprintf(buf, "FAIL : %u", code);
    shell_put_line(buf);
}
// prints OK message to terminal
static void shell_ok() {
  shell_put_line("OK");
  return;
}


static const char view_str[] PROGMEM = "Prints schedule";
static const char add_str[] PROGMEM = "Add event to schedule : args MODE COLOUR DURATION(in seconds) RATE(0-100)";
static const char mod_str[] PROGMEM = "Modify event in schedule : args INDEX MODE COLOUR DURATION(in seconds) RATE(0-100)";
static const char mov_str[] PROGMEM = "Move event in schedule : args INDEX_FROM INDEX_TO";
static const char remove_str[] PROGMEM = "Remove event from schedule : args INDEX";
static const char start_str[] PROGMEM = "Starts or unpauses machine";
static const char stop_str[] PROGMEM = "Pauses machine - useful for adding events to schedule";
static const char inc_str[] PROGMEM = "Increases the speed setting used for modes";
static const char dec_str[] PROGMEM = "Decreases the speed setting used for modes";
static const char next_str[] PROGMEM = "Cycles to the next colour";
static const char mode_str[] PROGMEM = "Cycles to the next mode";
static const char help_str[] PROGMEM = "Lists all commands";

// list of shell commands
static const sShellCommand s_shell_commands[] = {
    {"view", cli_cmd_view, view_str},
    {"add", cli_cmd_add, add_str},
    {"modify", cli_cmd_modify, mod_str},
    {"move", cli_cmd_move , mov_str},
    {"remove", cli_cmd_remove_event, remove_str},
    {"start", cli_cmd_start, start_str},
    {"stop", cli_cmd_stop, stop_str},
    {"inc", cli_cmd_inc, inc_str},
    {"dec", cli_cmd_dec, dec_str},
    {"next_colour", cli_cmd_next_colour, next_str},
    {"next_mode", cli_cmd_change_mode, mode_str},
    {"help", shell_help_handler, help_str},
};

const sShellCommand *const g_shell_commands = s_shell_commands;
const size_t g_num_shell_commands = sizeof(s_shell_commands)/sizeof(s_shell_commands[0]);


//// UI - helpers


static inline uint8_t char_to_uint8(char digit) { return digit - '0'; }
// unsigned power
static inline uint64_t powul(uint64_t base, uint64_t exponent) {
  if(!exponent)
    return 1;
  uint64_t product = 1;
  while (exponent--)
    product *= base;
  return product;
}

static uint8_t string_to_uint8(char* str, uint8_t* _out) {
  char* _str = str; 

  // advance till end
  while(*_str++ != '\0') ;

  char* end = _str;
  _str = str;

  // get length
  uint16_t len = end - _str;
  if(len > 4 || len < 1) {
    return 0;
  }

  uint16_t sum = 0;
  // add sum per decimal place
  uint8_t power = len - 2;
  for(uint8_t iter = 0 ; iter < len -1 ; iter++) {
    sum += (uint16_t)(char_to_uint8(*_str++))*powul(10, power--);
  }


  *_out = constrain(sum, 0 , 100);

  return 1;
} 

static void mode_to_text(char* buf, MODE _mode) { strcpy(buf, mode_table[_mode]);}
static void colour_to_text(char* buf, RGB _mode) { strcpy(buf, colour_table[_mode]);}
static MODE text_to_mode(char* buf) { 
    for(uint8_t index = 0 ; index < 4 ; index++)
      if(!strcmp(buf, mode_table[index])) {
        return index;
      }
}
static RGB text_to_colour(char* buf) { 
    for(uint8_t index = 0 ; index < 3 ; index++)
      if(!strcmp(buf, colour_table[index])) {
        return index;
      }
}