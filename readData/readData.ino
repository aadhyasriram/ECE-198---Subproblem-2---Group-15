// to be run by healthcare providers
#include <EEPROM.h>

const char* exerciseNames[] = {
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

int currAddr = 0;
int lowestHRread = 0;
int highestHRread = 0;
int avgHRread = 0;
int repsPerExerciseRead[9] = {};
int errorsPerExerciseRead[9] = {};
int restingHRRead = 0;

int readIntAtAddress(int address) {
  int value = 0;
  EEPROM.get(address, value);
  return value;
}

void setup()  {
  Serial.begin(9600);
  while(!Serial) {;}
  delay(500);

  currAddr = 0;
  lowestHRread = readIntAtAddress(currAddr);
  currAddr += sizeof(int);
  highestHRread = readIntAtAddress(currAddr);
  currAddr += sizeof(int);
  avgHRread = readIntAtAddress(currAddr);

  for (int i = 0; i < 9; i++) {
    currAddr += sizeof(int);
    repsPerExerciseRead[i] = readIntAtAddress(currAddr);
  }

  currAddr += sizeof(int);
  restingHRRead = readIntAtAddress(currAddr);

  for (int i = 0; i < 9; i++) {
    currAddr += sizeof(int);
    errorsPerExerciseRead[i] = readIntAtAddress(currAddr);
  }

  Serial.print("Lowest HR: ");
  Serial.println(lowestHRread);
  Serial.print("Highest HR: ");
  Serial.println(highestHRread);
  Serial.print("Avg HR: ");
  Serial.println(avgHRread);
  Serial.print("Resting HR: ");
  Serial.println(restingHRRead);

  Serial.println("Exercise Reps:");
  for (int i = 0; i < 9; i++) {
    Serial.print(exerciseNames[i]);
    Serial.print(": ");
    Serial.println(repsPerExerciseRead[i]);
  }

  Serial.println("Exercise Errors:");
  for (int i = 0; i < 9; i++) {
    Serial.print(exerciseNames[i]);
    Serial.print(": ");
    Serial.println(errorsPerExerciseRead[i]);
  }

  // JSON marker can be consumed by index.html using the Web Serial API.
  Serial.print("DATA_JSON={\"lowestHR\":");
  Serial.print(lowestHRread);
  Serial.print(",\"highestHR\":");
  Serial.print(highestHRread);
  Serial.print(",\"avgHR\":");
  Serial.print(avgHRread);
  Serial.print(",\"restingHR\":");
  Serial.print(restingHRRead);
  Serial.print(",\"reps\":[");
  for (int i = 0; i < 9; i++) {
    if (i > 0) Serial.print(",");
    Serial.print(repsPerExerciseRead[i]);
  }
  Serial.print("],\"errors\":[");
  for (int i = 0; i < 9; i++) {
    if (i > 0) Serial.print(",");
    Serial.print(errorsPerExerciseRead[i]);
  }
  Serial.println("]}");
}

void loop() {
  
  // put your main code here, to run repeatedly:
  
}
