#ifndef BMX160_H
#define BMX160_H
#include <DFRobot_BMX160.h>
enum exercises{
    CARDIO = 0,
    HAMMER_CURLS = 1,
    TRICEP_PUSHDOWN = 2,
    SHOULDER_PRESS = 3,
    SEATED_AB_CRUNCH = 4,
    LAT_PULLDOWN = 5,
    INCLINE_BENCH_PRESS = 6,
    DUMBBELL_SUMO_SQUAT = 7,
    ROMANIAN_DEADLIFT = 8
};

enum forearm_position{
    DOWN, // hanging down
    UP, // held horizontally
    FLOATING
};

void imu_init(); //initalizes imu
int get_sensor_data(exercises excr);
int rep_count(exercises excr, sBmx160SensorData_t Omagn, sBmx160SensorData_t Oaccel, sBmx160SensorData_t Ogyro);
void set_error_counter(exercises excr, sBmx160SensorData_t Omagn, sBmx160SensorData_t Oaccel, sBmx160SensorData_t Ogyro);
void set_error_counter();
int get_error_counter();
int get_error_counter_copy();

#endif