#ifndef BMX160_H
#define BMX160_H
#include <DFRobot_BMX160.h>
enum exercises{
    CARDIO,
    HAMMER_CURLS,
    TRICEP_PUSHDOWN,
    SHOULDER_PRESS,
    SEATED_AB_CRUNCH,
    LAT_PULLDOWN,
    INCLINE_BENCH_PRESS,
    DUMBBELL_SUMO_SQUAT,
    ROMANIAN_DEADLIFT
};

enum forearm_position{
    DOWN, // hanging down
    UP, // held horizontally
    FLOATING
};

void imu_init(); //initalizes imu
void get_sensor_data(exercises excr);
int rep_count(exercises excr, sBmx160SensorData_t Omagn, sBmx160SensorData_t Oaccel, sBmx160SensorData_t Ogyro);



#endif