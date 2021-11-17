/*  File name:    arduino-ws2812.ino
 *  Description:  Arduino code to control an LED strip using individual ws2812 programmable LEDs.
 *  Author:       Chris Saiko
 *  Email:        chris.saiko@gmail.com

    This file is part of holiday-led-jar.

    holiday-led-jar is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    holiday-led-jar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with holiday-led-jar.  If not, see <https://www.gnu.org/licenses/>.
    
 */

#include <FastLED.h>

// FastLED stuff
#define NUM_LEDS 6
#define DATA_PIN 4
CRGB leds[NUM_LEDS];

int redVal = 0;
int greenVal = 0;
int blueVal = 0;


void setup() {

  delay(500);  
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  delay(500);
  FastLED.setBrightness(25);

  redVal = 255;
  greenVal = 0;
  blueVal = 0;

}


// advances RGB colors in predetermined color wheel, with intermediate colors
// Red, Orange, Yellow, Green, Blue, Indigo, Violet, Restart
// 255, 0, 0    - Red
// 255, 64, 0
// 255, 128, 0  - Orange
// 255, 192, 0
// 255, 255, 0  - Yellow
// 128, 255, 0
// 0, 255, 0    - Green
// 0, 128, 128
// 0, 0, 255    - Blue
// 64, 64, 255
// 128, 128, 255  - Indigo
// 192, 64, 255
// 255, 0, 255  - Violet
// 255, 0, 128
void wheel(int &redVal, int &greenVal, int &blueVal){
    if (redVal == 255) {
      if ((greenVal == 0) && (blueVal == 0)) {
        greenVal = 64;
        return;
      }
      if ((greenVal == 64) && (blueVal == 0)) {
        greenVal = 128;
        return;
      }
      if ((greenVal == 128) && (blueVal == 0)) {
        greenVal = 192;
        return;
      }
      if ((greenVal == 192) && (blueVal == 0)) {
        greenVal = 255;
        return;
      }
      if ((greenVal == 255) && (blueVal == 0)) {
        redVal = 128;
        return;
      }
      if ((greenVal == 0) && (blueVal == 255)) {
        blueVal = 128;
        return;
      }
      if ((greenVal == 0) && (blueVal == 128)) {
        blueVal = 0;
        return;
      }
    }
    else if (blueVal == 255){
      if ((redVal == 0) && (greenVal == 0)) {
        redVal = 64;
        greenVal = 64;
        return;
      }
      if ((redVal == 64) && (greenVal == 64)) {
        redVal = 128;
        greenVal = 128;
        return;
      }
      if ((redVal == 128) && (greenVal == 128)) {
        redVal = 192;
        greenVal = 64;
        return;
      }
      if ((redVal == 192) && (greenVal == 64)) {
        redVal = 255;
        greenVal = 0;
        return;
      }
    }
    else if(greenVal == 255){
      if ((redVal == 128) && (blueVal == 0)){
        redVal = 0;
        return;
      }
      if ((redVal == 0) && (blueVal == 0)){
        greenVal = 128;
        blueVal = 128;
        return;
      }
    }
    greenVal = 0;      //base case, assume return to blue, this catches any odd color as well
    blueVal = 255;
    redVal = 0;
    return;

}

  
void loop() {

  
  for (int i = 0; i < NUM_LEDS; i++){
    leds[i] = CRGB(redVal, greenVal, blueVal);
    
    FastLED.show();
    wheel(redVal, greenVal, blueVal);

    leds[i] = CRGB::Black;
    delay(60);
  }  

  for (int i = NUM_LEDS - 2; i > 0; i--){
    leds[i] = CRGB(redVal, greenVal, blueVal);
    
    FastLED.show();
    wheel(redVal, greenVal, blueVal);
    
    leds[i] = CRGB::Black;
    delay(60);
  }

}
