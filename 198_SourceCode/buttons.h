#ifndef BUTTONS_H
#define BUTTONS_H
#include <Arduino.h>
 
void check_buttons();//call to check both buttons
void powerb_ISR();//used for hardware interrupt (dont call)
void nextb_ISR();//used for hardware interrupt
String checkPowerButton();//call if powerb_event ==true. 
String checkNextButton();// handles debounce and returns info about the session
void reset_pb_deb_time();//call each time after checkPowerButton()
void reset_nb_deb_time();
int get_Current_Exercise();//in case the caller might want...
int get_lastTickTime();
int get_TOTAL_EXERCISES();
bool get_session_running();
const char** get_exercises();
volatile bool get_powerb_event();
volatile bool get_nextb_event();
bool updateTimer();//it will update the time and return true if time is updated



//uses OLED watch out for timing 
void buttons_init();//call in setup() 
void drawCardio();//helper func called by displayCurrentExercise or updateTimer
void displayCurrentExercise();
#endif
 