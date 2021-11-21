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

// FastLED setup
#define NUM_LEDS 6
#define DATA_PIN 4
CRGB leds[NUM_LEDS];

int redVal = 0;
int greenVal = 0;
int blueVal = 0;

// Flag to note when the button has been pressed.
volatile bool buttonPress = 0;

// THe LEDState array contains the on/off state for the LEDs, after the LFSR algorithm has run to determine their status.
// True and False correspond to On and Off, respectively.
bool LEDState[NUM_LEDS];
long stateCompare = 0x00000001;

long currentState = 0x000001AD; //must be non-zero for lfsr cycle to work
long tempState = 0x0;           //for validating a working state

// colorList[numColors, color1, color2, color3, color4, color5, color6, color7]
// Each complete color needs 24 bits, longs required, use hex to more easily work with them.
// start color list at Christmas theme
long colorList[8] = {3, 0x00FF0000, 0x0000FF00, 0x00DCDCDC, 0x0, 0x0, 0x0, 0x0};
int colorTheme = 13;   //color theme of the colors
int colorMode = 0;     //color mode for animations (currently unused)
int colorPick = 0;

int minLED = 5;        //minimum number of LEDs that must be lit

// The lastColorState array contains the last color state of the LEDs.  Similar
//  to the targetColorState, though it is trivial when the LEDs are simply
//  blinking to a new color, it will be important to know the starting state
//  the when executing a smooth gradient between colors. Defaults to all off.
unsigned long int lastColorState[NUM_LEDS];

// The currentColorState array contains the current or soon to be displayed
//  colors of the LEDs. It will be more useful when gradients are executed.
unsigned long int currentColorState[NUM_LEDS];

// The targetColorState array contains "packedColor" 32 bit integers, which
//  contain the RGB values for each of the six LEDs. It is set when new colors
//  are selected at random to populate the LEDs. Although it is trivial when
//  the LEDs are simply blinking to a new color, it will be important to know
//  the end result wanted when executing a smooth gradient between colors.
unsigned long int targetColorState[NUM_LEDS];

const byte intPin = 2;

//ADC config
const int ADCPin = A1;
int ADCValue = 0;
int voltageMin = 1000;
int voltageMax = 0;
int voltageAvg = 0;

int DisplayInterrupt = 0;


void setup() {

  delay(500);  
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  delay(500);
  FastLED.setBrightness(25);

  redVal = 255;
  greenVal = 0;
  blueVal = 0;

  attachInterrupt(digitalPinToInterrupt(intPin), colorGrpISR, FALLING);

  Serial.begin(9600);

}


// advances RGB colors in predetermined color wheel, with intermediate colors
// Red, Orange, Yellow, Green, Blue, Indigo, Violet, Restart
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

}//end wheel

// Interrupt Service Routine to advance to the next color group
void colorGrpISR(){
  
  //quick debounce delay
  delayMicroseconds(5000);
  
  buttonPress = 1;

}//end colorGrpISR

