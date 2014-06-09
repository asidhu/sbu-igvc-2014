#include "Arduino.h"

namespace LEDS{



int led_mode;
unsigned long led_timer;
  
void initialize(){
  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN,HIGH);
  led_mode =1;
}


void input(char* buffer, int len){
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
  
}


void update(){
 if(led_mode==0)
    digitalWrite(LED_PIN,HIGH);
  else if(led_mode==1)
    digitalWrite(LED_PIN,LOW);
  else if( led_mode==2){
     digitalWrite(LED_PIN,LOW);
     if(millis()-led_timer>1000 || millis()<led_timer){
        led_timer = millis();
         led_mode=3;
     } 
  }
  else if( led_mode==3){
     digitalWrite(LED_PIN,HIGH);
     if(millis()-led_timer>1000 || millis()<led_timer){
        led_timer = millis();
         led_mode=2;
     } 
  } 
}
}

