#include <Arduino.h> 
#include "Adafruit_GPS.h"

namespace GPSNAMESPACE{
boolean usingInterrupt = true;

Adafruit_GPS GPS[]={Adafruit_GPS(&Serial1),Adafruit_GPS(&Serial2),Adafruit_GPS(&Serial3)};

void useInterrupt(boolean);
void initialize(){
 for( int i=0;i<3;i++){
    GPS[i].begin(9600);
    // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
   GPS[i].sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  
  // Set the update rate
  GPS[i].sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
//  GPS[1].sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
//  GPS[2].sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);  // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS[i].sendCommand(PGCMD_ANTENNA);
//  GPS[1].sendCommand(PGCMD_ANTENNA);
//  GPS[2].sendCommand(PGCMD_ANTENNA);
  }
 useInterrupt(true);

  delay(1000);
  // Ask for firmware version
  Serial1.println(PMTK_Q_RELEASE);
  Serial2.println(PMTK_Q_RELEASE);
  Serial3.println(PMTK_Q_RELEASE); 
}


// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) {
  //for(int i=0;i<3;i++)  
  GPS[0].read();
  GPS[1].read();
  GPS[2].read();
  // if you want to debug, this is a good time to do it!
#ifdef UDR0
  //if (GPSECHO)
   // if (c) UDR0 = c;  
    // writing direct to UDR0 is much much faster than Serial.print 
    // but only one character can be written at a time. 
#endif
  
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}

void update(){
 for(int i=0;i<3;i++){
   if (GPS[i].newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences! 
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
  
    if (!GPS[i].parse(GPS[i].lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      continue;  // we can fail to parse a sentence in which case we should just wait for another

    Serial.print("GPS"); Serial.print(i,DEC);
    Serial.print(",F,"); Serial.print((int)GPS[i].fix);
    Serial.print(","); Serial.print((int)GPS[i].fixquality); 
    Serial.print('\n');
    if (GPS[i].fix) {
      Serial.print("GPS"); Serial.print(i,DEC);
      Serial.print(",L,");
      Serial.print(GPS[i].latitude, 4); Serial.print(GPS[i].lat);
      Serial.print(","); 
      Serial.print(GPS[i].longitude, 4); Serial.print(GPS[i].lon);
      Serial.print('\n');
      
      Serial.print("GPS"); Serial.print(i,DEC);
      Serial.print(",S,"); Serial.print(GPS[i].speed);
      Serial.print('\n');
      
      Serial.print("GPS"); Serial.print(i,DEC);
      Serial.print(",A,"); Serial.print(GPS[i].angle);
      Serial.print('\n');
      
      Serial.print("GPS"); Serial.print(i,DEC);
      Serial.print(",E,"); Serial.print(GPS[i].altitude);
      Serial.print('\n');
      
      Serial.print("GPS"); Serial.print(i,DEC);
      Serial.print(",N,"); Serial.print((int)GPS[i].satellites);
      Serial.print('\n');
    }
    //GPS[(i+1)%3].begin(9600);
     }
  } 
}

}
