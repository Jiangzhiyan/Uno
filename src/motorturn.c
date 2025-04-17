//
// Created by Jiang Zhiyan on 25-4-17.
//


#include <Arduino.h>

int Left_motor_go=9;
int Right_motor_go=10;

int Left_motor_back=8;
int Right_motor_back=12;

void setup()
{
    pinMode(Left_motor_go,OUTPUT);
    pinMode(Right_motor_go,OUTPUT);
    pinMode(Left_motor_back,OUTPUT);
    pinMode(Right_motor_back,OUTPUT);
}

void run(int time, int speed)
{
    analogWrite(Left_motor_go,speed);
    digitalWrite(Left_motor_back,LOW);
    analogWrite(Right_motor_go,speed);
    digitalWrite(Right_motor_back,LOW);
    delay(time *100);
}
