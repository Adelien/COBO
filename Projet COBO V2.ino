#include "Ultrasonic.h"
#include "HX711.h"
#include "LedController.hpp"
#include "ESP32Servo.h"

//Ou les pins sont branchés
#define BOTTLE_SENSOR_PIN 12
#define BALANCE_OUT_PIN 21
#define BALANCE_CLOCK_PIN 22
#define SERVO_PIN 23
#define MOTEUR_1_PIN 13
#define MOTEUR_2_PIN 2
#define LED_DATA_PIN 19
#define LED_CLOCK_PIN 18
#define LED_CS_PIN 5

//Pin capteurs de couleurs
#define S0_PIN 0
#define S1_PIN 0
#define S2_PIN 25
#define S3_PIN 32
#define SENSOR_OUTPUT_PIN 34

#define RED_VALUE 8
#define GREEN_VALUE 8
#define BLUE_VALUE 5
#define CLEAR_VALUE 2

#define RED_RATIO 0.2
#define GREEN_RATIO 0.2
#define BLUE_RATIO 1
#define CLEAR_RATIO 1

#define MAX_COLOR_DISTANCE 7


//Constantes
#define SEUIL_DISTANCE 8
#define MAX_WEIGHT 60 //g
#define MIN_WEIGHT 5
#define MIN_PULSE 1200
#define MAX_PULSE 1800
#define DEVIATION_ANGLE 10
#define MOVING_SERVO_TIME_CW 1100 * 0.5
#define MOVING_SERVO_TIME_CCW 2300 *0.5
#define WAITING_TIME 1000


//Delay
#define TIME_BEFORE_BOTTLE_CRUSHED 7500
#define CRUSH_TIME 15000
#define OPENING_TIME 6000


ByteBlock cross = {
        B00000000,
        B01000010,
        B00100100,
        B00011000,
        B00011000,
        B00100100,
        B01000010,
        B00000000
};

ByteBlock circle = {
        B00000000,
        B00111100,
        B01000010,
        B01000010,
        B01000010,
        B01000010,
        B00111100,
        B00000000
};

template<typename T>
T squared(T t){
  return t*t;
}

class ColorSensor{
    uint8_t S0_pin, S1_pin, S2_pin, S3_pin, sensorOutput;
public:
    ColorSensor(uint8_t s0Pin, uint8_t s1Pin, uint8_t s2Pin, uint8_t s3Pin, uint8_t sensorOutput) : S0_pin(s0Pin),
                                                                                                    S1_pin(s1Pin),
                                                                                                    S2_pin(s2Pin),
                                                                                                    S3_pin(s3Pin),
                                                                                                    sensorOutput(
                                                                                                            sensorOutput) {
        pinMode(s0Pin, OUTPUT);
        pinMode(s1Pin, OUTPUT);
        pinMode(s2Pin, OUTPUT);
        pinMode(s3Pin, OUTPUT);
        pinMode(sensorOutput, INPUT);
        digitalWrite(S0_pin, HIGH);
        digitalWrite(S1_pin, HIGH);
    }

    unsigned long getRedFrequency() const{
        digitalWrite(S2_pin, LOW);
        digitalWrite(S3_pin, LOW);
        return pulseIn(sensorOutput, LOW);
    }
    unsigned long getGreenFrequency() const{
        digitalWrite(S2_pin, HIGH);
        digitalWrite(S3_pin, HIGH);
        return pulseIn(sensorOutput, LOW);
    }
    unsigned long getBlueFrequency() const{
        digitalWrite(S2_pin, LOW);
        digitalWrite(S3_pin, HIGH);
        return pulseIn(sensorOutput, LOW);
    }
    unsigned long getFilterLessFrequency() const{
        digitalWrite(S2_pin, HIGH);
        digitalWrite(S3_pin, LOW);
        return pulseIn(sensorOutput, LOW);
    }

    bool isClear(){
      return getDistance() <= MAX_COLOR_DISTANCE; 
    }

