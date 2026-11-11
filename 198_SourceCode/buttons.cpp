#include "buttons.h"
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
 
// OLED settings
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;
const uint8_t OLED_ADDRESS = 0x3C;   // try 0x3D if the screen stays blank
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
extern Adafruit_SSD1306 display;
 
// Layout: small header on the top row, main content fills the rest
const int HEADER_HEIGHT = 12;
const int BODY_TOP = HEADER_HEIGHT;
const int BODY_HEIGHT = SCREEN_HEIGHT - HEADER_HEIGHT;
 
const int POWER_BUTTON = 2;
const int NEXT_BUTTON = 3;
 
const unsigned long DEBOUNCE_DELAY = 50;
 
// Session states
bool sessionRunning = false;
bool sessionStarted = false;
 
// 0 = Cardio, 1-8 = Strength exercises
int currentExercise = 0;
 
const int TOTAL_EXERCISES = 9;
 
const String exercises[TOTAL_EXERCISES] = {
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
 
 
// ---------- Text layout helpers ----------
 
// Splits text into lines at spaces so that no line is longer than maxChars.
// Returns the number of lines, or -1 if it doesn't fit (a word that is too
// long, or more than maxLines lines needed).
int wrapText(const char* t, int maxChars, int maxLines, int starts[], int lens[]) {
  int len = strlen(t);
  int n = 0;
  int i = 0;
 
  while (i < len) {
    while (i < len && t[i] == ' ') i++;
    if (i >= len) break;
    if (n >= maxLines) return -1;
 
    int start = i;
    int end = i;
 
    while (i < len) {
      int wordStart = i;
      while (i < len && t[i] != ' ') i++;
      int wordEnd = i;
 
      if (wordEnd - wordStart > maxChars) return -1;
 
      if (wordEnd - start > maxChars) {
        i = wordStart;   // this word goes on the next line
        break;
      }
 
      end = wordEnd;
      while (i < len && t[i] == ' ') i++;
    }
 
    starts[n] = start;
    lens[n] = end - start;
    n++;
  }
 
  return n;
}
 
 
// Draws text centered in the given area, using the largest size that fits
// (tries size 3, then 2, then 1) and wrapping onto lines at spaces.
void drawCentered(const char* text, int areaTop, int areaHeight) {
  int starts[6];
  int lens[6];
 
  display.setTextColor(SSD1306_WHITE);
 
  for (int size = 3; size >= 1; size--) {
    int maxChars = SCREEN_WIDTH / (6 * size);
    int maxLines = areaHeight / (8 * size);
    if (maxLines > 6) maxLines = 6;
 
    int n = wrapText(text, maxChars, maxLines, starts, lens);
 
    if (n > 0) {
      display.setTextSize(size);
      int blockHeight = n * 8 * size;
      int y = areaTop + (areaHeight - blockHeight) / 2;
 
      for (int line = 0; line < n; line++) {
        int x = (SCREEN_WIDTH - lens[line] * 6 * size) / 2;
        display.setCursor(x, y + line * 8 * size);
        for (int k = 0; k < lens[line]; k++) {
          display.write(text[starts[line] + k]);
        }
      }
      return;
    }
  }
 
  // Fallback for text that can't be wrapped to fit
  display.setTextSize(1);
  display.setCursor(0, areaTop);
  display.print(text);
}
 
 
// Small header on the top row, main text centered in the rest of the screen
void showMessage(const __FlashStringHelper* header, const char* body) {
  display.clearDisplay();
 
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(header);
 
  drawCentered(body, BODY_TOP, BODY_HEIGHT);
 
  display.display();
}
 
 
void buttons_init() {
  pinMode(POWER_BUTTON, INPUT_PULLUP);
  pinMode(NEXT_BUTTON, INPUT_PULLUP);
 
  // If the OLED isn't found, the code keeps running instead of freezing
 // display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);
 
  showMessage(F("Exercise Tracker"), "Press POWER to begin");
}
 
 
void check_buttons() {
  checkPowerButton();
  checkNextButton();
}
 
 
// POWER BUTTON
String checkPowerButton() {
  int reading = digitalRead(POWER_BUTTON);

  if (reading != lastPowerReading) {
    lastPowerDebounceTime = millis();
    Serial.print("clicked Power");
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
 
          return exercises[currentExercise];
        }
        else if (sessionRunning) {
          // Pause session
          updateTimer();
          sessionRunning = false;
 
          return String("Paused: ") + exercises[currentExercise];
        }
        else {
          // Resume session
          sessionRunning = true;
 
          // Don't count the time spent paused
          lastTickTime = millis();
          lastShownSeconds = -1;
 
          return exercises[currentExercise];
        }
      }
    }
  }
 
  lastPowerReading = reading;
  return "";
}
 
 
// NEXT EXERCISE BUTTON
String checkNextButton() {
  int reading = digitalRead(NEXT_BUTTON);
 
  if (reading != lastNextReading) {
    
    lastNextDebounceTime = millis();
    Serial.print("clicked Next");
  }
 
  if (millis() - lastNextDebounceTime > DEBOUNCE_DELAY) {
 
    if (reading != stableNextState) {
      stableNextState = reading;
 
      // Button pressed
      if (stableNextState == LOW) {
 
        // Allow advancing while paused, but not before the session starts.
        if (sessionStarted) {
 
          // Check whether another exercise exists
          if (currentExercise < TOTAL_EXERCISES - 1) {
            currentExercise++;
            return exercises[currentExercise];
          }
          else {
            // Already at the final exercise
            return "No more exercises";
          }
        }
      }
    }
  }
 
  lastNextReading = reading;
  return "";
}
 
 
// // Cardio screen: header, "Cardio" label, and a big countdown that fills the rest
// String drawCardio() {
//   if (cardioDone) {
//     showMessage(F("Cardio Complete"), "Press NEXT");
//     return "";
//   }
 
