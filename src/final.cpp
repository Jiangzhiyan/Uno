//
// Created by Jiang Zhiyan on 25-5-9.
// 第34组
//

#include <Arduino.h>
#include <Servo.h>

// ---------------- Servo Signal Pin Definitions ----------------
// Servo signal pins
const int basePin = 11;
const int rArmPin = 5;
const int fArmPin = 3;
const int clawPin = 6;

Servo baseServo;
Servo rArmServo;
Servo fArmServo;
Servo clawServo;

// Step delay for smooth movement (ms)
const int stepDelay = 20;
const int clawDelay = 50;

// ---------------- Pick and Place Position Constants ----------------
// 定义三次抓取与放置过程中的各关节目标角度
// Pick and place positions for transfer 1
const int basePick1 = 115;
const int fArmPick1 = 65;
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
// ---------------- Motor Control Pin Definitions ----------------
// 定义电机前进、后退控制引脚
// 左电机前进控制引脚（连接到 IN2）
int Left_motor_go = A5;
// 右电机前进控制引脚（连接到 IN3）
int Right_motor_go = A4;
// 左电机后退控制引脚（连接到 IN1）
int Left_motor_back = 8;
// 右电机后退控制引脚（连接到 IN4）
int Right_motor_back = 12;
// ---------------- Tracking and Obstacle Sensor Pin Definitions ----------------
// 定义循迹与避障传感器输入引脚
// 左循迹接收引脚 (连接到 OUT1)
const int Left_tracking = A2;
// 右循迹接收引脚 (连接到 OUT2)
const int Right_tracking = A3;
// 左避障接收引脚 (连接到 OUT4)
const int SensorLeft_2 = A1;
// 右避障接收引脚 (连接到 OUT3)
const int SensorRight_2 = A0;


 /**
  * 平滑控制函数 slowWrite
  * 通过微小步进延时实现舵机平滑转动到目标位置
  *
  * @param s       目标舵机对象
  * @param target  目标角度
  * @param stepDelay 每步延时(ms)
  */
void slowWrite(Servo& s, int target, int stepDelay)
{
	int current = s.read();
	int step = (current < target) ? 1 : -1;
	for (int pos = current; pos != target; pos += step)
	{
		s.write(pos);
		delay(stepDelay);
	}
	s.write(target);
}

/**
 * 单通道手动PWM输出
 * 根据 duty 生成 0~2000us 脉宽用于控制普通电机
 */
void manualPWM(int pin, int duty)
{
	int highTime = map(duty, 0, 255, 0, 2000);
	digitalWrite(pin, HIGH);
	delayMicroseconds(highTime);
	digitalWrite(pin, LOW);
	delayMicroseconds(2000 - highTime);
}

/**
 * 双通道同步手动PWM输出
 * 在同一个20ms周期内对两个引脚分别输出独立占空比的PWM
 */
void manualPWMTwo(int pin1, int pin2, int duty1, int duty2)
{
	int high1 = map(duty1, 0, 255, 0, 2000);
	int high2 = map(duty2, 0, 255, 0, 2000);
	// Start both high
	digitalWrite(pin1, HIGH);
	digitalWrite(pin2, HIGH);
	if (high1 < high2)
	{
		delayMicroseconds(high1);
		digitalWrite(pin1, LOW);
		delayMicroseconds(high2 - high1);
		digitalWrite(pin2, LOW);
		delayMicroseconds(2000 - high2);
	}
	else
	{
		delayMicroseconds(high2);
		digitalWrite(pin2, LOW);
		delayMicroseconds(high1 - high2);
		digitalWrite(pin1, LOW);
		delayMicroseconds(2000 - high1);
	}
}

/**
 * Arduino 初始化
 * - 设置所有电机及传感器引脚模式
 * - 初始化四个舵机并启动串口
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
	pinMode(basePin, OUTPUT);
	pinMode(rArmPin, OUTPUT);
	pinMode(fArmPin, OUTPUT);
	pinMode(clawPin, OUTPUT);

	baseServo.attach(basePin);
	rArmServo.attach(rArmPin);
	fArmServo.attach(fArmPin);
	clawServo.attach(clawPin);

	Serial.begin(9600);
}

/**
 * 小车运动控制接口 move
 * 根据 action 参数执行不同运动模式，左右轮使用手动PWM控制速度
 *
 * @param action       运动类型（0前进 1后退 2左转 3右转 4绕左轮右转 5绕右轮左转 6刹车）
 * @param speed_left   左电机速度 (0~255)
 * @param speed_right  右电机速度 (0~255)
 */