// Advance to the next color theme
void nextTheme(){
  
  // Reset the color list so the theme selection doesn't have to do it
  for (int i = 1; i < 8; i++){
    colorList[i] = 0x0;
  }

  // Advance to the next color theme, and check bounds
  colorTheme++;
  if (colorTheme > 15) colorTheme = 0;
  
  // Set new colors
  // https://www.wikiwand.com/en/Web_colors

  switch (colorTheme){
    case 0:
      //New Years'
      colorList[0] = 0x00000003;
      colorList[1] = 0x00800080;  //Purple
      colorList[2] = 0x00FFD700;  //Gold
      colorList[3] = 0x00C0C0C0;  //Silver
      break;
    case 1:
      //Winter
      colorList[0] = 0x00000002;
      colorList[1] = 0x0087CEEB;  //SkyBlue
      colorList[2] = 0x00FFFFFF;  //White
      break;
    case 2:
      //Valentine's
      colorList[0] = 0x00000004;
      colorList[1] = 0x00FF0000;  //Red
      colorList[2] = 0x00FF69B4;  //HotPink
      colorList[3] = 0x00C0C0C0;  //Silver
      colorList[4] = 0x00FF00FF;  //Magenta
      break;
    case 3:
      //St. Patrick's
      colorList[0] = 0x00000004;
      colorList[1] = 0x0000FF00;  //Green
      colorList[2] = 0x00006400;  //DarkGreen
      colorList[3] = 0x00D3D3D3;  //LightGray
      colorList[4] = 0x00FFD700;  //Gold
      break;
    case 4:
      //Easter
      colorList[0] = 0x00000004;
      colorList[1] = 0x00FFB6C1;  //LightPink
      colorList[2] = 0x00FFFFE0;  //LightYellow
      colorList[3] = 0x0090EE90;  //LightGreen
      colorList[4] = 0x00AFEEEE;  //PaleTurquoise
      break;
    case 5:
      //Spring
      colorList[0] = 0x00000003;
      colorList[1] = 0x00FFC0CB;  //Pink
      colorList[2] = 0x00FFFF00;  //Yellow
      colorList[3] = 0x0000FF7F;  //SpringGreen
      break;
    case 6:
      //USA
      colorList[0] = 0x00000003;
      colorList[1] = 0x00FF0000;  //Red
      colorList[2] = 0x00DCDCDC;  //Gainsboro
      colorList[3] = 0x000000FF;  //Blue
      break;
    case 7:
      //Summer
      colorList[0] = 0x00000003;
      colorList[1] = 0x007CFC00;  //LawnGreen
      colorList[2] = 0x00228B22;  //ForestGreen
      colorList[3] = 0x00FAFAD2;  //LightGoldenRodYellow
      break;
    case 8:
      //Fall
      colorList[0] = 0x00000004;
      colorList[1] = 0x00A52A2A;  //Brown
      colorList[2] = 0x00FF7F50;  //Coral
      colorList[3] = 0x00E9967A;  //DarkSalmon
      colorList[4] = 0x00FFE4B5;  //Moccasin
      break;
    case 9:
      //Halloween
      colorList[0] = 0x00000004;
      colorList[1] = 0x00FF8C00;  //DarkOrange
      colorList[2] = 0x0032CD32;  //LimeGreen
      colorList[3] = 0x00800080;  //Purple
      colorList[4] = 0x00A9A9A9;  //DarkGray
      break;
    case 10:
      //Thanksgiving
      colorList[0] = 0x00000003;
      colorList[1] = 0x008B4513;  //SaddleBrown
      colorList[2] = 0x00FFA500;  //Orange
      colorList[3] = 0x00EEE8AA;  //PaleGoldenrod
      break;
    case 11:
      //Christmas
      colorList[0] = 0x00000003;
      colorList[1] = 0x00FF0000;  //Red
      colorList[2] = 0x0000FF00;  //Lime
      colorList[3] = 0x00DCDCDC;  //Gainsboro
      break;
    case 12:
      //Rainbow
      colorList[0] = 0x00000007;
      colorList[1] = 0x00FF0000;  //Red
      colorList[2] = 0x00FFA500;  //Orange
      colorList[3] = 0x00FFFF00;  //Yellow
      colorList[4] = 0x0000FF00;  //Lime
      colorList[5] = 0x000000FF;  //Blue
      colorList[6] = 0x004B0082;  //Indigo
      colorList[7] = 0x00FF00FF;  //Magenta
      break;
    case 13:
      //White Lantern
      colorList[0] = 0x00000001;
      colorList[1] = 0x00FAFAFA;  //Snow
      break;
    case 14:
      //Red Lantern
      colorList[0] = 0x00000001;
      colorList[1] = 0x00FF0000;  //Red
      break;
    case 15:
      //Orange Blink
      colorList[0] = 0x00000001;
      colorList[1] = 0x00FFA500;  //Orange
      break;
  }

  // Erase, then set the new colors to the LEDs so the user can see them
  for (int j = 0; j < NUM_LEDS; j++){
    leds[j] = CRGB::Black;
    //also reset color states
    lastColorState[j] = 0x0;
    currentColorState[j] = 0x0;
    targetColorState[j] = 0x0;
    //reset LEDState
    LEDState[j] = 0;
  }
  for (int led = 0; led < colorList[0]; led++){

    //Set new colors
    redVal = getRed(colorList[led + 1]);
    greenVal = getGreen(colorList[led + 1]);
    blueVal = getBlue(colorList[led + 1]);
    leds[led] = CRGB(redVal, greenVal, blueVal);

    //Set color states
    lastColorState[led] = colorList[led + 1];
    currentColorState[led] = colorList[led + 1];
    targetColorState[led] = colorList[led + 1];

    //set LEDState
    LEDState[led] = 1;

    if (led == NUM_LEDS) break;
  }
  FastLED.show();
  
  delay(100);

  //reset interrupt flag
  buttonPress = 0;
    
}//end nextTheme

