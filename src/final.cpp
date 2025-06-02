/**
 * @file final.cpp
 * @author Jiang Zhiyan
 * @date 2025-5-9
 * @brief 第34组 - 智能小车循迹避障与机械臂抓取控制系统
 * 
 * 本程序实现了智能小车的循迹、避障和机械臂抓取功能：
 * - 循迹功能：通过两个循迹传感器实现沿黑线行驶
 * - 避障功能：通过两个避障传感器检测前方障碍物
 * - 机械臂控制：包含四个舵机的协调控制，完成三次抓取放置任务
 * - 电机控制：采用手动PWM方式控制左右两个直流电机
 */

#include <Arduino.h>
#include <Servo.h>

// ==================== 舵机控制相关定义 ====================
// 舵机信号引脚定义
constexpr int basePin = 11;  // 底座舵机引脚
constexpr int rArmPin = 5;   // 后臂舵机引脚
constexpr int fArmPin = 3;   // 前臂舵机引脚
constexpr int clawPin = 6;   // 爪子舵机引脚

// 舵机对象声明
Servo baseServo;  // 底座舵机
Servo rArmServo;  // 后臂舵机
Servo fArmServo;  // 前臂舵机
Servo clawServo;  // 爪子舵机

// 舵机平滑运动延时参数（毫秒）
constexpr int stepDelay = 20;  // 舵机每步移动的延时
constexpr int clawDelay = 20;  // 爪子舵机的延时

// ==================== 机械臂位置参数定义 ====================
/**
 * 定义三次抓取与放置过程中的各关节目标角度
 * 每组参数包含：底座角度、前臂角度、后臂角度、爪子角度
 * Pick：抓取位置参数
 * Place：放置位置参数
 */
// ---------- 第一次抓取放置参数 ----------
constexpr int basePick1 = 125;
constexpr int fArmPick1 = 70;
constexpr int rArmPick1 = 150;
constexpr int clawPick1 = 95;
constexpr int basePlace1 = 20;
constexpr int fArmPlace1 = 20;
constexpr int rArmPlace1 = 170;
constexpr int clawPlace1 = 130;

// ---------- 第二次抓取放置参数 ----------
constexpr int basePick2 = 158;
constexpr int fArmPick2 = 70;
constexpr int rArmPick2 = 150;
constexpr int clawPick2 = 103;
constexpr int basePlace2 = 45;
constexpr int fArmPlace2 = 20;
constexpr int rArmPlace2 = 165;
constexpr int clawPlace2 = 120;

// ---------- 第三次抓取放置参数 ----------
constexpr int basePick3 = 179;
constexpr int fArmPick3 = 60;
constexpr int rArmPick3 = 160;
constexpr int clawPick3 = 103;
constexpr int basePlace3 = 70;
constexpr int fArmPlace3 = 10;
constexpr int rArmPlace3 = 179;
constexpr int clawPlace3 = 130;

// ==================== 硬件连接说明 ====================
/**
 * 传感器连接说明：
 * - A0: 右避障传感器 (连接OUT3)
 * - A1: 左避障传感器 (连接OUT4)
 * - A2: 左循迹传感器 (连接OUT1)
 * - A3: 右循迹传感器 (连接OUT2)
 * 
 * 电机驱动连接说明：
 * - 8:  左电机后退 (连接IN1)
 * - A5: 左电机前进 (连接IN2)
 * - A4: 右电机前进 (连接IN3)
 * - 12: 右电机后退 (连接IN4)
 */

// ==================== 电机控制引脚定义 ====================
// 电机控制引脚
int Left_motor_go = A5;     // 左电机前进控制引脚（PWM）
int Right_motor_go = A4;    // 右电机前进控制引脚（PWM）
int Left_motor_back = 8;    // 左电机后退控制引脚
int Right_motor_back = 12;  // 右电机后退控制引脚

// ==================== 传感器引脚定义 ====================
// 循迹传感器引脚（检测黑线）
constexpr int Left_tracking = A2;   // 左侧循迹传感器
constexpr int Right_tracking = A3;  // 右侧循迹传感器

// 避障传感器引脚（检测障碍物）
constexpr int SensorLeft_2 = A1;   // 左侧避障传感器
constexpr int SensorRight_2 = A0;  // 右侧避障传感器


// ==================== 函数定义 ====================
/**
 * @brief 舵机平滑控制函数
 * 
 * 通过逐步调整角度实现舵机平滑转动，避免机械冲击
 * 根据当前位置和目标位置自动判断转动方向
 *
 * @param s         目标舵机对象引用
 * @param target    目标角度（0-180度）
 * @param stepDelay 每步延时时间（毫秒）
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
 * @brief 单通道手动PWM输出函数
 * 
 * 由于硬件PWM引脚被舵机占用，采用软件模拟PWM
 * 产生周期为2ms的PWM信号控制电机速度
 * 
 * @param pin  输出引脚
 * @param duty 占空比（0-255，对应0-100%）
 */
