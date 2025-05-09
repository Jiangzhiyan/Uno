//
// Created by Jiang Zhiyan on 25-5-9.
//

#include <Arduino.h>
#include <Servo.h>

Servo base, rArm, fArm, claw;

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
}

void loop()
{

	for (int pos = 0; pos <= 180; pos++)
	{
		base.write(pos);
		delay(10);
	}
	for (int pos = 180; pos >= 0; pos--)
	{
		base.write(pos);
		delay(10);
	}
	base.write(90);
	for (int pos = 90; pos <= 180; pos++)
	{
		rArm.write(pos);
		delay(10);
	}
	for (int pos = 180; pos >= 90; pos--)
	{
		rArm.write(pos);
		delay(10);
	}
	rArm.write(90);
	for (int pos = 81; pos <= 180; pos++)
	{
		claw.write(pos);
		delay(10);
	}

	for (int pos = 180; pos >= 81; pos--)
	{
		claw.write(pos);
		delay(10);
	}
	claw.write(81);
	for (int pos = 0; pos <= 117; pos++)
	{
		fArm.write(pos);
		delay(10);
	}
	for (int pos = 117; pos >= 0; pos--)
	{
		fArm.write(pos);
		delay(10);
	}
	fArm.write(90);


	delay(1000);
}
