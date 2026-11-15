#include "OLED.h"
#include <Arduino.h>
#include <string.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
 
// OLED settings
static const int SCREEN_WIDTH = 128;
static const int SCREEN_HEIGHT = 64;
static const uint8_t OLED_ADDRESS = 0x3C;   // try 0x3D if the screen stays blank
static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
 
static const int OLED_HEADER_HEIGHT = 12;
 
 
// ---------- Basic helpers ----------
 
// Finds the top-left coordinates that center one line of text (len characters,
// text size 2: 12 x 16 px per character) on the screen.
coord find_coord(int len, int y_offset, int x_offset) {
  int x = (SCREEN_WIDTH - len * 12) / 2 + x_offset;
  int y = (SCREEN_HEIGHT - 16) / 2 + y_offset;
  return coord(x, y);
}
 
// Number of characters needed to print n (includes the minus sign)
int int_len(int n) {
  int len = 1;
  if (n < 0) {
    len++;
    n = -n;
  }
  while (n >= 10) {
    n /= 10;
    len++;
  }
  return len;
}
 
 
void OLED_init() {
  // If the OLED isn't found, the code keeps running instead of freezing
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);
  display.clearDisplay();
  display.display();
}
 
 
// ---------- Text layout helpers ----------
 
// Splits text into lines at spaces so no line is longer than maxChars.
// Returns the number of lines, or -1 if it doesn't fit.
static int oledWrapText(const char* t, int maxChars, int maxLines, int starts[], int lens[]) {
  int len = strlen(t);
  int n = 0;
  int i = 0;
 
  while (i < len) {
    while (i < len && t[i] == ' ') i++;
    if (i >= len) break;
    if (n >= maxLines) return -1;
 
    int start = i;
    int end = i;
 
    while (i < len) {
      int wordStart = i;
      while (i < len && t[i] != ' ') i++;
      int wordEnd = i;
 
      if (wordEnd - wordStart > maxChars) return -1;
 
      if (wordEnd - start > maxChars) {
        i = wordStart;   // this word goes on the next line
        break;
      }
 
      end = wordEnd;
      while (i < len && t[i] == ' ') i++;
    }
 
    starts[n] = start;
    lens[n] = end - start;
    n++;
  }
 
  return n;
}
 
 
// Draws text centered in an area, using the largest size that fits
// (size 3, then 2, then 1) and wrapping at spaces.
static void oledDrawCentered(const char* text, int areaTop, int areaHeight) {
  int starts[6];
  int lens[6];
 
  display.setTextColor(SSD1306_WHITE);
 
  for (int size = 3; size >= 1; size--) {
    int maxChars = SCREEN_WIDTH / (6 * size);
    int maxLines = areaHeight / (8 * size);
    if (maxLines > 6) maxLines = 6;
 
    int n = oledWrapText(text, maxChars, maxLines, starts, lens);
 
    if (n > 0) {
      display.setTextSize(size);
      int blockHeight = n * 8 * size;
      int y = areaTop + (areaHeight - blockHeight) / 2;
 
      for (int line = 0; line < n; line++) {
        int x = (SCREEN_WIDTH - lens[line] * 6 * size) / 2;
        display.setCursor(x, y + line * 8 * size);
        for (int k = 0; k < lens[line]; k++) {
          display.write(text[starts[line] + k]);
        }
      }
      return;
    }
  }
 
  // Fallback for text that can't be wrapped to fit
  display.setTextSize(1);
  display.setCursor(0, areaTop);
  display.print(text);
}
 
 
// ---------- Rep / heart rate / warning screens ----------
 
void OLED_display_rep(int rep) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
 
  coord c = find_coord(5 + int_len(rep));   // "Rep: " + number
  display.setCursor(c.getX(), c.getY());
  display.print(F("Rep: "));
  display.print(rep);
 
  display.display();
}
 
 
void OLED_display_HR(int minHR, int maxHR, int actualHR) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
 
  // Target range, small text near the top
  display.setTextSize(1);
  int rangeLen = 7 + int_len(minHR) + 1 + int_len(maxHR);   // "Target " + min + "-" + max
  display.setCursor((SCREEN_WIDTH - rangeLen * 6) / 2, 8);
  display.print(F("Target "));
  display.print(minHR);
  display.print('-');
  display.print(maxHR);
 
  // Actual heart rate, larger and centered
  display.setTextSize(2);
  coord c = find_coord(4 + int_len(actualHR), 10);   // "HR: " + number
  display.setCursor(c.getX(), c.getY());
  display.print(F("HR: "));
  display.print(actualHR);
 
  display.display();
}
 
 
void OLED_warning(const char* problem) {
  display.clearDisplay();
 
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(F("WARNING"));
 
  oledDrawCentered(problem, OLED_HEADER_HEIGHT, SCREEN_HEIGHT - OLED_HEADER_HEIGHT);
 
  display.display();
}
 
 
// ---------- Screens used by buttons.cpp ----------
 
// Small header on the top row, main text centered in the rest of the screen
void OLED_display_message(String header, const char* body) {
  display.clearDisplay();
 
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(header);
 
  oledDrawCentered(body, OLED_HEADER_HEIGHT, SCREEN_HEIGHT - OLED_HEADER_HEIGHT);
 
  display.display();
}
 
 
// "Exercise N of TOTAL" header with the exercise name auto-sized and centered
void OLED_display_exercise(int number, int total, const char* name) {
  display.clearDisplay();
 
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(F("Exercise "));
  display.print(number);
  display.print(F(" of "));
  display.print(total);
 
  oledDrawCentered(name, OLED_HEADER_HEIGHT, SCREEN_HEIGHT - OLED_HEADER_HEIGHT);
 
  display.display();
}
 
 
// Header, "Cardio" label, and a large centered MM:SS countdown
void OLED_display_cardio(int number, int total, int minutes, int seconds) {
  char timeText[6];
  timeText[0] = '0' + (minutes / 10);
  timeText[1] = '0' + (minutes % 10);
  timeText[2] = ':';
  timeText[3] = '0' + (seconds / 10);
  timeText[4] = '0' + (seconds % 10);
  timeText[5] = '\0';
 
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
 
  // Header
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(F("Exercise "));
  display.print(number);
  display.print(F(" of "));
  display.print(total);
 
  // "Cardio" label, size 2 (6 chars * 12 px = 72 px wide)
  display.setTextSize(2);
  display.setCursor((SCREEN_WIDTH - 72) / 2, 11);
  display.print(F("Cardio"));
 
  // Timer, size 4 (5 chars * 24 px = 120 px wide)
  display.setTextSize(4);
  display.setCursor((SCREEN_WIDTH - 120) / 2, 30);
  display.print(timeText);
 
  display.display();
}