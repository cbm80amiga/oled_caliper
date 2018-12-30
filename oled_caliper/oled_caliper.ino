/*
  (c)2018 Pawel A. Hernik
  YouTube video:
  https://youtu.be/RIt08GJH2IA
*/

/*
   Cheap digital caliper pinout (from top left):
   #1 - GND (black)
   #2 - DAT (green)
   #3 - CLK (blue)
   #4 - VCC 1.5-1.8V (red)
 */

#define CLOCK_PIN 2
#define DATA_PIN  3

#include <OLEDSoftI2C_SSD1306.h>
//#include <OLEDSoftI2C_SH1106.h>
// define USEHW in above header for hw I2C version

OLEDSoftI2C_SSD1306 oled(0x3c);
//OLEDSoftI2C_SH1106 oled(0x3c);
int oled128x32 = 0;

#include "term9x14_font.h"

void setup() 
{
  Serial.begin(115200);
  Serial.println("Ready");
  pinMode(CLOCK_PIN, INPUT);
  pinMode(DATA_PIN, INPUT);
  oled.init(oled128x32);  // 0-128x64, 1-128x32
  oled.clrScr();
  oled.setFont(Term9x14);
  oled.printStr(ALIGN_CENTER, 0, "Caliper");
  oled.setDigitMinWd(9);
  oled.setCharMinWd(9);
}

char buf[20];
unsigned long tmpTime;
int sign;
int inches;
long value;
float result;

void loop()
{
  while(digitalRead(CLOCK_PIN)==LOW) {}
  tmpTime=micros();
  while(digitalRead(CLOCK_PIN)==HIGH) {}
  if((micros()-tmpTime)<500) return;
  readCaliper();
  //Serial.println(result,2); Serial.println(inches);
  buf[0]=' ';
  dtostrf(result,6,3,buf+1); strcat(buf," in ");
  oled.printStr(ALIGN_CENTER, 3, buf);
  dtostrf(result*2.54,6,3,buf+1); strcat(buf," cm ");
  oled.printStr(ALIGN_CENTER, 6, buf);
  delay(500);
}

void readCaliper()
{
  sign=1;
  value=0;
  inches=0;
  for(int i=0;i<24;i++) {
    while(digitalRead(CLOCK_PIN)==LOW) {}
    while(digitalRead(CLOCK_PIN)==HIGH) {}
    if(digitalRead(DATA_PIN)==HIGH) {
      if(i<20) value|=(1<<i);
      if(i==20) sign=-1;
      if(i==23) inches=1; // doesn't work with my caliper, always returns inches
    }
  }
  result=(value*sign)/(inches?2000.0:100.0);
}