//   long totalSeconds = (cardioRemainingMs + 999) / 1000;  // round up
//   int minutes = totalSeconds / 60;
//   int seconds = totalSeconds % 60;
 
//   char timeText[6];
//   timeText[0] = '0' + (minutes / 10);
//   timeText[1] = '0' + (minutes % 10);
//   timeText[2] = ':';
//   timeText[3] = '0' + (seconds / 10);
//   timeText[4] = '0' + (seconds % 10);
//   timeText[5] = '\0';
//   return timeText;
 
//   display.clearDisplay();
//   display.setTextColor(SSD1306_WHITE);
 
//   // Header
//   display.setTextSize(1);
//   display.setCursor(0, 0);
//   display.print(F("Exercise 1 of "));
//   display.print(TOTAL_EXERCISES);
 
//   // "Cardio" label, size 2, centered (6 chars * 12 px = 72 px wide)
//   display.setTextSize(2);
//   display.setCursor((SCREEN_WIDTH - 72) / 2, 11);
//   display.print(F("Cardio"));
 
//   // Timer, size 4, centered (5 chars * 24 px = 120 px wide)
//   display.setTextSize(4);
//   display.setCursor((SCREEN_WIDTH - 120) / 2, 30);
//   display.print(timeText);
 
//   display.display();
// }
String drawCardio() {

    if (cardioDone) {
        showMessage(F("Cardio Complete"), "Press NEXT");
        return "";
    }

    long totalSeconds = (cardioRemainingMs + 999) / 1000;
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    char timeText[6];

    sprintf(timeText, "%02d:%02d", minutes, seconds);

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    display.setTextSize(1);
    display.setCursor(0, 0);
    if (sessionRunning) {
        display.print(F("Exercise 1 of "));
        display.print(TOTAL_EXERCISES);
    } else {
        display.print(F("PAUSED"));
    }

    display.setTextSize(2);
    display.setCursor((SCREEN_WIDTH - 72) / 2, 11);
    display.print(F("Cardio"));

    display.setTextSize(4);
    display.setCursor((SCREEN_WIDTH - 120) / 2, 30);
    display.print(timeText);

    display.display();

    return timeText;
}
 
// DISPLAY CURRENT EXERCISE
void displayCurrentExercise() {
  if (!sessionStarted) {
    showMessage(F("Exercise Tracker"), "Press POWER to begin");
    return;
  }

  if (currentExercise == 0) {
    drawCardio();
    return;
  }
 
  display.clearDisplay();
 
  // Header
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  if (sessionRunning) {
    display.print(F("Exercise "));
    display.print(currentExercise + 1);
    display.print(F(" of "));
    display.print(TOTAL_EXERCISES);
  } else {
    display.print(F("PAUSED"));
  }
 
  // Exercise name, auto-sized and centered
  drawCentered(exercises[currentExercise].c_str(),
             BODY_TOP,
             BODY_HEIGHT);
 
  display.display();
}
 
 
// CARDIO TIMER - call this every pass of loop()
// String updateTimer() {
//   // Only counts down during a running cardio exercise
//   if (!sessionRunning || currentExercise != 0 || cardioDone) {
//     return "";
//   }
 
//   unsigned long now = millis();
//   unsigned long elapsed = now - lastTickTime;
//   lastTickTime = now;
 
//   if (elapsed >= cardioRemainingMs) {
//     cardioRemainingMs = 0;
//     cardioDone = true;
//     return "";
//   }
 
//   cardioRemainingMs -= elapsed;
 
//   // Only redraw when the displayed second changes
//   long secondsLeft = (cardioRemainingMs + 999) / 1000;
//   if (secondsLeft != lastShownSeconds) {
//     lastShownSeconds = secondsLeft;
//     return drawCardio();
//   }
//   return "";
// }
String updateTimer() {

    if (!sessionStarted || !sessionRunning || currentExercise != 0 || cardioDone) {
        return "";
    }

    unsigned long now = millis();
    unsigned long elapsed = now - lastTickTime;
    lastTickTime = now;

    if (elapsed >= cardioRemainingMs) {
        cardioRemainingMs = 0;
        cardioDone = true;
        lastShownSeconds = 0;
        return "00:00";
    }

    cardioRemainingMs -= elapsed;

    long secondsLeft = (cardioRemainingMs + 999) / 1000;

    if (secondsLeft != lastShownSeconds) {
        lastShownSeconds = secondsLeft;
        int minutes = secondsLeft / 60;
        int seconds = secondsLeft % 60;
        char timeText[6];
        sprintf(timeText, "%02d:%02d", minutes, seconds);
        return String(timeText);
    }

    return "";
}
String getCurrentExercise() {
  return exercises[currentExercise];
}