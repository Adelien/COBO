#include "Ultrasonic.h"
#include "HX711.h"
#include <ESP32Servo.h> 
#include "LedController.hpp"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
//#include <Servo.h>

long dist;
long remp;
float poids;

const int LOADCELL_DOUT_PIN = 21;
const int LOADCELL_SCK_PIN = 22;
const int ULTRA_PRESENCE = 12;
const int ULTRA_REMP = 10;
const int SERVO_PIN = 23;
const int COUL_S0=34;
const int COUL_S1=35;
const int COUL_S2=32;
const int COUL_S3=33;
const int COUL_OUT=25;
const int LED_DIN=19;
const int LED_CS=5;
const int LED_CLK=18;
const int MOT_1=15;
const int MOT_2=2;

HX711 scale;
Ultrasonic ultrapres(ULTRA_PRESENCE);
Ultrasonic ultraremp(ULTRA_REMP);
const int Segments=1;
LedController<1,1> lc = LedController<Segments,1>();
Servo servo;
WiFiServer server(80);


const long seuil=8;
const long marge=20;
const float poids_max=60;
int vdr = 102;
int vgh =80;
int tdr=400;
int tgh=1100;
const char *ssid = "COBO";

ByteBlock X= ByteBlock::reverse({//X
  B00000000,
  B01000010,
  B00100100,
  B00011000,
  B00011000,
  B00100100,
  B01000010,
  B00000000
});
ByteBlock O= ByteBlock::reverse({//O
  B00000000,
  B00111100,
  B01000010,
  B01000010,
  B01000010,
  B01000010,
  B00111100,
  B00000000
});

void Tare(){
  Serial.println("Tare");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(-1798.f);
  scale.tare();
  Serial.println("Tare terminée");
}
void Servtup(){
  Serial.println("Allumage des servos");
  servo.setPeriodHertz(50);
  servo.attach(SERVO_PIN, 1200, 1800);
  Serial.println("Servos allumés");
}
void Motus(){
  Serial.println("Allumage des moteurs");
  pinMode(MOT_1, OUTPUT);
  pinMode(MOT_2, OUTPUT);
  Serial.println("Moteurs allumés");
}
void UpLink(){
  WiFi.softAP(ssid);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();
  Serial.println("Server started");
}
void LedOn(){
  lc.init(LED_DIN,LED_CLK,LED_CS);
}
void setup() {
  Serial.begin(115200);
  Serial.println("Initialisation");
  
  Tare();
  Servtup();
  Motus();
  UpLink();
  LedOn();
  Serial.println();
}

void Ultra(){
  Serial.print("Mesure distance: ");
  dist = ultrapres.MeasureInCentimeters();
  Serial.println(dist);
}
void UltraRempli(){
  Serial.print("Mesure remplissage: ");
  remp=ultraremp.MeasureInCentimeters()-marge;
  //Afficher l'info
  Serial.println(remp);
}
void balance(){
  Serial.print("Pèse: ");
  scale.power_up();
  poids= scale.get_units(10);
  scale.power_down();
  Serial.println(poids);
}
void Coul(){
  //
}
void Compress(){
  Serial.println("Compresse...");
  digitalWrite(MOT_2,HIGH);
  digitalWrite(MOT_1,LOW);

  delay(15000);

  digitalWrite(MOT_2,LOW);
  digitalWrite(MOT_1,HIGH);

  delay(5000);

  digitalWrite(MOT_1,0);
  digitalWrite(MOT_2,0);
  Serial.println("Fait !");
}
void Wifi(){
  Serial.println("Wifi");
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Utilisateur connecté");
    String currentLine = "";
    while (client.connected()) {
    if (client.available()) {
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Place restante: ");
            client.println(remp);
            client.print("<br>Poids: ");
            client.println(poids);
            if(poids>poids_max){
              client.println("Trop lourd !!!");
            }
            client.print("<br>Dist: ");
            client.println(dist);
             client.println("<a href=\"/\">Actualiser</a>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
    }
    }
    client.stop();
    Serial.println("Client Disconnected.");
  }
}


void loop() {
  String inputString = "";
  int sens=0;
  while (Serial.available())                                              // Enquanto tiver dados na serial
  {
     inputString = Serial.readString();                                    // Le um byte:
      sens = inputString.toInt();                                    // Transforma a string em inteiro
      Serial.println(sens);
      inputString = "";
  }
  delay(100);
  if(sens==1){
  Serial.println("Sens");
  digitalWrite(MOT_1, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(MOT_2, LOW);    // turn the LED off by making the voltage LOW
  }
  else if(sens==2){
  Serial.println("Contre Sens");
  digitalWrite(MOT_2, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(MOT_1, LOW);    // turn the LED off by making the voltage LOW
  }
  else{
    digitalWrite(MOT_2, LOW);   // turn the LED on (HIGH is the voltage level)
    digitalWrite(MOT_1, LOW);    // turn the LED off by making the voltage LOW
  
  }

  lc.clearMatrix();
  Ultra();
  Serial.println(dist);
  if(dist<seuil){

    balance();
    if(poids<poids_max){
      lc.displayOnSegment(lc.getConfig().getSegmentNumber(0,1),O);    
      Coul();//Ne fais rien

      delay(5000);
      Compress();
      delay(555);
      if(poids<poids_max){
        servo.write(vgh);
        delay(tgh);
        servo.write(90);
        delay(1000);
        servo.write(vdr);
        delay(tgh);
      }
      else{
        servo.write(vdr);
        delay(tdr);
        servo.write(90);
        delay(1000);        
        servo.write(vgh);
        delay(tdr);
      }
      servo.write(90);
      UltraRempli();
    }
    else{
      Serial.println("Erreur: bouteille trop lourde");
      lc.displayOnSegment(lc.getConfig().getSegmentNumber(0,1),X);
    }
  }
  Wifi();
  delay(1000);
  servo.write(91);
}
