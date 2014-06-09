#include "Arduino.h"

namespace Motors{

/*
Config: Motors reset setup gpio pin

*/

  
  
void initialize(){
   delay(100);
   pinMode(MOTOR_PIN,OUTPUT);
   digitalWrite(MOTOR_PIN,HIGH);
}


void input(char* data, int len){
    digitalWrite(MOTOR_PIN,HIGH);
    delay(500);
    digitalWrite(MOTOR_PIN,LOW);
    delay(500);
    digitalWrite(MOTOR_PIN,HIGH);
}

}

