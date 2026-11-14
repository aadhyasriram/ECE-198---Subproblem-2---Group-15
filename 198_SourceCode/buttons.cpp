#include "buttons.h"
#include "OLED.h"

 
const int POWER_BUTTON = 2;
const int NEXT_BUTTON = 3;

//used in hardware interrupt. So that button clicks can be detected even if the cpu is not currently checking the loop
volatile bool powerb_event= false;
volatile bool nextb_event=false;
 
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

  attachInterrupt(
    digitalPinToInterrupt(POWER_BUTTON),
    powerb_ISR,
    FALLING
  );

  attachInterrupt(
    digitalPinToInterrupt(NEXT_BUTTON),
    nextb_ISR,
    FALLING
  );
  OLED_display_message(F("Exercise Tracker"), "Press POWER to begin");
}

void powerb_ISR(){
  powerb_event=true;
}

void nextb_ISR(){
  nextb_event=true;
}
 
void check_buttons() {
  checkPowerButton();
  checkNextButton();
}
 
 
// POWER BUTTON (starts and pause)
//Cardio is returned when session is not started and button is pressed
//paused is returned when session is started and is running
//resumed is returned when session is not running and session is started
// "" is returned when we dont want to count the press
String checkPowerButton() {
  int reading = LOW; //if i call this its cuz button is was pressed
 
  if (millis() - lastPowerDebounceTime > DEBOUNCE_DELAY) { //button is pressed and we want change
    Serial.print("Elapsed: ");
    Serial.println(millis() - lastPowerDebounceTime);
      // Button pressed
    powerb_event= false;
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
 
      //displayCurrentExercise();
      return "Cardio"; //should be cardio
    }
    else if (sessionRunning) { //session started and is running. we want to pause it
      // Pause session
      sessionRunning = false;
 
      return "paused";
    }
    else {
    // Resume session
      sessionRunning = true;
 
      // Don't count the time spent paused
      lastTickTime = millis();
      lastShownSeconds = -1;
 
      return "resumed";
    }
  }
  return "";
}
 //call after chectPowerButton
void reset_pb_deb_time() {
  lastPowerDebounceTime = millis();
}

void reset_nb_deb_time(){
  lastNextDebounceTime=millis();
}
 
 
// NEXT EXERCISE BUTTON (change exercises)
//"Next!" is returned if session is running and we are not at the last exercise
// "Done!" is returned if session is at the last exercise
//"" is returned is session is not running or debounce stuff we dont count
String checkNextButton() {
  int reading = LOW;

  if (millis() - lastNextDebounceTime > DEBOUNCE_DELAY) {
    nextb_event= false;
 
    // Only advance during an active session
    if (sessionRunning) {
 
    // Check whether another exercise exists
      if (currentExercise < TOTAL_EXERCISES - 1) {
        currentExercise++;
        lastNextReading = reading;
        return "Next!";
      }
      else {
        // Already at the final exercise
        lastNextReading = reading;
        return "Done!";
      }
    }
  }
  return "";
 
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
bool updateTimer() {
  // Only counts down during a running cardio exercise
  if (!sessionRunning || currentExercise != 0 || cardioDone) {
    return false;
  }
  unsigned long now = millis();
  unsigned long elapsed = now - lastTickTime;
  lastTickTime = now;
 
  if (elapsed >= cardioRemainingMs) {
    cardioRemainingMs = 0;
    cardioDone = true;
    return true;
  }
 
  cardioRemainingMs -= elapsed;
 
  // Only redraw when the displayed second changes
  long secondsLeft = (cardioRemainingMs + 999) / 1000;
  if (secondsLeft != lastShownSeconds) {
    lastShownSeconds = secondsLeft;
    return true;
  }
  return false;
}
int get_Current_Exercise(){
  return currentExercise;
}
int get_lastTickTime(){
  return lastTickTime;
}
int get_TOTAL_EXERCISES(){
  return TOTAL_EXERCISES;
}

bool get_session_running(){
  return sessionRunning;
}
const char** get_exercises(){
  return exercises;
}

volatile bool get_powerb_event(){
  return powerb_event;
}
volatile bool get_nextb_event(){
  return nextb_event;
}