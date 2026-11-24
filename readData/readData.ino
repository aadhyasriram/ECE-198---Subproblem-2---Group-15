// to be run by healthcare providers
#include <EEPROM.h>
int currAddr = 0;
int lowestHRread{};
int highestHRread{};
int avgHRread{};
int repsPerExerciseRead[9]{};
int errorsPerExerciseRead[9]{};
int restingHRRead{};
void setup()  {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial) {;}
  delay(500);
  // gets data from arduino
  EEPROM.get(currAddr,lowestHRread);
  currAddr += sizeof(int);
  EEPROM.get(currAddr, highestHRread);
  currAddr += sizeof(int);
  EEPROM.get(currAddr,avgHRread);
  for (int i = 0; i < 9; i++) {
    currAddr += sizeof(int);
    EEPROM.get(currAddr,repsPerExerciseRead[i]);
  }
  currAddr += sizeof(int);
  EEPROM.get(currAddr,restingHRRead);
  for (int i = 0; i < 9; i++) {
    currAddr += sizeof(int);
    EEPROM.get(currAddr, errorsPerExerciseRead[i]);
  }
  //printing
  Serial.print("Lowest HR: ");
  Serial.println(lowestHRread);
  Serial.print("Highest HR: ");
  Serial.println(highestHRread);
  Serial.print("Avg HR: ");
  Serial.println(avgHRread);
  Serial.print(" Hammer curl ");
  Serial.println(repsPerExerciseRead[1]);
  Serial.print("Resting HR: ");
  Serial.println(restingHRRead);
  Serial.print("Number of errors for hammer curl: ");
  Serial.println(errorsPerExerciseRead[1]);
  
}

void loop() {
  
  // put your main code here, to run repeatedly:
  
}
