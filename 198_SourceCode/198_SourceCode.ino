#include "MAX20102.h"
#include "OLED.h"
#include "buttons.h"

int prev=100;
int timer;
bool showHR = true;
unsigned long screenTimer = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Max30102_init();
  OLED_init();
  buttons_init();
  timer = millis();
}

void loop() {

    int filtered_chr = Max30102_filteredHR(prev);

    String nextAction = checkNextButton();
    String powerAction = checkPowerButton();
    String timerUpdate = updateTimer();

    bool redrawExercise = nextAction.length() > 0 || powerAction.length() > 0;
    bool screenChanged = false;

    // Screen switching
    if (millis() - screenTimer >= 2000) {
        showHR = !showHR;
        screenTimer = millis();
        screenChanged = true;
    }

    if (redrawExercise) {
        showHR = false;
        screenTimer = millis();
    }

    if (showHR) {
        OLED_display_HR(90, 153, filtered_chr);
    }
    else if (screenChanged || redrawExercise || timerUpdate.length() > 0) {
        displayCurrentExercise();
    }

    prev = filtered_chr;
}
