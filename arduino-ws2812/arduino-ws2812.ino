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

// FastLED defines
#define NUM_LEDS 6
#define DATA_PIN 4

//other defines
#define GRAD_STEPS 255

//the led strip
CRGB leds[NUM_LEDS];



int redVal = 0;
int greenVal = 0;
int blueVal = 0;

int tempRval = 0;
int tempGval = 0;
int tempBval = 0;

int targetRval = 0;
int targetGval = 0;
int targetBval = 0;

// Flag to note when the button has been pressed.
volatile bool buttonPress = 0;

// THe LEDState array contains the on/off state for the LEDs, after the LFSR algorithm has run to determine their status.
// True and False correspond to On and Off, respectively.
bool LEDState[NUM_LEDS];
long stateCompare = 0x00000001;

long currentState = 0x000001BF; //must be non-zero for lfsr cycle to work
long tempState = 0x0;           //for validating a working state

// colorList[numColors, color1, color2, color3, color4, color5, color6, color7]
// Each complete color needs 24 bits, longs required, use hex to more easily work with them.
// start color list at Christmas theme
long colorList[8] = {3, 0x00FF0000, 0x0000FF00, 0x00DCDCDC, 0x0, 0x0, 0x0, 0x0};
int colorTheme = 13;          //color theme of the colors
int colorMode = 0;            //color mode for animations (currently unused)
int colorPick = 0;
int minLED = (NUM_LEDS*2)/3;  //minimum number of LEDs active for a theme
int LEDtotal = 0;
long LEDmask = 0x0;

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
//int voltageMin = 1000;
//int voltageMax = 0;
//int voltageAvg = 0;

//int DisplayInterrupt = 0;


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

  //needs to read in once 
  ADCValue = analogRead(ADCPin);

  //create an LED mask
  stateCompare = 0x00000001;  //reset stateCompare before use
  for (int i = 0; i < NUM_LEDS; i++){
    LEDmask = LEDmask | stateCompare;
    stateCompare = stateCompare << 1;
  }

  //initialize color states
  for (int i = 0; i < NUM_LEDS; i++){
    currentColorState[i] = 0x0;
    lastColorState[i] = 0x0;
    targetColorState[i] = 0x0;
  }

  Serial.print("LEDmask: ");
  Serial.println(LEDmask, HEX);
  Serial.println(" ");

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
  delayMicroseconds(50000);
  
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
      minLED = (NUM_LEDS*2)/3;
      break;
    case 1:
      //Winter
      colorList[0] = 0x00000002;
      colorList[1] = 0x0087CEEB;  //SkyBlue
      colorList[2] = 0x00FFFFFF;  //White
      minLED = (NUM_LEDS*2)/3;
      break;
    case 2:
      //Valentine's
      colorList[0] = 0x00000004;
      colorList[1] = 0x00FF0000;  //Red
      colorList[2] = 0x00FF69B4;  //HotPink
      colorList[3] = 0x00C0C0C0;  //Silver
      colorList[4] = 0x00FF00FF;  //Magenta
      minLED = (NUM_LEDS*2)/3;
      break;
    case 3:
      //St. Patrick's
      colorList[0] = 0x00000004;
      colorList[1] = 0x0000FF00;  //Green
      colorList[2] = 0x00006400;  //DarkGreen
      colorList[3] = 0x00D3D3D3;  //LightGray
      colorList[4] = 0x00FFD700;  //Gold
      minLED = (NUM_LEDS*2)/3;
      break;
    case 4:
      //Easter
      colorList[0] = 0x00000004;
      colorList[1] = 0x00FFB6C1;  //LightPink
      colorList[2] = 0x00FFFFE0;  //LightYellow
      colorList[3] = 0x0090EE90;  //LightGreen
      colorList[4] = 0x00AFEEEE;  //PaleTurquoise
      minLED = (NUM_LEDS*2)/3;
      break;
    case 5:
      //Spring
      colorList[0] = 0x00000003;
      colorList[1] = 0x00FFC0CB;  //Pink
      colorList[2] = 0x00FFFF00;  //Yellow
      colorList[3] = 0x0000FF7F;  //SpringGreen
      minLED = (NUM_LEDS*2)/3;
      break;
    case 6:
      //USA
      colorList[0] = 0x00000003;
      colorList[1] = 0x00FF0000;  //Red
      colorList[2] = 0x00DCDCDC;  //Gainsboro
      colorList[3] = 0x000000FF;  //Blue
      minLED = (NUM_LEDS*2)/3;
      break;
    case 7:
      //Summer
      colorList[0] = 0x00000003;
      colorList[1] = 0x007CFC00;  //LawnGreen
      colorList[2] = 0x00228B22;  //ForestGreen
      colorList[3] = 0x00FAFAD2;  //LightGoldenRodYellow
      minLED = (NUM_LEDS*2)/3;
      break;
    case 8:
      //Fall
      colorList[0] = 0x00000004;
      colorList[1] = 0x00A52A2A;  //Brown
      colorList[2] = 0x00FF7F50;  //Coral
      colorList[3] = 0x00E9967A;  //DarkSalmon
      colorList[4] = 0x00FFE4B5;  //Moccasin
      minLED = (NUM_LEDS*2)/3;
      break;
    case 9:
      //Halloween
      colorList[0] = 0x00000004;
      colorList[1] = 0x00FF8C00;  //DarkOrange
      colorList[2] = 0x0032CD32;  //LimeGreen
      colorList[3] = 0x00800080;  //Purple
      colorList[4] = 0x00A9A9A9;  //DarkGray
      minLED = (NUM_LEDS*2)/3;
      break;
    case 10:
      //Thanksgiving
      colorList[0] = 0x00000003;
      colorList[1] = 0x008B4513;  //SaddleBrown
      colorList[2] = 0x00FFA500;  //Orange
      colorList[3] = 0x00EEE8AA;  //PaleGoldenrod
      minLED = (NUM_LEDS*2)/3;
      break;
    case 11:
      //Christmas
      colorList[0] = 0x00000003;
      colorList[1] = 0x00FF0000;  //Red
      colorList[2] = 0x0000FF00;  //Lime
      colorList[3] = 0x00DCDCDC;  //Gainsboro
      minLED = (NUM_LEDS*2)/3;
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
      minLED = (NUM_LEDS*2)/3;
      break;
    case 13:
      //White Lantern
      colorList[0] = 0x00000002;
      colorList[1] = 0x00FAFAFA;  //Snow
      colorList[2] = 0x00FAFAFA;  //Snow
      minLED = NUM_LEDS;
      break;
    case 14:
      //Red Lantern
      colorList[0] = 0x00000002;
      colorList[1] = 0x00FF0000;  //Red
      colorList[2] = 0x00FF0000;  //Red
      minLED = NUM_LEDS;
      break;
    case 15:
      //Orange Blink
      colorList[0] = 0x00000002;
      colorList[1] = 0x00FF4500;  //OrangeRed
      colorList[2] = 0x00FF4500;  //OrangeRed
      minLED = NUM_LEDS;
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
  
  delay(1000);

  //reset interrupt flag
  buttonPress = 0;
    
}//end nextTheme