// Extracts and returns the red component of a packed color
int getRed(long packedColor){
  long red = packedColor & 0x00FF0000;
  red = red >> 16;
  return (int)red;
}

// Extracts and returns the green component of a packed color
int getGreen(long packedColor){
  long green = packedColor & 0x0000FF00;
  green = green >> 8;
  return (int)green;
}

// Extracts and returns the blue component of a packed color
int getBlue(long packedColor){
  long blue = packedColor & 0x000000FF;
  return (int)blue;
}


void loop() {

//  //Rainbow cycle
//  for (int i = 0; i < NUM_LEDS; i++){
//    leds[i] = CRGB(redVal, greenVal, blueVal);
//    
//    FastLED.show();
//    wheel(redVal, greenVal, blueVal);
//
//    leds[i] = CRGB::Black;
//    delay(60);
//  }  

//  for (int i = NUM_LEDS - 2; i > 0; i--){
//    leds[i] = CRGB(redVal, greenVal, blueVal);
//    
//    FastLED.show();
//    wheel(redVal, greenVal, blueVal);
//    
//    leds[i] = CRGB::Black;
//    delay(60);
//  }

//  // Voltage reading
//  for (int i = 0; i < NUM_LEDS; i++){
//    if ( potVoltage >= ((i+1)*145)){
//      leds[i] = CRGB(255, 0, 0);
//    }
//    else{
//      leds[i] = CRGB::Black;
//    }
//  }

//  // Show color group colors
//  for (int i = 0; i < NUM_LEDS; i++){
//    //get colors
//    redVal = getRed(colArray[i+2]);
//    greenVal = getGreen(colArray[i+2]);
//    blueVal = getBlue(colArray[i+2]);
//    leds[i] = CRGB(redVal, greenVal, blueVal);
//  }

//  // Show color group colors
//  for (int i = 0; i < NUM_LEDS; i++){
//    //get colors
//    redVal = getRed(colorList[i + 1]);
//    greenVal = getGreen(colorList[i + 1]);
//    blueVal = getBlue(colorList[i + 1]);
//    leds[i] = CRGB(redVal, greenVal, blueVal);
//  }

  //get voltage readings
  ADCValue = analogRead(ADCPin);
//  if (voltageMin > ADCValue){
//    voltageMin = ADCValue;
//  }
//  if (voltageMax < ADCValue){
//    voltageMax = ADCValue;
//  }
//  voltageAvg = (float)voltageAvg*((float)DisplayInterrupt - 1.0)/(float)DisplayInterrupt + (float)ADCValue*(1.0/(float)DisplayInterrupt);

  DisplayInterrupt++;
  delay(40);

  if (DisplayInterrupt >= 128){
    //output data to serial
//    Serial.print("ADC Min/Max Avg: ");
//    Serial.print(voltageMin);
//    Serial.print("/");
//    Serial.print(voltageMax);
//    Serial.print(" ");
//    Serial.print(voltageAvg);
//    Serial.println(" ");
    Serial.print("Color Theme/Qty  ");
    Serial.print(colorTheme);
    Serial.print("/");
    Serial.println(colorList[0]);
    Serial.print("LED State:       ");
    
    for (int i = 0; i < NUM_LEDS; i++){
      Serial.print(LEDState[i]);
      Serial.print(" ");
    }
    Serial.println(" ");
    Serial.print("currentState:    ");
    Serial.print(currentState, HEX);
    Serial.print(" ");
    Serial.println(currentState, BIN);
    Serial.println(" ");
    DisplayInterrupt = 1;
    voltageMin = 1023;
    voltageMax = 0;
  }


  //Check for button pressed to advance to next theme
  if (buttonPress) nextTheme();

  // Set LEDState array values from state
  for (int i = 0; i < NUM_LEDS; i++){
    LEDState[i] = ((currentState & stateCompare) == stateCompare);
    stateCompare = stateCompare << 1;
  }
  
  //reset stateCompare
  stateCompare = 0x00000001; 

  
  




  FastLED.show();
  

}
