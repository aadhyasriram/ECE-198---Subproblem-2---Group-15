#include "MAX20102.h"
#include "OLED.h"
#include "buttons.h"
#include "BMX160.h"
#include <EEPROM.h>

int prev=100;

const char** exercise= get_exercises();
unsigned long lastDisplayTime = 0;
const unsigned long DISPLAY_INTERVAL = 3000;
exercises allExercises[9] = {CARDIO,HAMMER_CURLS,TRICEP_PUSHDOWN,
                            SHOULDER_PRESS,SEATED_AB_CRUNCH,LAT_PULLDOWN,
                            INCLINE_BENCH_PRESS, DUMBBELL_SUMO_SQUAT, ROMANIAN_DEADLIFT};

int displayPage = 0;
bool showRestingHR = false;

int HRR = 0;
int targetHRMin = 0;
int targetHRMax = 0;
int cur_exc = 0;

int amountOfHRs = 0;

// values to be saved
int highestHR = 0;
int lowestHR = 999;
int avgHR = 0;
int repsPerExercise[9]{};
int errorsPerExercise[9]{};
int currentRestingHR = 0;

// address
int currAddr = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Max30102_init();
  imu_init();
  OLED_init();
  buttons_init();
}

void loop() {

  // 1. BUTTON HANDLING

  if (get_powerb_event()) {
    String info = checkPowerButton();
    reset_pb_deb_time();

    if (info == "Cardio") {
      showRestingHR = true;
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
    // get reps
    int reps{};
    reps = get_sensor_data(allExercises[get_Current_Exercise()]);
     // could be replaced with hashmap
    // values to be saved to persistent memory
    lowestHR = lowestHR > filtered_chr ? filtered_chr : lowestHR;
    highestHR = highestHR < filtered_chr ? filtered_chr : highestHR;
    amountOfHRs++;
    avgHR = ((avgHR * (amountOfHRs - 1)) + filtered_chr)/amountOfHRs;
    repsPerExercise[get_Current_Exercise()] = reps;
    errorsPerExercise[get_Current_Exercise()] = get_error_counter_copy();
    // eeprom
    currAddr = 0;
    EEPROM.put(currAddr, lowestHR);
    currAddr += sizeof(int);
    EEPROM.put(currAddr, highestHR);
    currAddr += sizeof(int);
    EEPROM.put(currAddr, avgHR);
    for (int i : repsPerExercise) {
      currAddr += sizeof(int);
      EEPROM.put(currAddr, i);
    }
    currAddr += sizeof(int);
    EEPROM.put(currAddr,currentRestingHR);
    for (int i : errorsPerExercise) {
      currAddr += sizeof(int);
      EEPROM.put(currAddr, i);
    }
    // 3. OLED PAGE SWITCHING

    if (millis() - lastDisplayTime >= DISPLAY_INTERVAL) {
      if (displayPage == 0) {
        if (showRestingHR) {
          OLED_display_message("Calc Resting HR...","");
          currentRestingHR = restingHR();
          showRestingHR = false;
        }
        cur_exc= get_Current_Exercise();
        if ((cur_exc == 0)&&(timer_changed)) {
          drawCardio();
        }
        else if(cur_exc==1){
          if (get_error_counter() >= 2) {
            OLED_warning("Range of motion not enough!");
            set_error_counter();
          }else {
            OLED_display_rep(reps);
          }
        }
        else {
          displayCurrentExercise();
        }
      }
      else {
        HRR = 153 - currentRestingHR;
        targetHRMin = HRR *  5/ 10 + currentRestingHR;
        targetHRMax = HRR * 85 / 100 + currentRestingHR;
        OLED_display_HR(targetHRMin, targetHRMax, filtered_chr);
      }

      displayPage = (displayPage + 1) % 2;
      lastDisplayTime = millis();
    }
  }
}


