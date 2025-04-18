//
// Created by Jiang Zhiyan on 25-4-17.
//

#include <Arduino.h>

/**
 * 8  引脚连接IN1
 * 9  引脚连接IN2
 * 10 引脚连接IN3
 * 12 引脚连接IN4
 */

// 左电机前进控制引脚（连接到 IN2）
int Left_motor_go = 9;
// 右电机前进控制引脚（连接到 IN3）
int Right_motor_go = 10;
// 左电机后退控制引脚（连接到 IN1）
int Left_motor_back = 8;
// 右电机后退控制引脚（连接到 IN4）
int Right_motor_back = 12;

/**
 * 设置所有电机控制引脚为输出模式
 */
void setup()
{

	pinMode(Left_motor_go, OUTPUT);
	pinMode(Right_motor_go, OUTPUT);
	pinMode(Left_motor_back, OUTPUT);
	pinMode(Right_motor_back, OUTPUT);
}

/**
 * 控制电机运动的统一接口函数。
 * 可根据 action 参数决定前进、后退、旋转或刹车。
 *
 * @param action 动作类型：
 *               0 - 前进
 *               1 - 后退
 *               2 - 原地左转（左退右进）
 *               3 - 原地右转（左进右退/00？？
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
	digitalWrite(Left_motor_back, LOW);
	analogWrite(Right_motor_go, 0);
	digitalWrite(Right_motor_back, LOW);

	if (action == 0)
	{ // forward
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
	else if (action == 6)
	{
		// 停止刹车
	}
}

void loop()
{
	// 直行前进，初始进入S形路径
	move(0, 100, 100);
	delay(700);
	// move(6,0,0);
	// delay(500);

	// 左转弯，开始进入S形第一个弯道
	move(0, 120, 0);
	delay(410);

	// 再次直行，穿过S形路径的中间直道
	move(0, 100, 100);
	delay(2800);

	// 右转弯，进入S形路径第二个弯道
	move(0, 0, 120);
	delay(450);

	// 最后直行，完成S形路径
	move(0, 100, 100);
	delay(1200);

	// 停止，动作结束
	move(6, 0, 0);
	delay(1000000);
}
