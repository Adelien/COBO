int lign=7;//Relié au PNP
int col=LED_BUILTIN;//pin 13 Relié au NPN
void setup() {
  Serial.begin(9600);
  pinMode(lign,OUTPUT);
  pinMode(col,OUTPUT);
}

void loop() {

  digitalWrite(lign,LOW);//Pour PNP, LOW=>Courant passe
  digitalWrite(col,HIGH);//Pour NPN, HIGH=>Courant passe
  Serial.write("01\n");

  delay(500);

  /*digitalWrite(lign,HIGH);
  digitalWrite(col,LOW);
  Serial.write("10\n");

  delay(500);

  digitalWrite(lign,HIGH);
  digitalWrite(col,HIGH);
  Serial.write("11\n");

  delay(500);
  
  digitalWrite(lign,LOW);
  digitalWrite(col,LOW);
  Serial.write("00\n");

  delay(500);*/
}
