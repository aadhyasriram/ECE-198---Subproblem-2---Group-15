#include "MAX20102.h"
#include "OLED.h"
#include "buttons.h"
int prev=100;
int time;
const char* exercise[9] = {
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
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Max30102_init();
  OLED_init();
  buttons_init();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (checkPowerButton()=="Cardio"){
    for (int k{0};k<100;k++){ //display for a while...
      time= millis()-get_lastTickTime();
      OLED_display_cardio(1, get_TOTAL_EXERCISES(), time/60000, (time- time%60000)/1000 );
    }
  }
  else if (checkPowerButton()=="Paused"){
    Serial.println("Paused");
    OLED_display_message("Paused",exercise[get_Current_Exercise()]);
  }
  else {
    Serial.println("Resumed");
    OLED_display_message("Resumed",exercise[get_Current_Exercise()]);
  }
  int chr= Max30102_readHR();
  int filtered_chr= Max30102_filteredHR(prev);
  checkNextButton();
  // OLED_display_HR(90, 153, filtered_chr); //Call to show the HR range and actual HR
  prev= filtered_chr;
  updateTimer();

}


