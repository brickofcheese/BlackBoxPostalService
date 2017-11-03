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
int obstacleX;
int obstacleY;

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
  //In phase #1, the OSV tries to escape the landing zone and traverse the rough terrain.
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

  //In phase #2, the OSV searches for and moves towards the Black Box. If the OSV hits an obstacle, it enters phase #3. 
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

    if(digitalRead(crashLeftPin) == HIGH || digitalRead(crashMiddlePin) == HIGH || digitalRead(crashRightPin) == HIGH) {
      enes.println("The OSV hit something and entered phase #3.");
      phase = 3;
    }

    if(!enes.updateLocation()) {
      enes.println("The OSV can't find its location.");
    }
  }

  //In phase #3, the OSV decides what to when it hits something. It then follows a preset list of instructions to go around an obstacle.
  while(phase == 3) {
    \\This is where we decide what to do when we hit something
    if(!enes.updateLocation()) {
      enes.println("The OSV can't find its location.");
    }

    obstacleX = enes.location.x;
    obstalceY = enes.location.y;
    obstalceTheta = enes.location.theta;
    
    if(digitalRead(crashLeftPin) == HIGH && digitalRead(crashRightPin) == LOW) {
      if(digitalRead(crashMiddlePin) == HIGH){
        enes.print("The OSV will circumnavigate the obstacle on the right side.");
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

        while(sqrt(pow(enes.location.x - obstacleX, 2) + pow(enes.location.x - obstacleX, 2))) < 0.60) {
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

        while(infraredLeft == LOW && infraredRight == LOW) {
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
      if(digitalRead(crashMiddlePin) == LOW){
        enes.print("The OSV will circumnavigate the obstacle on the right side.");
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

        while(sqrt(pow(enes.location.x - obstacleX, 2) + pow(enes.location.x - obstacleX, 2))) < 0.20) {
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

        while(infraredLeft == LOW && infraredRight == LOW) {
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
    
    if(digitalRead(crashLeftPin) == LOW && digitalRead(crashRightPin) == HIGH) {
      if(digitalRead(crashMiddlePin) == HIGH) {
        enes.print("The OSV will circumnavigate the obstacle on the left side.");
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

        while(sqrt(pow(enes.location.x - obstacleX, 2) + pow(enes.location.x - obstacleX, 2))) < 0.60) {
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

        while(infraredLeft == LOW && infraredRight == LOW) {
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
      
      if(digitalRead(crashMiddlePin) == LOW) {
        enes.print("The OSV will circumnavigate the obstacle on the left side.");
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

        while(sqrt(pow(enes.location.x - obstacleX, 2) + pow(enes.location.x - obstacleX, 2))) < 0.20) {
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

        while(infraredLeft == LOW && infraredRight == LOW) {
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
    
    if(digitalRead(crashLeftPin) == HIGH && digitalRead(crashRightPin) == HIGH) {
      //go to the center of the arena
    }
    
    if(digitalRead(crashMiddlePin) == HIGH && digitalRead(crashLeftPin) == LOW && digitalRead(crashRightPin) == LOW) {
      analogWrite(motorLeftEnable, 0);
      analogWrite(motorLeftEnable, 0);
      enes.println("The OSV has found the Black Box.");
      enes.navigated():
      enes.baseObjective(Coordinate(enes.location.x, enes.location.y));
    }

    enes.println("The OSV has navigated the obstacle and re-entered phase #2.");
    phase = 2;
  }
}
