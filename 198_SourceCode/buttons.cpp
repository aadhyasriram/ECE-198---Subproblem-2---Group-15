#include "buttons.h"
const int POWER_BUTTON = 2;
const int NEXT_BUTTON = 3;

const unsigned long DEBOUNCE_DELAY = 50;

// Session states
bool sessionRunning = false;
bool sessionStarted = false;

// 0 = Cardio, 1–8 = Strength exercises
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


void setup() {
  Serial.begin(9600);

  pinMode(POWER_BUTTON, INPUT_PULLUP);
  pinMode(NEXT_BUTTON, INPUT_PULLUP);

  Serial.println("Exercise Tracker Ready");
  Serial.println("Press POWER to begin.");
}


void loop() {
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

          Serial.println("Session Started!");
          displayCurrentExercise();
        }
        else if (sessionRunning) {
          // Pause session
          sessionRunning = false;

          Serial.println("Session Paused.");
        }
        else {
          // Resume session
          sessionRunning = true;

          Serial.println("Session Resumed!");
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

            Serial.println("Moving to next exercise:");
            displayCurrentExercise();
          }
          else {
            // Already at the final exercise
            Serial.println("No more exercises.");
          }
        }
      }
    }
  }

  lastNextReading = reading;
}


// DISPLAY CURRENT EXERCISE
void displayCurrentExercise() {
  Serial.print("Current Exercise: ");
  Serial.println(exercises[currentExercise]);
}


