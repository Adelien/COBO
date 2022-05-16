#include <ESP32Servo.h>

Servo aaa;
int a,b;
const int ledR=5;
const int ledJ=6;

void setup() {
  aaa.attach(4);//3,5,6,9,10 ou 11 car elles peuvent écrire en analogique
  Serial.begin(115200);
}

void loop() {
  
  aaa.write(90);
  delay(5000);
  aaa.write(0);
  delay(5000);
  Serial.write("aaa.read()\n");
  delay(10000);
  /*
    digitalWrite(ledJ,LOW);
    digitalWrite(ledR,HIGH);
  for(int i=0; i<a; i+=2){
    aaa.write(i);
    delay(20);
  }
  delay(100);*/
}