#include "MAX20102.h"
#include "OLED.h"
#include "buttons.h"
int prev=100;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Max30102_init();
  OLED_init();
  buttons_init();
}

void loop() {
  // put your main code here, to run repeatedly:
//  OLED_display_rep(15);
  checkPowerButton();
  int chr= Max30102_readHR();
  int filtered_chr= Max30102_filteredHR(prev);
  checkNextButton();
  OLED_display_HR(90, 153, filtered_chr); //Call to show the HR range and actual HR
  prev= filtered_chr;
  updateTimer();

}


