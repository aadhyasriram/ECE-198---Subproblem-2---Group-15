#include "buttons.h"
#include "OLED.h"
#include <Arduino.h>
 
const int POWER_BUTTON = 2;
const int NEXT_BUTTON = 3;
 
const unsigned long DEBOUNCE_DELAY = 50;
 
// Session states
bool sessionRunning = false;
bool sessionStarted = false;
 
// 0 = Cardio, 1-8 = Strength exercises
int currentExercise = 0;
 
const int TOTAL_EXERCISES = 9;
 
const char* exercises[TOTAL_EXERCISES] = {
  "Cardio",
  "Hammer Curls",
  "Tricep Pushdown",
  "Shoulder Press",
  "Seated Ab Crunch",
  "Lat Pulldown",
  "Incline Bench Press",
  "Dumbbell Sumo Squat",
  "Romanian Deadlift"
};
 
// Button debouncing variables
int lastPowerReading = HIGH;
int lastNextReading = HIGH;
 
int stablePowerState = HIGH;
int stableNextState = HIGH;
 
unsigned long lastPowerDebounceTime = 0;
unsigned long lastNextDebounceTime = 0;
 
// Cardio timer (15 minutes)
const unsigned long CARDIO_DURATION_MS = 15UL * 60UL * 1000UL;
unsigned long cardioRemainingMs = CARDIO_DURATION_MS;
unsigned long lastTickTime = 0;
long lastShownSeconds = -1;
bool cardioDone = false;
 
 
// Note: OLED_init() is called from setup() in the main file, before this.
void buttons_init() {
  pinMode(POWER_BUTTON, INPUT_PULLUP);
  pinMode(NEXT_BUTTON, INPUT_PULLUP);
 
  OLED_display_message(F("Exercise Tracker"), "Press POWER to begin");
}
 
 
void check_buttons() {
  checkPowerButton();
  checkNextButton();
}
 
 
// POWER BUTTON
void checkPowerButton() {
  int reading = digitalRead(POWER_BUTTON);
 
  if (reading != lastPowerReading) {
    lastPowerDebounceTime = millis();
  }
 
  if (millis() - lastPowerDebounceTime > DEBOUNCE_DELAY) {
 
    if (reading != stablePowerState) {
      stablePowerState = reading;
 
      // Button pressed
      if (stablePowerState == LOW) {
        if (!sessionStarted) {
          // First start: begin with cardio
          sessionStarted = true;
          currentExercise = 0;
          sessionRunning = true;
 
          // Start the 15 minute cardio timer
          cardioRemainingMs = CARDIO_DURATION_MS;
          cardioDone = false;
          lastTickTime = millis();
          lastShownSeconds = -1;
 
          displayCurrentExercise();
        }
        else if (sessionRunning) {
          // Pause session
          sessionRunning = false;
 
          OLED_display_message(F("Session Paused"), exercises[currentExercise]);
        }
        else {
          // Resume session
          sessionRunning = true;
 
          // Don't count the time spent paused
          lastTickTime = millis();
          lastShownSeconds = -1;
 
          displayCurrentExercise();
        }
      }
    }
  }
 
  lastPowerReading = reading;
}
 
 
// NEXT EXERCISE BUTTON
void checkNextButton() {
  int reading = digitalRead(NEXT_BUTTON);
 
  if (reading != lastNextReading) {
    lastNextDebounceTime = millis();
  }
 
  if (millis() - lastNextDebounceTime > DEBOUNCE_DELAY) {
 
    if (reading != stableNextState) {
      stableNextState = reading;
 
      // Button pressed
      if (stableNextState == LOW) {
 
        // Only advance during an active session
        if (sessionRunning) {
 
          // Check whether another exercise exists
          if (currentExercise < TOTAL_EXERCISES - 1) {
            currentExercise++;
            displayCurrentExercise();
          }
          else {
            // Already at the final exercise
            OLED_display_message(F("Workout Complete"), "No more exercises");
          }
        }
      }
    }
  }
 
  lastNextReading = reading;
}
 
 
// Sends the cardio countdown (or the "done" message) to the OLED
void drawCardio() {
  if (cardioDone) {
    OLED_display_message(F("Cardio Complete"), "Press NEXT");
    return;
  }
 
  long totalSeconds = (cardioRemainingMs + 999) / 1000;  // round up
  OLED_display_cardio(1, TOTAL_EXERCISES, totalSeconds / 60, totalSeconds % 60);
}
 
 
// DISPLAY CURRENT EXERCISE
void displayCurrentExercise() {
  if (currentExercise == 0) {
    drawCardio();
    return;
  }
 
  OLED_display_exercise(currentExercise + 1, TOTAL_EXERCISES, exercises[currentExercise]);
}
 
 
// CARDIO TIMER - call this every pass of loop()
void updateTimer() {
  // Only counts down during a running cardio exercise
  if (!sessionRunning || currentExercise != 0 || cardioDone) {
    return;
  }
 
  unsigned long now = millis();
  unsigned long elapsed = now - lastTickTime;
  lastTickTime = now;
 
  if (elapsed >= cardioRemainingMs) {
    cardioRemainingMs = 0;
    cardioDone = true;
    drawCardio();
    return;
  }
 
  cardioRemainingMs -= elapsed;
 
  // Only redraw when the displayed second changes
  long secondsLeft = (cardioRemainingMs + 999) / 1000;
  if (secondsLeft != lastShownSeconds) {
    lastShownSeconds = secondsLeft;
    drawCardio();
  }
}
 