//
// Created by Jiang Zhiyan on 25-5-13.
//

#include <Arduino.h>
#include <Servo.h>

Servo baseServo;
Servo rArmServo;
Servo fArmServo;
Servo clawServo;

// Servo signal pins
const int basePin = 11;
const int rArmPin = 5;
const int fArmPin = 3;
const int clawPin = 6;


// Step delay for smooth movement (ms)
const int stepDelay = 20;
const int clawDelay = 50;

// Pick and place positions for transfer 1
const int basePick1 = 120;
const int fArmPick1 = 60;
const int rArmPick1 = 45;
const int clawPick1 = 180;
const int basePlace1 = 210; // basePick1 + 90 example
const int fArmPlace1 = fArmPick1;
const int rArmPlace1 = rArmPick1;
const int clawPlace1 = 180;

// Pick and place positions for transfer 2
const int basePick2 = 100;
const int fArmPick2 = 70;
const int rArmPick2 = 55;
const int clawPick2 = 180;
const int basePlace2 = 190;
const int fArmPlace2 = fArmPick2;
const int rArmPlace2 = rArmPick2;
const int clawPlace2 = 180;

// Pick and place positions for transfer 3
const int basePick3 = 140;
const int fArmPick3 = 50;
const int rArmPick3 = 65;
const int clawPick3 = 180;
const int basePlace3 = 230;
const int fArmPlace3 = fArmPick3;
const int rArmPlace3 = rArmPick3;
const int clawPlace3 = 180;

void setup()
{
	baseServo.attach(basePin);
	rArmServo.attach(rArmPin);
	fArmServo.attach(fArmPin);
	clawServo.attach(clawPin);
	baseServo.write(90);
	fArmServo.write(90);
	rArmServo.write(90);
	clawServo.write(90);
	Serial.begin(9600);
}

void slowWrite(Servo &s, int target, int stepDelay) {
	int current = s.read();
	int step = (current < target) ? 1 : -1;
	for (int pos = current; pos != target; pos += step) {
		s.write(pos);
		delay(stepDelay);
	}
	s.write(target);
}

void loop()
{
	// Transfer 1
	// 1. Open claw
	slowWrite(clawServo, 180, clawDelay);
	// 2. Move to pick
	slowWrite(baseServo, basePick1, stepDelay);
	slowWrite(fArmServo, fArmPick1, stepDelay);
	slowWrite(rArmServo, rArmPick1, stepDelay);
	slowWrite(clawServo, clawPick1, clawDelay);
	// 3. Lift fArm
	slowWrite(fArmServo, fArmPick1 + 10, stepDelay);
	// 4. Move to place
	slowWrite(baseServo, basePlace1, stepDelay);
	slowWrite(fArmServo, fArmPlace1, stepDelay);
	// 5. Open claw to release
	slowWrite(clawServo, 180, clawDelay);
	// 6. Reset all to 90°
	slowWrite(fArmServo, 90, stepDelay);
	slowWrite(rArmServo, 90, stepDelay);
	slowWrite(baseServo, 90, stepDelay);
	slowWrite(clawServo, 180, clawDelay);

	// Transfer 2
	// 2. Move to pick
	slowWrite(baseServo, basePick2, stepDelay);
	slowWrite(fArmServo, fArmPick2, stepDelay);
	slowWrite(rArmServo, rArmPick2, stepDelay);
	slowWrite(clawServo, clawPick2, clawDelay);
	// 3. Lift fArm
	slowWrite(fArmServo, fArmPick2 + 10, stepDelay);
	// 4. Move to place
	slowWrite(baseServo, basePlace2, stepDelay);
	slowWrite(fArmServo, fArmPlace2, stepDelay);
	// 5. Open claw to release
	slowWrite(clawServo, 180, clawDelay);
	// 6. Reset all to 90°
	slowWrite(fArmServo, 90, stepDelay);
	slowWrite(rArmServo, 90, stepDelay);
	slowWrite(baseServo, 90, stepDelay);
	slowWrite(clawServo, 180, clawDelay);
	// Transfer 3
	// 1. Open claw
	slowWrite(clawServo, clawPick3, clawDelay);
	// 2. Move to pick
	slowWrite(baseServo, basePick3, stepDelay);
	slowWrite(fArmServo, fArmPick3, stepDelay);
	slowWrite(rArmServo, rArmPick3, stepDelay);
	// 3. Lift fArm
	slowWrite(fArmServo, fArmPick3 + 10, stepDelay);
	// 4. Move to place
	slowWrite(baseServo, basePlace3, stepDelay);
	slowWrite(fArmServo, fArmPlace3, stepDelay);
	// 5. Open claw to release
	slowWrite(clawServo, 180, clawDelay);
	// 6. Reset all to 90°
	slowWrite(fArmServo, 90, stepDelay);
	slowWrite(rArmServo, 90, stepDelay);
	slowWrite(baseServo, 90, stepDelay);
	slowWrite(clawServo, 180, clawDelay);

	// Pause forever
	while (true)
	{
		delay(1000);
	}
}
