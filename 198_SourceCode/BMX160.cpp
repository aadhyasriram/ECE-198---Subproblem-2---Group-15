#include <Arduino.h>
#include <math.h>
#include "BMX160.h"
#define g 9.81
#define cycle 10000
#define alpha 0.8

DFRobot_BMX160 bmx160;

forearm_position current_position;
int half_rep_count;
double hammer_curl_error;
double angle_error;
unsigned long last_time;
float filter_output;
boolean filter_first_reading;
float theta;
boolean theta_first_reading;
int error_counter;
int error_counter_copy;
int last_reading_accel;
bool wait;

void imu_init(){
  Serial.begin(115200);
  delay(100);
  current_position = FLOATING;
  half_rep_count = 0;
  hammer_curl_error = 1;
  angle_error = 0.08726646259;
  last_time = 0;
  filter_first_reading = true;
  error_counter = 0;
  last_reading_accel = -10;
  theta = 0;
  theta_first_reading = true;
  wait = false;
  //init the hardware bmx160  
  if (bmx160.begin() != true){
    Serial.println("init false");
    while(1);
  }
  //bmx160.setLowPower();   //disable the gyroscope and accelerometer sensor
  //bmx160.wakeUp();        //enable the gyroscope and accelerometer sensor
  //bmx160.softReset();     //reset the sensor
  
  /** @typedef enum{eGyroRange_2000DPS,
   *                eGyroRange_1000DPS,
   *                eGyroRange_500DPS,
   *                eGyroRange_250DPS,
   *                eGyroRange_125DPS
   *                }eGyroRange_t;
   **/
  //bmx160.setGyroRange(eGyroRange_500DPS);
  
  /** @typedef enum{eAccelRange_2G,
   *                eAccelRange_4G,
   *                eAccelRange_8G,
   *                eAccelRange_16G
   *                }eAccelRange_t;
   */
  //bmx160.setAccelRange(eAccelRange_4G);
  delay(100);
}

int get_sensor_data(exercises excr){
  static int latest_reps = 0;
  sBmx160SensorData_t Omagn, Ogyro, Oaccel;

  /* Get a new sensor event */
  unsigned long current_time = micros();
  // if (current_time - last_time < cycle) {
  //   return latest_reps;
  // }
  last_time = current_time;
  bmx160.getAllData(&Omagn, &Ogyro, &Oaccel);
  Serial.print(Oaccel.x);
  Serial.print(" ");
  Serial.print(Oaccel.y);
  Serial.print(" ");
  Serial.println(Oaccel.z);
  latest_reps = rep_count(excr, Omagn, Oaccel, Ogyro);
  set_error_counter(excr, Omagn, Oaccel, Ogyro);
  return latest_reps;
}
int get_error_counter() {
  return error_counter;
}
int get_error_counter_copy() {
  return error_counter_copy;
}
void set_error_counter(exercises excr, sBmx160SensorData_t Omagn, sBmx160SensorData_t Oaccel, sBmx160SensorData_t Ogyro) {
  switch (excr) {
    case HAMMER_CURLS: {
      int value = Oaccel.z/g < -1 ? -1 : Oaccel.z/g;
      value = Oaccel.z/g;
      theta = acos(Oaccel.z/g);
      if (wait) {
        wait = false;
      }
      else if (current_position == DOWN && Oaccel.z < last_reading_accel - 2) {
        error_counter++;
        error_counter_copy++;
        Serial.println("Error! Didn't go up enough");
        // error count increases
      } else if (current_position == UP && Oaccel.z > last_reading_accel + 2) {
        error_counter++;
        error_counter_copy++;
        Serial.println("Error! Didn't go down enough");
        // error count increases
      }
      last_reading_accel = Oaccel.z;
    }
    default:
      return;
  }
}
void set_error_counter() {
  error_counter = 0;
}

int rep_count(exercises excr, sBmx160SensorData_t Omagn, sBmx160SensorData_t Oaccel, sBmx160SensorData_t Ogyro) {
  switch (excr) {
    case CARDIO:
      return 0;
    case HAMMER_CURLS: {
      // use forearm axis
      if ( 
        (Oaccel.z>= (0.0 - 1) )) 
      {
        // checks if in certain range
        if (current_position == FLOATING) {
          current_position = UP;
          // init to UP
          Serial.print("UP");
        } else if (current_position == DOWN) {
          ++half_rep_count;
          Serial.print("reached from down to up");
          wait = true;
          current_position = UP;
          // counts half rep
        }
      } else if ( Oaccel.z <= (g * -1 + 1)) 
      {
        if (current_position == FLOATING) {
          current_position = DOWN;
          // init to down
        } else if (current_position == UP) {
          ++half_rep_count;
          Serial.print("reached from up to down");
          wait = true;
          current_position = DOWN;
          // counts half rep
        }
      }
      return half_rep_count / 2;
    }
    case TRICEP_PUSHDOWN:
      // stuff
      return 0;
    case SHOULDER_PRESS:
      // stuff
      return 0;
    case SEATED_AB_CRUNCH:
      // stuff
      return 0;
    case LAT_PULLDOWN:
      //stuff
      return 0;
    case INCLINE_BENCH_PRESS:
      //stuff
      return 0;
    case DUMBBELL_SUMO_SQUAT:
      //stuff
      return 0;
    case ROMANIAN_DEADLIFT:
      //stuff
      return 0;
    default:
      return 0;
  }
}
