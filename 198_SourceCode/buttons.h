#ifndef BUTTONS_H
#define BUTTONS_H
#include <Arduino.h>
 
void check_buttons();
void buttons_init();
String checkPowerButton();
String checkNextButton();
void displayCurrentExercise();
void updateTimer();
int get_Current_Exercise();
int get_lastTickTime();
int get_TOTAL_EXERCISES();
#endif
 