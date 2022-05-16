#define S0 25
#define S1 35
#define S2 32
#define S3 33
#define OUT 34

int RED_freq = 0;
int GREEN_freq = 0;
int BLUE_freq = 0;

int red_value = 0;
int green_value = 0;
int blue_value = 0;

void setup() {
  Serial.begin(115200);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  
  pinMode(OUT, INPUT);
  
  digitalWrite(S0,HIGH);
  digitalWrite(S1,HIGH);
}

void loop() {
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  RED_freq = pulseIn(OUT, LOW);
  red_value = map(RED_freq, 140, 560, 255,0);
  Serial.print("R = ");
  Serial.print(RED_freq/*red_value*/);
  delay(200);
  
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  GREEN_freq = pulseIn(OUT, LOW);
  green_value = map(GREEN_freq, 215, 480, 255, 0);
  Serial.print(" G = ");
  Serial.print(GREEN_freq/*green_value*/);
  delay(200);
 
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  BLUE_freq = pulseIn(OUT, LOW);
  blue_value = map(BLUE_freq, 36, 430
  , 255, 0);
  Serial.print(" B = ");
  Serial.print(BLUE_freq/*blue_value*/);
  delay(200);

  if(red_value > green_value && red_value > blue_value){
      Serial.println(" RED detected!");
  }
  if(green_value > red_value && green_value > blue_value){
    Serial.println(" GREEN detected!");
  }
  if(blue_value > red_value && blue_value > green_value){
    Serial.println(" BLUE detected!");
  }
}