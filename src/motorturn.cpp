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

void move(int action, int time, int speed)
{
    // 动作类型定义：
    // 0 - forward
    // 1 - backward
    // 2 - turn_left
    // 3 - turn_right
    // 4 - pivot_left
    // 5 - pivot_right
    // 6 - brake

    // 先全部关闭，避免冲突
    analogWrite(Left_motor_go, 0);
    analogWrite(Left_motor_back, 0);
    analogWrite(Right_motor_go, 0);
    analogWrite(Right_motor_back, 0);

    if (action == 0) { // forward
        analogWrite(Left_motor_go, speed);
        digitalWrite(Left_motor_back, LOW);
        analogWrite(Right_motor_go, speed);
        digitalWrite(Right_motor_back, LOW);
    }
    else if (action == 1) { // backward
        digitalWrite(Left_motor_go, LOW);
        analogWrite(Left_motor_back, speed);
        digitalWrite(Right_motor_go, LOW);
        analogWrite(Right_motor_back, speed);
    }
    else if (action == 2) { // turn_left
        analogWrite(Left_motor_back, speed);
        analogWrite(Right_motor_go, speed);
    }
    else if (action == 3) { // turn_right
        analogWrite(Left_motor_go, speed);
        analogWrite(Right_motor_back, speed);
    }
    else if (action == 4) { // pivot_left
        analogWrite(Right_motor_go, speed);
    }
    else if (action == 5) { // pivot_right
        analogWrite(Left_motor_go, speed);
    }
    else if (action == 6) { // brake
        // 全部停下
    }

    delay(time * 100);
}

void loop()
{
	//
}
