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
const int basePick1 = 115;
const int fArmPick1 = 70;
const int rArmPick1 = 150;
const int clawPick1 = 102;
const int basePlace1 = 5;
const int fArmPlace1 = 20;
const int rArmPlace1 = 170;
const int clawPlace1 = 130;

// Pick and place positions for transfer 2
const int basePick2 = 145;
const int fArmPick2 = 70;
const int rArmPick2 = 150;
const int clawPick2 = 105;
const int basePlace2 = 30;
const int fArmPlace2 = 40;
const int rArmPlace2 = 155;
const int clawPlace2 = 120;

// Pick and place positions for transfer 3
const int basePick3 = 170;
const int fArmPick3 = 50;
const int rArmPick3 = 160;
const int clawPick3 = 103;
const int basePlace3 = 53;
const int fArmPlace3 = 50;
const int rArmPlace3 = 160;
const int clawPlace3 = 130;

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
	// 1. Open claw.
	slowWrite(clawServo, 130, clawDelay);
	delay(200);
	// 2. Move to pick
	slowWrite(baseServo, basePick1, stepDelay);
	slowWrite(fArmServo, fArmPick1, stepDelay);
	slowWrite(rArmServo, rArmPick1, stepDelay);
	slowWrite(clawServo, clawPick1, clawDelay);
	delay(200);
	// 3. Lift fArm
	slowWrite(rArmServo, rArmPick1 - 20, stepDelay);
	// 4. Move to place
	slowWrite(baseServo, basePlace1, stepDelay);
	slowWrite(fArmServo, fArmPlace1, stepDelay);
	slowWrite(rArmServo, rArmPlace1, stepDelay);
	delay(200);
	// 5. Open claw to release
	slowWrite(clawServo, 130, clawDelay);
	// 6. Reset all to 90°
	slowWrite(rArmServo, 90, stepDelay);
	slowWrite(baseServo, 90, stepDelay);
	slowWrite(clawServo, 130, clawDelay);

	// Transfer 2
	// 2. Move to pick.
	slowWrite(baseServo, basePick2, stepDelay);
	slowWrite(fArmServo, fArmPick2, stepDelay);
	slowWrite(rArmServo, rArmPick2, stepDelay);
	slowWrite(clawServo, clawPick2, clawDelay);
	delay(200);
	// 3. Lift fArm
	slowWrite(rArmServo, rArmPick2 - 20, stepDelay);
	// 4. Move to place
	slowWrite(baseServo, basePlace2, stepDelay);
	slowWrite(fArmServo, fArmPlace2, stepDelay);
	slowWrite(rArmServo, rArmPlace2, stepDelay);
	delay(200);
	// 5. Open claw to release
	slowWrite(clawServo, 130, clawDelay);
	// 6. Reset all to 90°
	slowWrite(rArmServo, 90, stepDelay);
	slowWrite(baseServo, 90, stepDelay);
	slowWrite(clawServo, 130, clawDelay);
	// Transfer 3
	// 1. Open claw.
	slowWrite(clawServo, 130, clawDelay);
	// 2. Move to pick
	slowWrite(baseServo, basePick3, stepDelay);
	slowWrite(fArmServo, fArmPick3, stepDelay);
	slowWrite(rArmServo, rArmPick3, stepDelay);
	slowWrite(clawServo, clawPick3, clawDelay);
	delay(200);
	// 3. Lift fArm
	slowWrite(rArmServo, rArmPick3 - 20, stepDelay);
	// 4. Move to place
	slowWrite(baseServo, basePlace3, stepDelay);
	slowWrite(fArmServo, fArmPlace3, stepDelay);
	slowWrite(rArmServo, rArmPlace3, stepDelay);
	delay(200);
	// 5. Open claw to release
	slowWrite(clawServo, 130, clawDelay);
	// 6. Reset all to 90°

	slowWrite(rArmServo, 90, stepDelay);
	slowWrite(baseServo, 90, stepDelay);
	slowWrite(clawServo, 130, clawDelay);

	// Pause forever
	while (true)
	{
		delay(1000);
	}
}
