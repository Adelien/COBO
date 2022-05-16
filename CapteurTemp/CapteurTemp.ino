float var=0;
float T;
void setup() {
  // put your setup code here, to run once:
  pinMode(11, OUTPUT);
  pinMode(7, INPUT);
  Serial.begin(9600);
}

void loop() {
  var = analogRead(A5);
  Serial.print(var);
  Serial.print(" ");
  T=(float)((float)125/256)*var-50;
  Serial.println(T);
  analogWrite(11,var);
  delay(200);
}