void manualPWM(int pin, int duty)
{
	int highTime = static_cast<int>(map(duty, 0, 255, 0, 2000));
	digitalWrite(pin, HIGH);
	delayMicroseconds(highTime);
	digitalWrite(pin, LOW);
	delayMicroseconds(2000 - highTime);
}

/**
 * @brief 双通道同步手动PWM输出函数
 * 
 * 在同一个2ms周期内同时控制两个电机的PWM
 * 优化了时序，减少两个电机控制信号的相位差
 * 
 * @param pin1  第一个输出引脚（左电机）
 * @param pin2  第二个输出引脚（右电机）
 * @param duty1 第一个引脚占空比（0-255）
 * @param duty2 第二个引脚占空比（0-255）
 */
void manualPWMTwo(int pin1, int pin2, int duty1, int duty2)
{
	int high1 = static_cast<int>(map(duty1, 0, 255, 0, 2000));
	int high2 = static_cast<int>(map(duty2, 0, 255, 0, 2000));
	// 同时拉高两个引脚
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
 * @brief Arduino初始化函数
 * 
 * 系统启动时执行一次，完成以下初始化：
 * 1. 配置电机控制引脚为输出模式
 * 2. 配置传感器引脚为输入模式
 * 3. 配置舵机控制引脚并附加舵机对象
 * 4. 初始化串口通信（9600波特率）
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
 * @brief 小车运动控制函数
 * 
 * 控制小车的各种运动状态，支持差速驱动
 * 使用手动PWM实现电机速度控制
 *
 * @param action      运动模式:
 *                    0 - 前进（可差速）
 *                    1 - 后退（未实现）
 *                    2 - 原地左转
 *                    3 - 原地右转（未实现）
 *                    4 - 绕左轮转向（未实现）
 *                    5 - 绕右轮转向（未实现）
 *                    6 - 刹车停止
 * @param speed_left  左电机速度 (0-255)
 * @param speed_right 右电机速度 (0-255)
 */
void move(int action, int speed_left, int speed_right)
{
	// 首先关闭所有电机输出，避免短路
	// analogWrite(Left_motor_go, 0);
	digitalWrite(Left_motor_go, LOW);
	digitalWrite(Left_motor_back, LOW);
	// analogWrite(Right_motor_go, 0);
	digitalWrite(Right_motor_go, LOW);
	digitalWrite(Right_motor_back, LOW);

	// 模式0：前进（支持差速控制）
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
	// 模式2：原地左转（仅左轮后退）.
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
	// 模式6：刹车（所有引脚已在开始时置低）.
	else if (action == 6)
	{
		// 刹车状态：所有电机停止
	}
}

/**
 * @brief 主循环函数
 * 
 * 系统主要工作流程：
 * 1. 持续读取传感器状态
 * 2. 优先处理避障逻辑
 * 3. 正常情况下执行循迹逻辑
 * 4. 检测到障碍物时执行完整的机械臂操作序列
 * 
 * 传感器逻辑：
 * - 循迹传感器：LOW表示检测到白色，HIGH表示检测到黑线
 * - 避障传感器：LOW表示检测到障碍物，HIGH表示无障碍
 */
void loop()
{
	// ========== 传感器状态读取 ==========
	int leftVal = digitalRead(Left_tracking);
	int rightVal = digitalRead(Right_tracking);

	// 读取避障传感器（LOW=有障碍，HIGH=无障碍）
	int obsLeft = digitalRead(SensorLeft_2);
	int obsRight = digitalRead(SensorRight_2);
	// ========== 避障与机械臂控制逻辑 ==========
	// 当两个避障传感器同时检测到障碍物时
	if (obsLeft == LOW && obsRight == LOW)
	{
		// 立即停车
		move(6, 0, 0);
		delay(500);

		// ========== 舵机校准序列 ==========
		// 依次测试每个舵机的运动范围，确保机械臂正常工作
		
		// 1. 底座舵机校准（0-180度往返）
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
		baseServo.write(90);  // 底座回中
		
		// 2. 前臂舵机校准（0-117度往返）
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
		fArmServo.write(0);  // 前臂归位
		
		// 3. 后臂舵机校准（90-180度往返）
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
		rArmServo.write(90);  // 后臂回中
		delay(500);
		
		// 4. 爪子舵机校准（90-130度往返）
		slowWrite(fArmServo, 90, stepDelay);
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
		clawServo.write(90);  // 爪子回中

		// ========== 机械臂初始化位置 ==========
		// 将所有舵机移动到准备位置

		slowWrite(baseServo, 90, stepDelay);
		slowWrite(fArmServo, 90, stepDelay);
		slowWrite(rArmServo, 90, stepDelay);
		delay(100);
		slowWrite(clawServo, 140, clawDelay);  // 打开爪子

		// ========== 第一次抓取放置操作 ==========
		// 步骤1：打开爪子准备抓取
		delay(200);
		// 步骤2：移动到抓取位置
		slowWrite(baseServo, basePick1, stepDelay);
		slowWrite(fArmServo, fArmPick1, stepDelay);
		delay(100);
		slowWrite(rArmServo, rArmPick1, stepDelay);
		slowWrite(clawServo, clawPick1, clawDelay);
		delay(200);
		// 步骤3：抬起后臂（提升物体）
		slowWrite(rArmServo, rArmPick1 - 20, stepDelay);
		// 步骤4：移动到放置位置
		slowWrite(baseServo, basePlace1, stepDelay);
		slowWrite(fArmServo, fArmPlace1, stepDelay);
		slowWrite(rArmServo, rArmPlace1, stepDelay);
		delay(200);
		// 步骤5：打开爪子释放物体
		slowWrite(clawServo, 140, clawDelay);
		// 步骤6：复位到中间位置
		slowWrite(rArmServo, 90, stepDelay);
		slowWrite(baseServo, 90, stepDelay);
		slowWrite(clawServo, 140, clawDelay);

		// ========== 第二次抓取放置操作 ==========
		// 步骤2：移动到抓取位置
		slowWrite(baseServo, basePick2, stepDelay);
		slowWrite(fArmServo, fArmPick2, stepDelay);
		slowWrite(rArmServo, rArmPick2, stepDelay);
		slowWrite(clawServo, clawPick2, clawDelay);
		delay(200);
		// 步骤3：抬起后臂（提升物体）
		slowWrite(rArmServo, rArmPick2 - 20, stepDelay);
		// 步骤4：移动到放置位置
		slowWrite(baseServo, basePlace2, stepDelay);
		slowWrite(fArmServo, fArmPlace2, stepDelay);
		slowWrite(rArmServo, rArmPlace2, stepDelay);
		delay(200);
		// 步骤5：打开爪子释放物体
		slowWrite(clawServo, 140, clawDelay);
		// 步骤6：复位到中间位置
		slowWrite(rArmServo, 90, stepDelay);
		slowWrite(baseServo, 90, stepDelay);
		slowWrite(clawServo, 130, clawDelay);
		
		// ========== 第三次抓取放置操作 ==========
		// 步骤1：打开爪子
		slowWrite(clawServo, 140, clawDelay);
		// 步骤2：移动到抓取位置
		slowWrite(baseServo, basePick3, stepDelay);
		slowWrite(fArmServo, fArmPick3, stepDelay);
		slowWrite(rArmServo, rArmPick3, stepDelay);
		slowWrite(clawServo, clawPick3, clawDelay);
		delay(200);
		// 步骤3：抬起后臂（提升物体）
		slowWrite(rArmServo, rArmPick3 - 20, stepDelay);
		// 步骤4：移动到放置位置
		slowWrite(baseServo, basePlace3, stepDelay);
		slowWrite(fArmServo, fArmPlace3, stepDelay);
		slowWrite(rArmServo, rArmPlace3, stepDelay);
		delay(200);
		// 步骤5：打开爪子释放物体
		slowWrite(clawServo, 140, clawDelay);
		// 步骤6：复位到中间位置

		slowWrite(rArmServo, 90, stepDelay);
		slowWrite(baseServo, 90, stepDelay);
		slowWrite(clawServo, 130, clawDelay);

		// ========== 任务完成，系统停止 ==========
		// 完成所有抓取任务后进入无限循环
		// 此处使用无限循环是为了避免程序意外重启或继续执行
		while (true)
		{
			delay(1000);
			// 任务完成，保持停止状态
		}
	}
	// ========== 循迹控制逻辑 ==========
	// 无障碍物时执行循迹行驶
	else
	{
		if (leftVal == LOW && rightVal == LOW)
		{
			// 情况1：两侧都检测到白色，保持直行
			move(0, 88, 70);
		}
		else if (leftVal == LOW && rightVal == HIGH)
		{
			// 情况2：右侧检测到黑线，需要右转修正
			move(0, 40, 120);
			// delay(3);
		}
		else if (leftVal == HIGH && rightVal == LOW)
		{
			// 情况3：左侧检测到黑线，需要左转修正
			move(0, 120, 40);
			// delay(3);
		}
	}
}
