#include "buttons.h"

void setup() {
    buttons_init();
    
}

void loop() {
    checkPowerButton();
    checkNextButton();
    updateTimer();
}