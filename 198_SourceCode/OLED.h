#ifndef OLED_H
#define OLED_H
 
#include <Arduino.h>
 
class coord{   
  public:
    int x;
    int y;
    coord(int x, int y) {
      this->x = x;
      this->y = y;
    }
    int getX() {
      return x;
    }
    int getY() {
      return y;
    }
};
 
coord find_coord(int len, int y_offset=0, int x_offset=0); //Finds the coordinates to center text on the screen
int int_len(int n);
void OLED_init(); //Call in setup() to initialize the OLED display
void OLED_display_rep(int rep); //Call each time rep changes
void OLED_display_HR(int minHR, int maxHR, int actualHR); //Call to show the HR range and actual HR
void OLED_warning(const char* problem); //Call to display a warning message
 
// --- New functions used by buttons.cpp ---
void OLED_display_message(const __FlashStringHelper* header, const char* body); //Small header on top, body text centered and auto-sized
void OLED_display_exercise(int number, int total, const char* name); //Shows "Exercise N of TOTAL" and the exercise name
void OLED_display_cardio(int number, int total, int minutes, int seconds); //Shows the cardio label and a large MM:SS countdown
 
 
#endif