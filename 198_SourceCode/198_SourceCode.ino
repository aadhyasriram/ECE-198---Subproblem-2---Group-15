#include "BMX160.h"

void setup() {
  // put your setup code here, to run once:
  imu_init();
}

void loop() {
  // put your main code here, to run repeatedly:
  get_sensor_data(HAMMER_CURLS);
}
