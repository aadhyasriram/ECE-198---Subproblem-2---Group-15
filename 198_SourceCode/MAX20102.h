#ifndef MAX30102_H
#define MAX30102_H

#include <Arduino.h>

void Max30102_init();
void Max30102_readIR();
int Max30102_readHR();
int Max30102_filteredHR(int prevHR);//call to read the filtered heart rate
int restingHR();


#endif