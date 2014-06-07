#include "Arduino.h"

namespace Motors{

/*
Config: Motors reset setup gpio pin

*/
int motor_reset_pin=8;

  
  
void initialize(){
   delay(100);
   pinMode(motor_reset_pin,OUTPUT);
   digitalWrite(motor_reset_pin,HIGH);
}


void input(char* data, int len){
    digitalWrite(motor_reset_pin,HIGH);
    delay(500);
    digitalWrite(motor_reset_pin,LOW);
    delay(500);
    digitalWrite(motor_reset_pin,HIGH);
}

}

