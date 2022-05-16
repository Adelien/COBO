/*
  Melody

  Plays a melody

  circuit:
  - 8 ohm speaker on digital pin 8

  created 21 Jan 2010
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/toneMelody
*/

#include "pitches.h"

// notes in the melody:
int melody[] = {
  
A6-C2-D2,
//Pokémon!

A6, A6,  C2, D2, D2, C2,
//Gotta catch 'em all

C2, C2, D2, D2, C2,
//It's you and me

C2, D2, E2, F2, E2, D2, C2
//I know it's my destiny...
};

// note durA6tions: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 4, 8, 8, 4, 4,
  4, 4, 4, 8, 8,
  4, 4, 4, 4, 8, 8, 8
};

int n=0;
void setup() {
  
  Serial.begin(9600);
 n=0;
}
int var;
void loop() {
  //var = analogRead(A5);

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[n];
    tone(11, melody[n], noteDuration);
    //Serial.println(melody[n]);
    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(8);
    
  n++;
  n=n%19;
  if(n==0){
    delay(2000);
  }
}
