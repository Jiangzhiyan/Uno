//
// Created by Jiang Zhiyan on 25-5-9.
//

#include <Arduino.h>
#include <Servo.h>

// Servo objects for pick-and-place
Servo base, rArm, fArm, claw;

// Servo debug step and angle limits
const int step = 5;
const int baseMin = 0,   baseMax = 180;
const int rArmMin = 90,  rArmMax = 180;
const int fArmMin = 0,   fArmMax = 117;
const int clawMin = 81,  clawMax = 180;

// Helper to move a servo slowly to a target angle
void slowWrite(Servo &s, int target, int stepDelay) {
  int current = s.read();
  int step = (current < target) ? 1 : -1;
  for (int pos = current; pos != target; pos += step) {
    s.write(pos);
    delay(stepDelay);
  }
  s.write(target);
}
// Pick-and-place sequence angles (adjust as needed)
const int basePick   = 90;
const int rArmPick   = 90;
const int fArmPick   = 90;
const int clawClosed = 45;
const int baseDrop   = 0;
const int rArmDrop   = 45;
const int fArmDrop   = 45;
const int clawOpen   = 0;
// Perform pick-and-place with slow movements
void pickAndPlace() {
  slowWrite(base, basePick, 20);
  slowWrite(rArm, rArmPick, 20);
  slowWrite(fArm, fArmPick, 20);
  slowWrite(claw, clawClosed, 50);
  delay(500);
  slowWrite(base, baseDrop, 20);
  slowWrite(rArm, rArmDrop, 20);
  slowWrite(fArm, fArmDrop, 20);
  slowWrite(claw, clawOpen, 50);
}



/**
 *  11 Base
 *  5  rArm
 *  3  fArm
 *  6  claw

 */
void setup()
{
	base.attach(11);
	rArm.attach(5);
	fArm.attach(3);
	claw.attach(6);
	claw.write(90);
	Serial.begin(9600);
	Serial.println("Commands: B<angle>, R<angle>, F<angle>, C<angle> to move servos; P to pick and place");
}

void loop()
{
	if (Serial.available() > 0) {
     String cmd = Serial.readStringUntil('\n');
     char id = cmd.charAt(0);
     switch (id) {
       case 'B': case 'b': {
         int val = cmd.substring(1).toInt();
         if (cmd.charAt(1) == '+') {
           val = constrain(base.read() + step, baseMin, baseMax);
         } else if (cmd.charAt(1) == '-') {
           val = constrain(base.read() - step, baseMin, baseMax);
         } else {
           val = constrain(val, baseMin, baseMax);
         }
         slowWrite(base, val, 20);
         Serial.print("Base -> "); Serial.println(val);
         break;
       }
       case 'R': case 'r': {
         int val = cmd.substring(1).toInt();
         if (cmd.charAt(1) == '+') {
           val = constrain(rArm.read() + step, rArmMin, rArmMax);
         } else if (cmd.charAt(1) == '-') {
           val = constrain(rArm.read() - step, rArmMin, rArmMax);
         } else {
           val = constrain(val, rArmMin, rArmMax);
         }
         slowWrite(rArm, val, 20);
         Serial.print("rArm -> "); Serial.println(val);
         break;
       }
       case 'F': case 'f': {
         int val = cmd.substring(1).toInt();
         if (cmd.charAt(1) == '+') {
           val = constrain(fArm.read() + step, fArmMin, fArmMax);
         } else if (cmd.charAt(1) == '-') {
           val = constrain(fArm.read() - step, fArmMin, fArmMax);
         } else {
           val = constrain(val, fArmMin, fArmMax);
         }
         slowWrite(fArm, val, 20);
         Serial.print("fArm -> "); Serial.println(val);
         break;
       }
       case 'C': case 'c': {
         int val = cmd.substring(1).toInt();
         if (cmd.charAt(1) == '+') {
           val = constrain(claw.read() + step, clawMin, clawMax);
         } else if (cmd.charAt(1) == '-') {
           val = constrain(claw.read() - step, clawMin, clawMax);
         } else {
           val = constrain(val, clawMin, clawMax);
         }
         slowWrite(claw, val, 20);
         Serial.print("claw -> "); Serial.println(val);
         break;
       }
       case 'P': case 'p':
         pickAndPlace();
         Serial.println("Completed pick-and-place");
         break;
       default:
         Serial.println("Unknown command");
         break;
     }
     Serial.println("Enter command:");
   }
}
