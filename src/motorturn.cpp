//
// Created by Jiang Zhiyan on 25-4-17.
//

#include <Arduino.h>

// 左电机前进控制引脚（连接到 D9）
int Left_motor_go=9;
// 右电机前进控制引脚（连接到 D10）
int Right_motor_go=10;

// 左电机后退控制引脚（连接到 D8）
int Left_motor_back=8;
// 右电机后退控制引脚（连接到 D11）
int Right_motor_back=11;

/**
 * 设置所有电机控制引脚为输出模式
 */
void setup()
{

    pinMode(Left_motor_go,OUTPUT);
    pinMode(Right_motor_go,OUTPUT);
    pinMode(Left_motor_back,OUTPUT);
    pinMode(Right_motor_back,OUTPUT);
}

/**
 * 控制电机运动的统一接口函数。
 * 可根据 action 参数决定前进、后退、旋转或刹车。
 *
 * @param action 动作类型：
 *               0 - 前进
 *               1 - 后退
 *               2 - 原地左转（左退右进）
 *               3 - 原地右转（左进右退）
 *               4 - 绕左轮右转（只动右轮）
 *               5 - 绕右轮左转（只动左轮）
 *               6 - 刹车（停止）
 * @param time 动作持续时间，单位为 100 毫秒。例如 time = 5 表示 500ms。
 * @param speed_left 电机PWM速度（0~255）
 * @param speed_right 电机PWM速度（0~255）
 */
void move(int action, int speed_left, int speed_right)
{
    // 先全部关闭，避免冲突
    analogWrite(Left_motor_go, 0);
    analogWrite(Left_motor_back, LOW);
    analogWrite(Right_motor_go, 0);
    analogWrite(Right_motor_back, LOW);

    if (action == 0) { // forward
        analogWrite(Left_motor_go, speed_left);
        digitalWrite(Left_motor_back, LOW);
        analogWrite(Right_motor_go, speed_right);
        digitalWrite(Right_motor_back, LOW);
    }
    // else if (action == 1) { // backward
    //     digitalWrite(Left_motor_go, LOW);
    //     analogWrite(Left_motor_back, speed_left);
    //     digitalWrite(Right_motor_go, LOW);
    //     analogWrite(Right_motor_back, speed_right);
    // }
    // else if (action == 2) { // turn_left
    //     analogWrite(Left_motor_back, speed_left);
    //     analogWrite(Right_motor_go, speed_right);
    // }
    // else if (action == 3) { // turn_right
    //     analogWrite(Left_motor_go, speed_left);
    //     analogWrite(Right_motor_back, speed_right);
    // }
    // else if (action == 4) { // pivot_left
    //     analogWrite(Right_motor_go, speed_right);
    // }
    // else if (action == 5) { // pivot_right
    //     analogWrite(Left_motor_go, speed_left);
    // }
    else if (action == 6) { // brake
        // 全部停下
    }
}

void loop()
{
    // 主循环函数（此处为空，需根据实际需要填入动作指令）
    // Arduino 会不断重复执行 loop() 内的指令
}
