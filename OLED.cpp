#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Screen Variables
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
int potPin = A2; // Potentiometer connected to analog pin A0
int adcValue = 0; 
void OLED_init() {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
}
void loop() {
delay(1000);
// Must clear the display before putting anything on it
display.clearDisplay();
display.setTextSize(1);
display.setCursor(5, 0);
adcValue = analogRead(potPin); // Read analog voltage (0–1023)
display.print("Current Reading: ");
Serial.println(adcValue); // Print the value to Serial Monitor
display.display();
}