    float getDistance(){
      unsigned long r, g, b, c;
      r = getRedFrequency();
      g = getGreenFrequency();
      b = getBlueFrequency();
      c = getFilterLessFrequency();
    Serial.print("R : ");
    Serial.print(r);
    Serial.print(" G : ");
    Serial.print(g);
    Serial.print(" B : ");
    Serial.print(b);
    Serial.print(" Filter less : ");
    Serial.println(c);
      return (squared(r-RED_VALUE)* RED_RATIO + squared(g-GREEN_VALUE) * GREEN_RATIO + squared(b-BLUE_VALUE) * BLUE_RATIO + squared(c - CLEAR_VALUE) * CLEAR_RATIO);
    }

};


class Balance {
    HX711 scale;
public:
    Balance(uint8_t outPin, uint8_t sckPin, float scale_value = -1798.f) {
        //Initialisation de la balance
        //OutPin, Clock Pin
        scale.begin(outPin, sckPin);
        //Valeur dépend des tests
        scale.set_scale(scale_value);
        scale.tare();
    }

    const HX711 &getScale() const {
        return scale;
    }

    float getWeight() {
        scale.power_up();
        float weight = scale.get_units(10);
        scale.power_down();
        return weight;
    }

    bool isValidWeight(float weight = -1) {
        weight = (weight == -1) ? getWeight() : weight;
        return weight < MAX_WEIGHT && weight > MIN_WEIGHT;
    }
};

//33 27 13 6 //bleu
//39 41 36 13 //vert
//15 23 15 5 //rouge transparent
//9 8 6 2//vert transparent
//31 40 31
//36 40 24 bleu

class Moteur {
    uint8_t pin1;
    uint8_t pin2;
public:
    Moteur(uint8_t pin1, uint8_t pin2) {
        this->pin1 = pin1;
        this->pin2 = pin2;
        pinMode(pin1, OUTPUT);
        pinMode(pin2, OUTPUT);
        stop();
    }

    void forward() {
        digitalWrite(pin1, HIGH);
        digitalWrite(pin2, LOW);
    }

    void backward() {
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, HIGH);
    }

    void forward(long time) {
        forward();
        delay(time);
        stop();
    }

    void backward(long time) {
        backward();
        delay(time);
        stop();
    }

    void stop() {
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, LOW);
    }

};


Ultrasonic bottleSensor(BOTTLE_SENSOR_PIN);
Balance balance(BALANCE_OUT_PIN, BALANCE_CLOCK_PIN);
Servo servo;
Moteur moteur(MOTEUR_1_PIN, MOTEUR_2_PIN);
LedController ledController;
ColorSensor colorSensor(S0_PIN, S1_PIN, S2_PIN, S3_PIN, SENSOR_OUTPUT_PIN);

