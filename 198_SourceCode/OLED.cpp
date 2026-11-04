#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Screen Variables
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
void OLED_init(); //Call in setup() to initialize the OLED display
void OLED_display_rep(int rep); //Call each time rep changes
void OLED_display_HR(int minHR, int maxHR, int actualHR); //Call to show the HR range and actual HR
void OLED_warning(const char* problem); //Call to display a warning message


void OLED_init() {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
}

void OLED_display_rep(int rep) {
// Must clear the display before putting anything on it
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(5, 0);
    display.print("Reps: ");
    display.print(rep);
//    Serial.println(adcValue); // Print the value to Serial Monitor
    display.display();
}
void OLED_display_HR(int minHR, int maxHR, int actualHR) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(5, 0);
    display.print("HR: ");

    display.setCursor(0, 32);
    display.print("Your HR: ");
    display.print(actualHR);
    display.println(" BPM");

    display.setCursor(0, 48);
    display.print("Range: ");
    display.print(minHR);
    display.print("-");
    display.print(maxHR);

    display.display();
}
void OLED_warning(const char* problem) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    display.setCursor(0, 0);
    display.println("WARNING!");
    display.setCursor(0, 20);
    display.println(problem);
    display.display();
}
