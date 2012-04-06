#include "LPD8806.h"
#include "SPI.h"

// Example to control LPD8806-based RGB LED Modules in a strip
//todo:
//smooth blinking
//final one 4 strands 10 meters each, 4 ft apart, 320 lights on each strand
// 320px by 4 px. 
/*****************************************************************************/

#define NORMAL 1
#define INVERSE 0

// Choose which 2 pins you will use for output.
// Can be any valid output pins.
int dataPin = 2;   
int clockPin = 3; 
int sensor = 0;
int sensorPin= A0;
int meterVal = 0;
int color = 0;
int maxVal = 30;
int primary = 16000;
int complement = 1000;
int mode = INVERSE;
int flag = false;

// Set the first variable to the NUMBER of pixels. 32 = 32 pixels in a row
// The LED strips are 32 LEDs per meter but you can extend/cut the strip
LPD8806 strip = LPD8806(64, dataPin, clockPin);

// you can also use hardware SPI, for ultra fast writes by leaving out the
// data and clock pin arguments. This will 'fix' the pins to the following:
// on Arduino 168/328 thats data = 11, and clock = pin 13
// on Megas thats data = 51, and clock = 52 
//LPD8806 strip = LPD8806(32);

void setup() {
  // Start up the LED strip
  Serial.begin(9600);
  strip.begin();
}


void loop() {

  int filtered;
  int scaled;
  int numPixels;
  
  int i;
  sensor = analogRead(sensorPin);  //this will be between 0 and 1023
  filtered = filter(sensor);
  scaled = ((filtered - 300)/2) + 300;
  meterVal = map(scaled, 300, 500, 3, 64);
  numPixels = strip.numPixels();
  
  for (i=0; i <= numPixels; i++) {
    if (mode == NORMAL) { 
        if(i < meterVal) {
            strip.setPixelColor(i, primary);
        } else if(i == meterVal) {
            strip.setPixelColor(i, complement);
        } else {
            strip.setPixelColor(i, 0);
        }
    } else {
        if(i < meterVal) {
            strip.setPixelColor(numPixels - i, primary);
        } else if(i == meterVal) {
            strip.setPixelColor(numPixels - i, complement);
        } else {
            strip.setPixelColor(numPixels - i, 0);
        }
    }
  }
  if(meterVal > 30) {
       primary += 2000;
       complement += 2000;
  }
  
  if (mode == NORMAL){
    if (meterVal > 60){
      flag = true;
    }
    if (flag == true && meterVal < 60) {
      flag = false;  
      mode = INVERSE;
    }
  } else if( mode == INVERSE) {
    if (meterVal > 60) {
      flag = true;
    }
    if (flag == true && meterVal < 60) {
      flag = false;  
      mode = NORMAL;
    }
  }
  
//  delay(10);
  strip.show();
}

int filter(int sensor)
{
  static float peak = 0;
  static float divisor = 1;
  static int cycle = 0;
  static float last = 0;
  float current;
  float diff;
  
  cycle++;
  
  current = peak/divisor;
  
  if ((float)sensor > current) {
    divisor = 1;
    peak = (float)sensor;
    current = peak; 
  } else if ((divisor < 1000) && !(cycle % 10)) {
    divisor = divisor * 1.04;
  }
  diff = current - last;
  if ((diff > 25.0) || (diff < -5.0)) {
    last = current;
    return (int)current;
  } else {
    return (int)last;
  }
}