void setup() {
    Serial.begin(115200); //115200 baudrate (vitesse de communication)
    Serial.println("Starting...");
    servo.setPeriodHertz(50);
    servo.attach(SERVO_PIN, MIN_PULSE, MAX_PULSE);
    servo.write(90);
    ledController.init(LED_DATA_PIN, LED_CLOCK_PIN, LED_CS_PIN, 1);
    Serial.println("Started");
// write your initialization code here
}
//Pas ouf de mettre des variable ici mais j'ai pas d'immense idée de simplification
int motorState = 0, servoState = 0; //0 stopped -1 CW/backward 1 CCW/forward
unsigned long motorMillis, servoMillis;
void loop() {
    if(Serial.available()){
        String data = Serial.readString();
        data.replace("\n", "");
        Serial.println("Read ");
        Serial.println(data);
        if(data.equalsIgnoreCase("forward")){
            Serial.println("Motor is forced forward");
            switch(motorState){
                case -1:
                    Serial.print("The motor ran backward for ");
                    Serial.println(millis() - motorMillis);
                    motorMillis = millis();
                    motorState = 1;
                    break;
                case 0:
                    motorMillis = millis();
                    motorState = 1;
                    break;
                case 1:
                    Serial.println("The motor was already running forward");
            }
            moteur.forward();
        }else if(data.equalsIgnoreCase("backward")){
            Serial.println("Motor is forced backward");
            switch(motorState){
                case 1:
                    Serial.print("The motor ran forward for ");
                    Serial.println(millis() - motorMillis);
                    motorMillis = millis();
                    motorState = -1;
                    break;
                case 0:
                    motorMillis = millis();
                    motorState = -1;
                    break;
                case -1:
                    Serial.println("The motor was already running backward");
            }
            moteur.backward();
        }else if(data.equalsIgnoreCase("stop")){
            Serial.println("Motor is stopped");
            moteur.stop();
            if(motorState){
                Serial.print("Motor was running ");
                Serial.print((motorState == -1 ? "backward " : "forward "));
                Serial.print("for ");
                Serial.println(millis() - motorMillis);
            }
            motorState = 0;
        }else if(data.equalsIgnoreCase("cw")){
            Serial.println("servo is going clockwise");
            switch(servoState){
                case 1:
                    Serial.print("The servo ran counter-clockwise for ");
                    Serial.println(millis() - servoMillis);
                    servoMillis = millis();
                    servoState = -1;
                    break;
                case 0:
                    servoMillis = millis();
                    servoState = -1;
                    break;
                case -1:
                    Serial.println("The motor was already running clockwise");
            }
            servo.write(90 - DEVIATION_ANGLE);
        }else if(data.equalsIgnoreCase("sstop")){
            Serial.println("Servo is stopped");
            servo.write(90);
            if(servoState){
                Serial.print("Servo was running ");
                Serial.print((servoState == -1 ? "clockwise " : "counter-clockwise "));
                Serial.print("for ");
                Serial.println(millis() - servoMillis);
            }
            servoState = 0;
        }else if(data.equalsIgnoreCase("ccw")){
            Serial.println("servo is going counter clockwise");
            servo.write(90 + DEVIATION_ANGLE);
            switch(servoState){
                case -1:
                    Serial.print("The servo ran clockwise for ");
                    Serial.println(millis() - servoMillis);
                    servoMillis = millis();
                    servoState = 1;
                    break;
                case 0:
                    servoMillis = millis();
                    servoState = 1;
                    break;
                case 1:
                    Serial.println("The motor was already running counter-clockwise");
            }
        }
    }
    if(motorState || servoState){
        delay(99);
        return;
    }
    delay(1000);
    Serial.print("R : ");
    Serial.print(colorSensor.getRedFrequency());
    Serial.print(" G : ");
    Serial.print(colorSensor.getGreenFrequency());
    Serial.print(" B : ");
    Serial.print(colorSensor.getBlueFrequency());
    Serial.print(" Filter less : ");
    Serial.println(colorSensor.getFilterLessFrequency());
    Serial.println("clearing matrix");
    ledController.clearMatrix();
    Serial.println("Cleared");
    long dist = bottleSensor.MeasureInCentimeters();
    Serial.print("Mesured distance : ");
    Serial.println(dist);

    if (dist < SEUIL_DISTANCE) {
      delay(1500);
        float weight = balance.getWeight();
        Serial.print("Mesured weight : ");
        Serial.println(weight);
        if (balance.isValidWeight(weight)) {
            ledController.displayOnSegment(0, circle);
            bool clockWise = colorSensor.isClear();
            delay(TIME_BEFORE_BOTTLE_CRUSHED);
            moteur.forward(CRUSH_TIME);
            moteur.backward(OPENING_TIME);
            //int8_t dir = ((millis() % 2) == 0) ? 2 : -1; //1 CCW -1 CW
            //bool clockWise = true;
            if(clockWise){
              servo.write(90 - DEVIATION_ANGLE);
              delay(MOVING_SERVO_TIME_CW*2);
              servo.write(90);
              delay(WAITING_TIME);
              servo.write(90 + DEVIATION_ANGLE);
              delay(MOVING_SERVO_TIME_CCW*2 -200);
            }else{
              servo.write(90 + DEVIATION_ANGLE);
              delay(MOVING_SERVO_TIME_CCW);
              servo.write(90);
              delay(WAITING_TIME);
              servo.write(90 - DEVIATION_ANGLE);
              delay(MOVING_SERVO_TIME_CW);
            }
            servo.write(90);
        } else {
            Serial.println("Bottle is not in the bounds");
            ledController.displayOnSegment(0, cross);
        }
    }
}