void move(int action, int speed_left, int speed_right)
{
	// 先全部关闭，避免冲突
	// analogWrite(Left_motor_go, 0);
	digitalWrite(Left_motor_go, LOW);
	digitalWrite(Left_motor_back, LOW);
	// analogWrite(Right_motor_go, 0);
	digitalWrite(Right_motor_go, LOW);
	digitalWrite(Right_motor_back, LOW);

	// 前进: 左右电机同步前行
	if (action == 0)
	{ // forward with independent, synchronized PWM
		manualPWMTwo(Left_motor_go, Right_motor_go, speed_left, speed_right);
		digitalWrite(Left_motor_back, LOW);
		digitalWrite(Right_motor_back, LOW);
	}
	// else if (action == 1)
	// { // backward
	// 	analogWrite(Left_motor_go, 0);
	// 	digitalWrite(Left_motor_back, HIGH);
	// 	analogWrite(Right_motor_go, 0);
	// 	digitalWrite(Right_motor_back, HIGH);
	// }
	// 原地左转: 左轮后退, 右轮前进
	else if (action == 2)
	{
		// left_backward
		// analogWrite(Left_motor_go, 0);
		digitalWrite(Left_motor_go, LOW);
		digitalWrite(Left_motor_back, HIGH);
		// analogWrite(Right_motor_go, 0);
		digitalWrite(Right_motor_go, LOW);
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
	// 刹车: 停止所有电机
	else if (action == 6)
	{
		// 停止刹车
	}
}

/**
 * 主循环 loop
 * 持续执行：
 * 1. 读取循迹与避障传感器
 * 2. 障碍物检测并执行避障逻辑
 * 3. 根据循迹传感器值执行线路跟踪
 * 4. 遇障后触发机械臂抓取流程
 */
void loop()
{
	// 读取左右循迹传感器状态
	int leftVal = digitalRead(Left_tracking);
	int rightVal = digitalRead(Right_tracking);

	// 读取左右避障传感器状态
	int obsLeft = digitalRead(SensorLeft_2);
	int obsRight = digitalRead(SensorRight_2);
	// 同时检测到障碍物，启动避障与抓取流程
	if (obsLeft == LOW && obsRight == LOW)
	{
		move(6, 0, 0);
		delay(500);


		for (int i = 0; i < 180; i++)
		{
			move(0, 80, 1);
		}
		for (int pos = 0; pos <= 180; pos++)
		{
			baseServo.write(pos);
			delay(10);
		}
		for (int pos = 180; pos >= 0; pos--)
		{
			baseServo.write(pos);
			delay(10);
		}
		baseServo.write(90);
		for (int pos = 0; pos <= 117; pos++)
		{
			fArmServo.write(pos);
			delay(10);
		}
		for (int pos = 117; pos >= 0; pos--)
		{
			fArmServo.write(pos);
			delay(10);
		}
		fArmServo.write(0);
		for (int pos = 90; pos <= 180; pos++)
		{
			rArmServo.write(pos);
			delay(10);
		}
		for (int pos = 180; pos >= 90; pos--)
		{
			rArmServo.write(pos);
			delay(10);
		}
		rArmServo.write(90);
		delay(500);
		fArmServo.write(90);
		for (int pos = 90; pos <= 130; pos++)
		{
			clawServo.write(pos);
			delay(10);
		}
		for (int pos = 130; pos >= 90; pos--)
		{
			clawServo.write(pos);
			delay(10);
		}
		clawServo.write(90);


		baseServo.write(90);
		fArmServo.write(90);
		rArmServo.write(90);
		clawServo.write(90);


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

	// 避障并完成三次抓取与放置后进入永久暂停
	while (true)
	{
		delay(1000);
	}
	}
	// 正常循迹逻辑，根据传感器调整方向
	else
	{
		if (leftVal == LOW && rightVal == LOW)
		{
			// 两个传感器都在白色区域，直行
			move(0, 108, 90);
		}
		else if (leftVal == LOW && rightVal == HIGH)
		{
			// 左侧探测到白线，右转调整
			move(0, 40, 120);
			//delay(3);
		}
		else if (leftVal == HIGH && rightVal == LOW)
		{
			// 右侧探测到白线，左转调整
			move(0, 120, 40);
			//delay(3);
		}
	}
}
