int var;
void setup() {
  pinMode(11, OUTPUT);
  pinMode(7, INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  var = analogRead(A0);
  var=var-300;
  Serial.println(var);
  /*if(var<500){
  digitalWrite(11,HIGH);
  }
  else{

  digitalWrite(11,LOW);
  }*/
  analogWrite(11,var);
  delay(200);
}
