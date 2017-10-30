#include "Enes100.h"
Enes100 enes("Black Box Postal Service", BLACK_BOX, 178, 8, 9);          //Make sure to change marker number and rx and tx pins to reflect OSV

//Set pin values
#define infraredLeftPin 0
#define infraredRightPin 0
#define crashLeftPin 0
#define crashMiddlePin 0
#define crashRightPin 0

#define motorLeftPin 0
#define motorRightPin 0

//Other constants
#define checkTime 100

//Variable defintitions
int phase 1;

void setup() {
  if(!enes.updateLocation()) {
    enes.println("The OSV can't find its location.");
  }
}

void loop() {
  if(phase == 1){
    while(enes.location.theta > PI / 16 && enes.location.theta < 31 * PI / 16) {
      if(enes.location.theta > PI / 16 && enes.location.theta < PI) {
        enes.println("The OSV turns right.");
      }

      if(enes.location.theta > PI && enes.location.theta < 31 * PI / 16) {
        enes.println("The OSV turns left.");
      }

      delay(checkTime);

      if(!enes.updateLocation()) {
       enes.println("The OSV can't find its location.");
      }
    }

    if(enes.location.x < 1.1) {
      enes.println("The OSV moves forward.");
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
      delay(checkTime);
    }

    while(infraredLeft == LOW && infraredLeft == HIGH) {
      enes.println("The OSV turns right.");
      delay(checkTime);
    }

    enes.println("The OSV moves forward.");

    if(crashLeft == HIGH || crashMiddle == HIGH || crashRight == HIGH) {
      enes.println("The OSV hit something and entered phase #3.");
      phase = 3;
    }

    if(!enes.updateLocation()) {
      enes.println("The OSV Can't find its location.");
    }
  }

  while(phase == 3) {
    \\This is where we decide what to do when we hit something0
  }
}
