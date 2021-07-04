//in UTF-8
#include<Servo.h>

//超声雷达测距器输入输出引脚
const int TrigPin = 2;
const int EchoPin = 3;
//舵机输入引脚
const int motorSignalPin = 7;
//蜂鸣器输入引脚
const int BuzzerPin = 8;
//雷达旋转参数设置
const int startingAngle = 15;
const int minimumAngle = 15;
const int maximumAngle = 165;
const int rotationSpeed = 1;

unsigned long Time_Echo_us = 0;
Servo motor;

void setup(void) 
{
    motor.attach(motorSignalPin);
    //初始化串口
    Serial.begin(9600);
    pinMode(BuzzerPin, OUTPUT);
}
void loop(void)
{
    static int motorAngle = startingAngle;
    static int motorRotateAmount = rotationSpeed;
    motor.write(motorAngle);
    delay(300);
    SerialOutput(motorAngle, CalculateDistance());
    //旋转舵机，带动超声测距
    motorAngle += motorRotateAmount;
    if(motorAngle <= minimumAngle || motorAngle >= maximumAngle) {
        motorRotateAmount = -motorRotateAmount;
    }
}
//通过高电平时间测距
int CalculateDistance(void)
{
    float distance;
    pinMode(TrigPin, OUTPUT);
    pinMode(EchoPin, INPUT);
    digitalWrite(TrigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(TrigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(TrigPin, LOW);
    Time_Echo_us = pulseIn(EchoPin, HIGH);
    if ((Time_Echo_us < 60000) && (Time_Echo_us > 1)) {
        distance = Time_Echo_us / 58.00;
        Serial.print("distance is :");
        Serial.print(distance);
        Serial.print("cm");
        Serial.println();
    }
    //距离小于12cm时，蜂鸣器发出响声
    if(Time_Echo_us < 696) digitalWrite(BuzzerPin, HIGH);
    else digitalWrite(BuzzerPin, LOW);
    return int(distance);
}
//串口输出
void SerialOutput(const int angle, const int distance)
{
    String angleString = String(angle);
    String distanceString = String(distance);
    Serial.println(angleString + "," + distanceString);
}
