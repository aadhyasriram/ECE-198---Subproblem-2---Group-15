// Reading IR data
// Libraries
#include <DFRobot_MAX30102.h>
#include <Wire.h>
# include <algorithm>
// Create an object from the DFRobot_MAX30102 class
DFRobot_MAX30102 particleSensor; // Allows us to call methods (functions) from the class
// Put your setup code here, to run once:
int32_t SPO2;
int32_t heartRate;
int8_t validSPO2;
int8_t validHeartRate;
int HR_c=0; // calibration variable for heart rate

// void Max30102_init();//call in setup()
// void Max30102_readIR();//call to read the infrared value too technical, prob won't use it
// int Max30102_readHR();//call to read the heart rate
// int Max30102_filteredHR(int prevHR);//call to read the filtered heart rate

// int restingHR();//call to get the resting heart rate

void Max30102_init() {
    // Serial.begin(115200); // Initialize Serial Monitor at 115200 baud
    // Wire.begin(); // Enable I2C (SDA: A4, SCL: A5)
    // If sensor fails to initialize, print an error message
    while (!particleSensor.begin()) { //
    Serial.println("MAX30102 not found!");
    delay(1000); // 2 sec delay before printing the message again
    }
    // Configuration: (Led Brightness (0-255), Sample Average, LED Mode (both IR and Red),
    // Sample Rate (Hz), Pulse Width (LED pulse width), ADC Range)
    particleSensor.sensorConfiguration(60, SAMPLEAVG_8, MODE_MULTILED,
    SAMPLERATE_400, PULSEWIDTH_411,
    ADCRANGE_16384);
}
// Put your main code here, to run repeatedly:
void Max30102_readIR() {
    int IRvalue = particleSensor.getIR();
    Serial.println(IRvalue); // Print the IR values to Serial Monitor
}
int Max30102_readHR(){
    particleSensor.heartrateAndOxygenSaturation(&SPO2, &validSPO2, &heartRate, &validHeartRate);
    heartRate = heartRate + HR_c; // calibration for heart rate
    return heartRate;
}
int Max30102_filteredHR(int prevHR){
    int chr = Max30102_readHR();
    if (chr == -999) {
        return prevHR; // Return previous heart rate if current reading is invalid
    }
    return 0.3*prevHR + 0.7*chr; // Apply a simple low-pass filter to smooth the heart rate readings
}

int restingHR() {
    int arr[4];
    int count = 0;

    // Collect 4 valid readings
    while (count < 4) {
        int reading = Max30102_readHR();

        if (reading != -1) {
            arr[count] = reading;
            count++;
        }

        delay(500);
    }

    std::sort(arr, arr + 4);

    // Median of 10 values = average of the two middle values
    return (arr[1] + arr[2]) / 2;
}
