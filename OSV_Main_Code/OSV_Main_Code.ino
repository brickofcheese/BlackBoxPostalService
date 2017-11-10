#include "Enes100.h"
#include "IRremote.h"

//Set pin values
#define infraredLeftPin 11
#define infraredRightPin 12
#define crashLeftPin 2
#define crashMiddlePin 3
#define crashRightPin 4

#define motorLeftEnable A5
#define motorLeftInput1 8
#define motorLeftInput2 9

#define motorRightEnable A4
#define motorRightInput1 6
#define motorRightInput2 7

//Other constants
#define checkTime 100

//Variable defintitions
int phase = 1;
int obstacleX;
int obstacleY;
int obstacleTheta;

Enes100 enes("Black Box Postal Service", BLACK_BOX, 199, 1, 0); //Make sure to change marker number and rx and tx pins to reflect OSV
IRrecv irLeft(infraredLeftPin);
IRrecv irRight(infraredRightPin);

decode_results resultsLeft;
decode_results resultsRight;

void setup() {
  if(!enes.updateLocation()) {
    enes.println("The OSV can't find its location.");
  }
  
  pinMode(motorLeftEnable, OUTPUT);
  pinMode(motorLeftInput1, OUTPUT);
  pinMode(motorLeftInput2, OUTPUT);

  pinMode(motorRightEnable, OUTPUT);
  pinMode(motorRightInput1, OUTPUT);
  pinMode(motorRightInput2, OUTPUT);

  pinMode(crashLeftPin, INPUT);
  pinMode(crashMiddlePin, INPUT);
  pinMode(crashRightPin, INPUT);

  irLeft.enableIRIn();
  irRight.enableIRIn();
}

