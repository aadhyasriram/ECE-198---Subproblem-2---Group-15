/*!
  * file readAllData.ino
  *
  * Through the example, you can get the sensor data by using getSensorData:
  * get all data of magnetometer, gyroscope, accelerometer.
  * 
  * With the rotation of the sensor, data changes are visible.
  *
  * Copyright   [DFRobot](https://www.dfrobot.com), 2016
  * Copyright   GNU Lesser General Public License
  *
  * version  V0.1
  * date  2019-6-25
  */

#include <Arduino.h>
#include "BMX160.h"
#define g 9.81
#define cycle 10000
#define alpha 0.8

DFRobot_BMX160 bmx160;

forearm_position current_position;
int half_rep_count;
double hammer_curl_error;
unsigned long last_time;
float filter_output;
boolean filter_first_reading;

void imu_init(){
  Serial.begin(115200);
  delay(100);
  current_position = FLOATING;
  half_rep_count = 0;
  hammer_curl_error = 1;
  last_time = 0;
  filter_first_reading = true;
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

void get_sensor_data(exercises excr){
  sBmx160SensorData_t Omagn, Ogyro, Oaccel;

  /* Get a new sensor event */
  unsigned long current_time = micros();
  if (current_time - last_time >= cycle) {
    last_time += cycle;
    bmx160.getAllData(&Omagn, &Ogyro, &Oaccel);
    // selected exercise will be passed in
    Serial.println(rep_count(excr, Omagn, Oaccel, Ogyro));
  }
}

// TODO: interpret data from sensors
int rep_count(exercises excr, sBmx160SensorData_t Omagn, sBmx160SensorData_t Oaccel, sBmx160SensorData_t Ogyro) {
  switch (excr) {
    case CARDIO:
      return 0;
    case HAMMER_CURLS: {
      filter_output = filter_first_reading ? Oaccel.z : alpha * filter_output + (1 - alpha) * Oaccel.z;
      filter_first_reading = false;
      float abs_filter_output = filter_output > 0 ? filter_output : -filter_output; //to get rid of orientation issues
      // use forearm axis
      if ((abs_filter_output <= hammer_curl_error && abs_filter_output >= 0.0) || 
        ((abs_filter_output >= -hammer_curl_error) && abs_filter_output<= 0.0 )) 
      {
        if (current_position == FLOATING) {
          current_position = UP;
        } else if (current_position == DOWN) {
          ++half_rep_count;
          current_position = UP;
        }
      } else if ((abs_filter_output <= hammer_curl_error + g && abs_filter_output >= g) || 
        ((abs_filter_output >= -hammer_curl_error + g) && abs_filter_output <= g )) 
      {
        if (current_position == FLOATING) {
          current_position = DOWN;
        } else if (current_position == UP) {
          ++half_rep_count;
          current_position = DOWN;
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
