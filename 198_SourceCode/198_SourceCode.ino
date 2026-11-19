#include "MAX20102.h"
#include "OLED.h"
#include "buttons.h"
int prev=100;

const char** exercise= get_exercises();
unsigned long lastDisplayTime = 0;
const unsigned long DISPLAY_INTERVAL = 3000;

int displayPage = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Max30102_init();
  OLED_init();
  buttons_init();
}

void loop() {

  // 1. BUTTON HANDLING

  if (get_powerb_event()) {
    String info = checkPowerButton();
    reset_pb_deb_time();

    if (info == "Cardio") {
      Serial.println("Cardio");

      // Reset display timing when cardio starts
      lastDisplayTime = millis();
      displayPage = 0;
    }
    else if (info == "paused") {
      Serial.println("paused");
      OLED_display_message("Paused", exercise[get_Current_Exercise()]);
    }
    else if (info == "resumed") {
      Serial.println("resumed");
      lastDisplayTime = millis();
      displayPage = 0;
      displayCurrentExercise();
    }
    else{

      Serial.println("Power: Debounce doesnt count");
    }
  }

  if (get_nextb_event()) {
    String info = checkNextButton();
    reset_nb_deb_time();

    if (info == "Next!") {
      Serial.println("Next!");
      lastDisplayTime = millis();
      displayPage = 0;
      displayCurrentExercise();
    }
    else if (info == "Done!") {
      Serial.println("Done!");
    }
    else{
      Serial.println("Next: Debounce doesnt count");
    }
  }


    // 2. CONTINUOUS SESSION EXECUTION

  if (get_session_running()) {
    bool timer_changed= updateTimer();
    // Read heart rate
    int filtered_chr = Max30102_filteredHR(prev);
    prev = filtered_chr;

    // 3. OLED PAGE SWITCHING

    if (millis() - lastDisplayTime >= DISPLAY_INTERVAL) {

      lastDisplayTime = millis();

      if (displayPage == 0) {
        if ((get_Current_Exercise() == 0)&&(timer_changed)) {
          drawCardio();
        }
        else {
          displayCurrentExercise();
        }
      }
      else {
        OLED_display_HR(90, 153, filtered_chr);
      }

      displayPage = (displayPage + 1) % 2;
    }
  }
}


