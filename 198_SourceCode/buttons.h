#ifndef BUTTONS_H
#define BUTTONS_H
#include <Arduino.h>

void check_buttons();
void buttons_init();
String checkPowerButton();
String checkNextButton();
void displayCurrentExercise();
String drawCardio();
String updateTimer();
String getCurrentExercise();

#endif