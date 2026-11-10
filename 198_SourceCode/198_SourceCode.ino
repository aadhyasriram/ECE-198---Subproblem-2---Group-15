#include "buttons.h"

void setup() {
    initializeButtons();
}

void loop() {
    checkPowerButton();
    checkNextButton();
}