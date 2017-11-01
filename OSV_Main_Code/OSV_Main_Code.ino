#include "Enes100.h"
Enes100 enes("Black Box Postal Service", BLACK_BOX, 178, 8, 9); //Make sure to change marker number and rx and tx pins to reflect OSV

//Set pin values
#define infraredLeftPin 0
#define infraredRightPin 0
#define crashLeftPin 3
#define crashMiddlePin 2
#define crashRightPin 1

#define motorLeftEnable 5
#define motorLeftOutput1 6
#define motorLeftOutput2 7

#define motorRightEnable 10
#define motorRightOutput1 8
#define motorRightOutput2 9

//Other constants
#define checkTime 100

//Variable defintitions
int phase 1;

void setup() {
  if(!enes.updateLocation()) {
    enes.println("The OSV can't find its location.");
  }
  
  pinMode(motorLeftEnable, OUTPUT);
  pinMode(motorLeftOutput1, OUTPUT);
  pinMode(motorLeftOutput2, OUTPUT);

  pinMode(motorRightEnable, OUTPUT);
  pinMode(motorRightOutput1, OUTPUT);
  pinMode(motorRightOutput2, OUTPUT);

  pinMode(crashLeftPin, INPUT);
  pinMode(crashMiddlePin, INPUT);
  pinMode(crashRightPin, INPUT);
}

void loop() {
  if(phase == 1){
    while(enes.location.theta > PI / 16 && enes.location.theta < 31 * PI / 16) {
      analogWrite(motorLeftEnable, 200);
      analogWrite(motorRightEnable, 200);
      if(enes.location.theta > PI / 16 && enes.location.theta < PI) {
        enes.println("The OSV turns right.");
        digitalWrite(motorLeftInput1, LOW);
        digitalWrite(motorLeftInput2, HIGH);
        digitalWrite(motorRightInput1, HIGH);
        digitalWrite(motorRightInput2, LOW);
      }

      if(enes.location.theta > PI && enes.location.theta < 31 * PI / 16) {
        enes.println("The OSV turns left.");
        digitalWrite(motorLeftInput1, HIGH);
        digitalWrite(motorLeftInput2, LOW);
        digitalWrite(motorRightInput1, LOW);
        digitalWrite(motorRightInput2, HIGH);
      }

      delay(checkTime);

      if(!enes.updateLocation()) {
       enes.println("The OSV can't find its location.");
      }
    }

    if(enes.location.x < 1.1) {
      enes.println("The OSV moves forward.");
      digitalWrite(motorLeftInput1, LOW);
      digitalWrite(motorLeftInput2, HIGH);
      digitalWrite(motorRightInput1, LOW);
      digitalWrite(motorRightInput2, HIGH);
    }

    if(enes.location.x > 1.1) {
      enes.println("The OSV crossed the rough terrarain and entered phase #2.");
      phase = 2;
    }

    if(!enes.updateLocation()) {
      enes.println("The OSV can't find its location.");
    }
  }
  
  while(phase == 2) {
    while(infraredLeft == HIGH && infraredRight == LOW) {
      enes.println("The OSV turns left.");
      digitalWrite(motorLeftInput1, HIGH);
      digitalWrite(motorLeftInput2, LOW);
      digitalWrite(motorRightInput1, LOW);
      digitalWrite(motorRightInput2, HIGH);
      delay(checkTime);
    }

    while(infraredLeft == LOW && infraredLeft == HIGH) {
      enes.println("The OSV turns right.");
      digitalWrite(motorLeftInput1, LOW);
      digitalWrite(motorLeftInput2, HIGH);
      digitalWrite(motorRightInput1, HIGH);
      digitalWrite(motorRightInput2, LOW);
      delay(checkTime);
    }

    enes.println("The OSV moves forward.");
    digitalWrite(motorLeftInput1, LOW);
    digitalWrite(motorLeftInput2, HIGH);
    digitalWrite(motorRightInput1, LOW);
    digitalWrite(motorRightInput2, HIGH);

    if(crashLeft == HIGH || crashMiddle == HIGH || crashRight == HIGH) {
      enes.println("The OSV hit something and entered phase #3.");
      phase = 3;
    }

    if(!enes.updateLocation()) {
      enes.println("The OSV Can't find its location.");
    }
  }

  while(phase == 3) {
    \\This is where we decide what to do when we hit something
    if(crashLeft == HIGH && crashRight == LOW) {
      if(crashMiddle == HIGH){
        //turn right a lot
      }
      else{
        //turn right a little
      }
    }
    if(crashLeft == LOW && crashRight == HIGH) {
      if(crashMiddle == HIGH){
        //turn left a lot
      }
      else{
        //turn left a little
      }
    }
    
    if(crashLeft == HIGH && crashRight == HIGH) {
      //go to the center of the arena
    }
    
    if(crashMiddle == HIGH && crashLeft == LOW && crashRight == LOW) {
      // mission complete
    }
  }
}
//testing 123
