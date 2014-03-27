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


/*
====DEVICE CONFIG====
Device specific configuration.



*/




/*
====PROTOCOL TAG SETUP====
Determines which device to forward messages to. Tag is basically first char of communication line.

*/

#define TAG_RAZORIMU 'R'





#include <Wire.h>
#include "RazorIMU.h"


char buffer[BUFFERLENGTH];
int numRead;

void setup()
{
  // Init serial output
  Serial.begin(BAUD_RATE);
    // Init sensors
  Razor::initialize();
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
     
     
  
}



void procInput(){
    
     #ifdef RAZORIMU
       if(buffer[0]==TAG_RAZORIMU){
         Razor::input(buffer,numRead); 
       }
     #endif
     
}
