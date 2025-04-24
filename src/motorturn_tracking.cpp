//
// Created by Jiang Zhiyan on 25-4-22.
//

#include <Arduino.h>

/**
 * A0  引脚连接OUT3
 * A1  引脚连接OUT4
 * A2  引脚连接OUT1
 * A3  引脚连接OUT2
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
// 左循迹接收引脚 (连接到 OUT1)
const int Left_tracking = A2;
// 右循迹接收引脚 (连接到 OUT2)
const int Right_tracking = A3;
// 左避障接收引脚 (连接到 OUT4)
const int SensorLeft_2 = A1;
// 右避障接收引脚 (连接到 OUT3)
const int SensorRight_2 = A0;

/**
 * 设置所有电机控制引脚为输出模式
 */
void setup()
{
	pinMode(Left_motor_go, OUTPUT);
	pinMode(Right_motor_go, OUTPUT);
	pinMode(Left_motor_back, OUTPUT);
	pinMode(Right_motor_back, OUTPUT);
	pinMode(Left_tracking, INPUT);
	pinMode(Right_tracking, INPUT);
	pinMode(SensorLeft_2, INPUT);
	pinMode(SensorRight_2, INPUT);
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
	else if (action == 1)
	{ // backward
		analogWrite(Left_motor_go, 0);
		digitalWrite(Left_motor_back, HIGH);
		analogWrite(Right_motor_go, 0);
		digitalWrite(Right_motor_back, HIGH);
	}
	else if (action == 2)
	{
		analogWrite(Left_motor_go, 0);
		digitalWrite(Left_motor_back, HIGH);
		analogWrite(Right_motor_go, 254);
		digitalWrite(Right_motor_back, LOW);
	}
	// else if (action == 3) {// turn_right
	//     analogWrite(Left_motor_go, speed_left);
	//     analogWrite(Right_motor_back, speed_right);
	// }
	// else if (action == 4) {// pivot_left
	//     analogWrite(Right_motor_go, speed_right);
	// }
	// else if (action == 5) {// pivot_right
	//     analogWrite(Left_motor_go, speed_left);
	// }
	else if (action == 6)
	{
		// 停止刹车
	}
}

void loop()
{
	// 读取避障传感器状态
	int obsLeft = digitalRead(SensorLeft_2);
	int obsRight = digitalRead(SensorRight_2);
	if (obsLeft == LOW && obsRight == LOW)
	{
		// 前后都检测到障碍，后退并停止
		move(2, 0, 0);
		delay(236);
		move(6, 0, 0);
	}
	else if (obsLeft == LOW && obsRight == HIGH)
	{
		// 左侧检测到障碍，右转
		move(0, 200, 0);
	}
	else if (obsLeft == HIGH && obsRight == LOW)
	{
		// 右侧检测到障碍，左转
		move(0, 0, 200);
	}
	else
	{
		// 追迹逻辑
		int leftVal = digitalRead(Left_tracking);
		int rightVal = digitalRead(Right_tracking);

		if (leftVal == LOW && rightVal == LOW)
		{
			// 两个传感器都在白色区域，直行
			move(0, 168, 160);
		}
		else if (leftVal == LOW && rightVal == HIGH)
		{
			// 左侧探测到白线，右转调整
			move(0, 40, 120);
			delay(3);
		}
		else if (leftVal == HIGH && rightVal == LOW)
		{
			// 右侧探测到白线，左转调整
			move(0, 120, 40);
			delay(3);
		}
	}
}
