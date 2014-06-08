/*
==== GENERAL CONFIG====
 These variables are related to protocol communication.
 EOLCHAR default \n
 BUFFERLENGTH default 256
 SLEEPTIME default 10
 */
#define EOLCHAR '\n'
#define BUFFERLENGTH 256
#define SLEEPTIME 10
#define BAUD_RATE 115200



/*
====DEVICE TOGGLE====
 Use defines to allow certain devices to be handled by this arduino.
 */

#define RAZORIMU
#define MOTORS
#define GPS_TOGGLE
/*
====DEVICE CONFIG====
 Device specific configuration.
 
 
 
 */

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

char buffer[BUFFERLENGTH];
int numRead;

int led_mode;
int led_timer;
void setup()
{
  // Init serial output
  Serial.begin(BAUD_RATE);
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
  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN,HIGH);
  led_mode =2;
  numRead=0;
}


void procInput();

void updateDevices();

// Main loop
void loop()
{


  if(Serial.available()){
    char num = Serial.readBytesUntil(EOLCHAR,buffer+numRead,BUFFERLENGTH-numRead);
    numRead+=num;
    if(buffer[numRead]==EOLCHAR){
      buffer[numRead]=0;
      procInput();

    }
  } 
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
  if(led_mode==0)
    digitalWrite(LED_PIN,HIGH);
  else if(led_mode==1)
    digitalWrite(LED_PIN,LOW);
  else if( led_mode==2){
     digitalWrite(LED_PIN,LOW);
     if(millis()-led_timer>1000){
        led_timer = millis();
         led_mode=3;
     } 
  }
  else if( led_mode==3){
     digitalWrite(LED_PIN,HIGH);
     if(millis()-led_timer>1000){
        led_timer = millis();
         led_mode=2;
     } 
  }

}



void procInput(){

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
 
#ifdef LED_DEVICE
  if(buffer[0] == TAG_LED){
	if(buffer[1] == TAG_LEDON){
		led_mode=1;
	}else if(buffer[1]==TAG_LEDOFF){
		led_mode=0;
	}else if(buffer[1]==TAG_LEDFLASH){
		led_mode=2;
		led_timer=millis();
	}
  }
#endif 
}
  