// lfsr function
// This function takes a 16 bit number and runs a series of linear feedback shift
//  operations on it. The next state it finds is guaranteed to be different than
//  the start state it was given.
// Arguments: 32 bit integer representing an input start state
// Returns:   32 bit integer representing the next state
long lfsr(long start){

    long state = start;  // Initially set the current state to the start state

  //example set of shift operations
  //state ^= state >> 7;  // 0b10110001 XOR with 0b00000001
  //state ^= state << 3;  // 0b10110000 XOR with 0b10000000
  //state ^= state >> 2;  // 0b00110000 XOR with 0b00001100
              // end result state = 0b00111100

    // These shift operations result in 65535 states before returning to the start state.

  state ^= state >> 7;
  state ^= state << 9;
  state ^= state >> 13;

  return state; // Return the next state found

}//end lfsr function

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

// Returns a packColor value from RGB components
long packColor(int red, int green, int blue){
  long RGBval = 0;
  RGBval = ((long) red << 16 | ((long) green << 8) | (long) blue);
  return RGBval;
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

  //Check for button pressed to advance to next theme
  if (buttonPress) nextTheme();
  
  //reset stateCompare
  stateCompare = 0x00000001;
  // Set LEDState array values from state
  for (int i = 0; i < NUM_LEDS; i++){
    LEDState[i] = ((currentState & stateCompare) == stateCompare);
    stateCompare = stateCompare << 1;
  }
  Serial.print("LEDstate: ");
  Serial.print(LEDState[0]);
  Serial.print(LEDState[1]);
  Serial.print(LEDState[2]);
  Serial.print(LEDState[3]);
  Serial.print(LEDState[4]);
  Serial.println(LEDState[5]);

  //set targetColorState array from lastColorState
  for (int i = 0; i < NUM_LEDS; i++){

    if (LEDState[i]){   //LED is on, needs to select a color
      colorPick = 0;
      //pick random color
      while (colorPick == 0){

        //get random value from ADC
        ADCValue = analogRead(ADCPin);

        ADCValue = (ADCValue & 0x07);

        if ((ADCValue > colorList[0]) || (ADCValue == 0)){
          continue; //ADC value out of bounds, pick new number
        }

        //use this (hopefully) random number to select a random color
        targetColorState[i] = colorList[ADCValue];
        colorPick = 1;  //color selected, exit
        
      }//end colorPick while
    }//end LEDState if
    else{
      targetColorState[i] = 0x0;  //LED off
    }
  }//end targetColorState for loop


//  //BLINKY Code
//  //Simply blinks to the new LED state and colors
//  for (int k = 0; k < NUM_LEDS; k++){
//    currentColorState[k] = targetColorState[k];
//  }
//
//  //set the colors in the led array
//  for (int led = 0; led < NUM_LEDS; led++){
//    redVal = getRed(currentColorState[led]);
//    greenVal = getGreen(currentColorState[led]);
//    blueVal = getBlue(currentColorState[led]);
//    leds[led] = CRGB(redVal, greenVal, blueVal);
//  }

  //GRADIENT Code
  // Moves in a smooth gradient from the current color state to the target color state

  //set the current color to what the last color was
  for (int j = 0; j < NUM_LEDS; j++){
    currentColorState[j] = lastColorState[j];
  }

  //the gradient is carried out over GRAD_STEPS number of steps
  for (int g = 0; g < GRAD_STEPS; g++){

    //within each gradient step, set up each color step
    for (int v = 0; v < NUM_LEDS; v++){
      tempRval = getRed(currentColorState[v]);
      tempGval = getGreen(currentColorState[v]);
      tempBval = getBlue(currentColorState[v]);

      targetRval = getRed(targetColorState[v]);
      targetGval = getGreen(targetColorState[v]);
      targetBval = getBlue(targetColorState[v]);

      //compare Red values
      if (tempRval > targetRval){
        tempRval--;
      }
      else if (tempRval < targetRval){
        tempRval++;
      }

      //compare Green values
      if (tempGval > targetGval){
        tempGval--;
      }
      else if (tempGval < targetGval){
        tempGval++;
      }

      //compare Blue values
      if (tempBval > targetBval){
        tempBval--;
      }
      else if (tempBval < targetBval){
        tempBval++;
      }

      //repack the color
      currentColorState[v] = packColor(tempRval, tempGval, tempBval);
      
    }//end color setup for loop

    //write the colors
    for (int led = 0; led < NUM_LEDS; led++){
      redVal = getRed(currentColorState[led]);
      greenVal = getGreen(currentColorState[led]);
      blueVal = getBlue(currentColorState[led]);
      leds[led] = CRGB(redVal, greenVal, blueVal);
    }
    FastLED.show();
    //delay(4);
  }

  //LED State Code
  // Create a new LED state combination
  
  //reset the number of active LEDs
  LEDtotal = 0;
  Serial.print("minLED: ");
  Serial.println(minLED);
  
  //need some minimum number LEDs active for the theme
  while (LEDtotal < minLED){

    LEDtotal = 0;

    if (minLED == NUM_LEDS){
      currentState = LEDmask;  //all LEDs on, just set the state
    }
    else{
      currentState = lfsr(currentState);  //get new state
    }

    //reset stateCompare
    stateCompare = 0x00000001;
    //total up the sum of digits, representing the number of active LEDs
    for (int led = 0; led < NUM_LEDS; led++){
      if ((currentState & stateCompare) == stateCompare) LEDtotal++;
      stateCompare = stateCompare << 1;
    }
    Serial.print("LEDtotal: ");
    Serial.println(LEDtotal);

    if (minLED == NUM_LEDS);  //all LEDs on, don't worry about duplicates
    //if a next duplicate state is encountered when using a the LED mask, try again
    else if ((currentState & LEDmask) == (tempState & LEDmask)){
      LEDtotal = 0;
      Serial.print("duplicate: ");
      Serial.print((currentState & LEDmask), HEX);
      Serial.print(" ");
      Serial.println((tempState & LEDmask), HEX);
      Serial.println(" ");
    }
    
  }//end LFSR while loop

  //found a good state, save it in case it gets duplicated in the next one
  Serial.print("currentState: ");
  Serial.print(currentState, HEX);
  Serial.print(" ");
  Serial.println(currentState, BIN);
  Serial.println(" ");
  tempState = currentState;

  //save a new lastColorState and update currentColorState
  for (int j = 0; j < NUM_LEDS; j++){
    lastColorState[j] = targetColorState[j];
    currentColorState[j] = targetColorState[j]; //possibly redundant
  }






  //FastLED.show();
  
  //three second delay 60*50
  // This could be handled with a potentiometer in the future
  for (int d = 0; d < 60; d++){
    delay(50);
    if (buttonPress) break; //exit loop if button to change theme has been pressed
  }
  


}
