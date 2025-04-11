#include <Arduino.h>

//Written by 23122609 姜祉延 H103

int ledpin1 = 11;
int inpin1 = 7;
int ledpin2 = 10;
int inpin2 = 6;
int ledpin3 = 9;
int inpin3 = 5;

int val1;
int val2;
int val3;

void setup()
{
    // put your setup code here, to run once:
    pinMode(ledpin1,OUTPUT);
    pinMode(inpin1,INPUT);
    pinMode(ledpin2,OUTPUT);
    pinMode(inpin2,INPUT);
    pinMode(ledpin3,OUTPUT);
    pinMode(inpin3,INPUT);
}

void loop()
{
    // put your main code here, to run repeatedly:
    val1 = digitalRead(inpin1);
    if (val1 == LOW)
        digitalWrite(ledpin1,LOW);
    else
        digitalWrite(ledpin1,HIGH);
    val2 = digitalRead(inpin2);
    if (val2 == LOW)
        digitalWrite(ledpin2,LOW);
    else
        digitalWrite(ledpin2,HIGH);
    val3 = digitalRead(inpin3);
    if (val3 == LOW)
        digitalWrite(ledpin3,LOW);
    else
        digitalWrite(ledpin3,HIGH);
}
