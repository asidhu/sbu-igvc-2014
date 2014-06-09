/*
==== GENERAL CONFIG====
 These variables are related to protocol communication.
 EOLCHAR default \n
 BUFFERLENGTH default 256
 SLEEPTIME default 10
 */
#define EOLCHAR '\n'
#define BUFFERLENGTH 256
#define SLEEPTIME 50
#define BAUD_RATE 115200



/*
====DEVICE TOGGLE====
 Use defines to allow certain devices to be handled by this arduino.
 */

#define RAZORIMU
//#define MOTORS
#define GPS_TOGGLE
//#define LEDS_TOGGLE
/*
====DEVICE CONFIG====
 Device specific configuration.
 
 
 
 */

#define MOTOR_PIN 8
#define LED_PIN 13


/*
====PROTOCOL TAG SETUP====
 Determines which device to forward messages to. Tag is basically first char of communication line.
 
 */

#define TAG_RAZORIMU 'R'
#define TAG_MOTORS   'M'
#define TAG_LED	     'L'
#define TAG_LEDON    'O'
#define TAG_LEDOFF   'D'
#define TAG_LEDFLASH 'F'



#include <Wire.h>
#include "RazorIMU.h"
#include "Motors.h"
#include "GPS.h"
#include "LEDS.h"

char buffer[BUFFERLENGTH];
int numRead;

void setup()
{
  // Init serial output
  Serial.begin(BAUD_RATE);
  Serial.setTimeout(20);
  // Init sensors
#ifdef RAZORIMU
  Razor::initialize();
#endif

#ifdef GPS_TOGGLE
  GPSNAMESPACE::initialize();
#endif

#ifdef MOTORS
  Motors::initialize();
#endif

#ifdef LEDS_TOGGLE
  LEDS::initialize();
#endif
  numRead=0;
}


void procInput(char*, int);

void updateDevices();

// Main loop
void loop()
{

#if defined(LEDS_TOGGLE) || defined(MOTORS)
  if(Serial.available()){
    char num = Serial.readBytes(buffer+numRead,BUFFERLENGTH-numRead);
    Serial.println("WTF");
    int last=0;
    for(int i=numRead;i<numRead+num;i++){
      if(buffer[i]==EOLCHAR){
      procInput(buffer+last,i-last);
      last=i;
      }
    }
    numRead=0;
  } 
 #endif
  updateDevices();
  delay(SLEEPTIME);
}

void updateDevices(){

#ifdef RAZORIMU
  Razor::update();
#endif

#ifdef GPS_TOGGLE
  GPSNAMESPACE::update();
#endif

#ifdef LEDS_TOGGLE
  LEDS::update();
#endif

}



void procInput(char* buffer, int numRead){
#ifdef RAZORIMU
  if(buffer[0]==TAG_RAZORIMU){
    Razor::input(buffer,numRead); 
  }
#endif

#ifdef MOTORS
  if(buffer[0]==TAG_MOTORS){
    Motors::input(buffer,numRead); 
  }
#endif
#ifdef LEDS_TOGGLE
 if(buffer[0]==TAG_LED){
   LEDS::input(buffer,numRead); 
  }
#endif 
}
  