void loop() {
  //In phase 1, the OSV tries to escape the landing zone and traverse the rough terrain.
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
      enes.println("The OSV crossed the rough terrarain and entered phase 2.");
      phase = 2;
    }

    if(!enes.updateLocation()) {
      enes.println("The OSV can't find its location.");
    }
  }

  //In phase 2, the OSV searches for and moves towards the Black Box. If the OSV hits an obstacle, it enters phase #3. 
  while(phase == 2) {
    while(irLeft.decode(&resultsLeft) && !irRight.decode(&resultsRight)) {
      enes.println("The OSV turns left.");
      digitalWrite(motorLeftInput1, HIGH);
      digitalWrite(motorLeftInput2, LOW);
      digitalWrite(motorRightInput1, LOW);
      digitalWrite(motorRightInput2, HIGH);
      delay(checkTime);
    }

    while(!irLeft.decode(&resultsLeft) && irRight.decode(&resultsRight)) {
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

    if(!digitalRead(crashLeftPin) == HIGH || !digitalRead(crashMiddlePin) == HIGH || !digitalRead(crashRightPin) == HIGH) {
      enes.println("The OSV hit something and entered phase 3.");
      phase = 3;
    }

    if(!enes.updateLocation()) {
      enes.println("The OSV can't find its location.");
    }
  }

  //In phase 3, the OSV decides what to when it hits something. It then follows a preset list of instructions to go around an obstacle.
  while(phase == 3) {
    //This is where we decide what to do when we hit something
    if(!enes.updateLocation()) {
      enes.println("The OSV can't find its location.");
    }

    obstacleX = enes.location.x;
    obstacleY = enes.location.y;
    obstacleTheta = enes.location.theta;
    
    if(!digitalRead(crashLeftPin) == HIGH && !digitalRead(crashRightPin) == LOW) {
      if(!digitalRead(crashMiddlePin) == HIGH){
        enes.println("The OSV will circumnavigate the obstacle on the right side.");
        while(sqrt(pow(enes.location.x - obstacleX, 2) + pow(enes.location.y - obstacleY, 2)) < 0.30) {
          enes.println("The OSV moves backwards.");
          digitalWrite(motorLeftInput1, HIGH);
          digitalWrite(motorLeftInput2, LOW);
          digitalWrite(motorRightInput1, HIGH);
          digitalWrite(motorRightInput2, LOW);

          delay(checkTime);

          if(!enes.updateLocation()){
            enes.println("The OSV can't find its location.");
          }
        }
        
        while(abs(enes.location.theta - obstacleTheta) < PI/3) {
          enes.println("The OSV turns right.");
          digitalWrite(motorLeftInput1, LOW);
          digitalWrite(motorLeftInput2, HIGH);
          digitalWrite(motorRightInput1, HIGH);
          digitalWrite(motorRightInput2, LOW);

          delay(checkTime);

          if(!enes.updateLocation()){
            enes.println("The OSV can't find its location.");
          }
        }

        obstacleX = enes.location.x;
        obstacleY = enes.location.y;

        while(sqrt(pow(enes.location.x - obstacleX, 2) + pow(enes.location.x - obstacleX, 2)) < 0.60) {
          enes.println("The OSV moves foward.");
          digitalWrite(motorLeftInput1, LOW);
          digitalWrite(motorLeftInput2, HIGH);
          digitalWrite(motorRightInput1, LOW);
          digitalWrite(motorRightInput2, HIGH);

          delay(checkTime);

          if(!enes.updateLocation()) {
            enes.println("The OSV can't find its location.");
          }
        }

        while(!irLeft.decode(&resultsLeft) && !irRight.decode(&resultsRight)) {
          enes.println("The OSV turns left.");
          digitalWrite(motorLeftInput1, HIGH);
          digitalWrite(motorLeftInput2, LOW);
          digitalWrite(motorRightInput1, LOW);
          digitalWrite(motorRightInput2, HIGH);

          delay(checkTime);

          if(!enes.updateLocation()){
            enes.println("The OSV can't find its location.");
          }
        }
      }
      if(!digitalRead(crashMiddlePin) == LOW){
        enes.println("The OSV will circumnavigate the obstacle on the right side.");
        while(sqrt(pow(enes.location.x - obstacleX, 2) + pow(enes.location.y - obstacleY, 2)) < 0.20) {
          enes.println("The OSV moves backwards.");
          digitalWrite(motorLeftInput1, HIGH);
          digitalWrite(motorLeftInput2, LOW);
          digitalWrite(motorRightInput1, HIGH);
          digitalWrite(motorRightInput2, LOW);

          delay(checkTime);

          if(!enes.updateLocation()){
            enes.println("The OSV can't find its location.");
          }
        }
        
        while(abs(enes.location.theta - obstacleTheta) < PI/2) {
          enes.println("The OSV turns right.");
          digitalWrite(motorLeftInput1, LOW);
          digitalWrite(motorLeftInput2, HIGH);
          digitalWrite(motorRightInput1, HIGH);
          digitalWrite(motorRightInput2, LOW);

          delay(checkTime);

          if(!enes.updateLocation()){
            enes.println("The OSV can't find its location.");
          }
        }

        obstacleX = enes.location.x;
        obstacleY = enes.location.y;

        while(sqrt(pow(enes.location.x - obstacleX, 2) + pow(enes.location.x - obstacleX, 2)) < 0.20) {
          enes.println("The OSV moves foward.");
          digitalWrite(motorLeftInput1, LOW);
          digitalWrite(motorLeftInput2, HIGH);
          digitalWrite(motorRightInput1, LOW);
          digitalWrite(motorRightInput2, HIGH);

          delay(checkTime);

          if(!enes.updateLocation()) {
            enes.println("The OSV can't find its location.");
          }
        }

        while(!irLeft.decode(&resultsLeft) && !irRight.decode(&resultsRight)) {
          enes.println("The OSV turns left.");
          digitalWrite(motorLeftInput1, HIGH);
          digitalWrite(motorLeftInput2, LOW);
          digitalWrite(motorRightInput1, LOW);
          digitalWrite(motorRightInput2, HIGH);

          delay(checkTime);

          if(!enes.updateLocation()){
            enes.println("The OSV can't find its location.");
          }
        }
      }
    }
    
    if(!digitalRead(crashLeftPin) == LOW && !digitalRead(crashRightPin) == HIGH) {
      if(!digitalRead(crashMiddlePin) == HIGH) {
        enes.println("The OSV will circumnavigate the obstacle on the left side.");
        while(sqrt(pow(enes.location.x - obstacleX, 2) + pow(enes.location.y - obstacleY, 2)) < 0.30) {
          enes.println("The OSV moves backwards.");
          digitalWrite(motorLeftInput1, HIGH);
          digitalWrite(motorLeftInput2, LOW);
          digitalWrite(motorRightInput1, HIGH);
          digitalWrite(motorRightInput2, LOW);

          delay(checkTime);

          if(!enes.updateLocation()){
            enes.println("The OSV can't find its location.");
          }
        }
        
        while(abs(enes.location.theta - obstacleTheta) < PI/3) {
          enes.println("The OSV turns left.");
          digitalWrite(motorLeftInput1, HIGH);
          digitalWrite(motorLeftInput2, LOW);
          digitalWrite(motorRightInput1, LOW);
          digitalWrite(motorRightInput2, HIGH);

          delay(checkTime);

          if(!enes.updateLocation()){
            enes.println("The OSV can't find its location.");
          }
        }

        obstacleX = enes.location.x;
        obstacleY = enes.location.y;

        while(sqrt(pow(enes.location.x - obstacleX, 2) + pow(enes.location.x - obstacleX, 2)) < 0.60) {
          enes.println("The OSV moves foward.");
          digitalWrite(motorLeftInput1, LOW);
          digitalWrite(motorLeftInput2, HIGH);
          digitalWrite(motorRightInput1, LOW);
          digitalWrite(motorRightInput2, HIGH);

          delay(checkTime);

          if(!enes.updateLocation()) {
            enes.println("The OSV can't find its location.");
          }
        }

        while(!irLeft.decode(&resultsLeft) && !irRight.decode(&resultsRight)) {
          enes.println("The OSV turns right.");
          digitalWrite(motorLeftInput1, LOW);
          digitalWrite(motorLeftInput2, HIGH);
          digitalWrite(motorRightInput1, HIGH);
          digitalWrite(motorRightInput2, LOW);

          delay(checkTime);

          if(!enes.updateLocation()){
            enes.println("The OSV can't find its location.");
          }
        }
      }
      
      if(!digitalRead(crashMiddlePin) == LOW) {
        enes.println("The OSV will circumnavigate the obstacle on the left side.");
        while(sqrt(pow(enes.location.x - obstacleX, 2) + pow(enes.location.y - obstacleY, 2)) < 0.20) {
          enes.println("The OSV moves backwards.");
          digitalWrite(motorLeftInput1, HIGH);
          digitalWrite(motorLeftInput2, LOW);
          digitalWrite(motorRightInput1, HIGH);
          digitalWrite(motorRightInput2, LOW);

          delay(checkTime);

          if(!enes.updateLocation()){
            enes.println("The OSV can't find its location.");
          }
        }
        
        while(abs(enes.location.theta - obstacleTheta) < PI/2) {
          enes.println("The OSV turns left.");
          digitalWrite(motorLeftInput1, HIGH);
          digitalWrite(motorLeftInput2, LOW);
          digitalWrite(motorRightInput1, LOW);
          digitalWrite(motorRightInput2, HIGH);

          delay(checkTime);

          if(!enes.updateLocation()){
            enes.println("The OSV can't find its location.");
          }
        }

        obstacleX = enes.location.x;
        obstacleY = enes.location.y;

        while(sqrt(pow(enes.location.x - obstacleX, 2) + pow(enes.location.x - obstacleX, 2)) < 0.20) {
          enes.println("The OSV moves foward.");
          digitalWrite(motorLeftInput1, LOW);
          digitalWrite(motorLeftInput2, HIGH);
          digitalWrite(motorRightInput1, LOW);
          digitalWrite(motorRightInput2, HIGH);

          delay(checkTime);

          if(!enes.updateLocation()) {
            enes.println("The OSV can't find its location.");
          }
        }

        while(!irLeft.decode(&resultsLeft) && !irRight.decode(&resultsRight)) {
          enes.println("The OSV turns right.");
          digitalWrite(motorLeftInput1, LOW);
          digitalWrite(motorLeftInput2, HIGH);
          digitalWrite(motorRightInput1, HIGH);
          digitalWrite(motorRightInput2, LOW);

          delay(checkTime);

          if(!enes.updateLocation()){
            enes.println("The OSV can't find its location.");
          }
        }
      }
    }
    
    if(!digitalRead(crashLeftPin) == HIGH && !digitalRead(crashRightPin) == HIGH) {
      //go to the center of the arena
    }
    
    if(!digitalRead(crashMiddlePin) == HIGH && !digitalRead(crashLeftPin) == LOW && !digitalRead(crashRightPin) == LOW) {
      analogWrite(motorLeftEnable, 0);
      analogWrite(motorLeftEnable, 0);
      enes.println("The OSV has found the Black Box.");
      enes.navigated();
      Coordinate blackbox(enes.location.x, enes.location.y);
      enes.baseObjective(blackbox);
    }

    enes.println("The OSV has navigated the obstacle and re-entered phase 2.");
    phase = 2;
  }
}
