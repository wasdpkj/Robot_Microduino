/*
  =================================
  本作品采用知识共享 署名-非商业性使用-相同方式共享 3.0 未本地化版本 许可协议进行许可
  访问 http://creativecommons.org/licenses/by-nc-sa/3.0/ 查看该许可协议
  版权所有:Kejia Pan ,Email:wasdpkj@hotmail.com / pankejia@microduino.cc
  =================================

  =================================
  包含程序库、硬件支持包的IDE可在Getting Start中下载:
  http://www.microduino.cc/download/

  =================================
  IDE Support：
  https://github.com/wasdpkj/Microduino-IDE-Support/

  =================================
  串口信息（Serial Monitor）,9600 baud,Both NL & CR

  =================================
  所需硬件：
  Microduino USBTLL
  Microduino Core/Core+/CoreRF/CoreUSB
  Microduino Module BLE/nRF
  Microduino Shield Robot

  功能:
  可兼容joypad遥控(nRF)与手机遥控(BLE)
  开机自动识别是否有nRF模块,进入不同模式
*/

#include "userDef.h"
#include "Protocol.h"
#include <Servo.h>
Servo myservo;  // create servo object to control a servo

///////////////////////////////////////////////////////////
#include <Microduino_Motor.h>
Motor MotorLeft(motor_pin0A, motor_pin0B);
Motor MotorRight(motor_pin1A, motor_pin1B);

///////////////////////////////////////////////////////////
uint16_t channalData[CHANNEL_NUM]; //8通道数据
bool mode = 0; //nrf或者ble模式
int16_t throttle = 0; //油门
int16_t steering = 0; //转向
unsigned long safe_ms = millis();

void setup() {
  Serial.begin(9600);
  Serial.println("Hello Microduino!");

  mode = protocolSetup();  //遥控接收器初始化

  MotorLeft.Fix(motor_fixL);
  MotorRight.Fix(motor_fixR);
  myservo.attach(A0);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  if (protocolRead(channalData, mode)) { //判断是否接收到遥控信号
    throttle = map(channalData[CHANNEL_THROTTLE], 1000, 2000, -MAX_THROTTLE, MAX_THROTTLE);
    steering = map(channalData[CHANNEL_STEERING], 1000, 2000, -MAX_STEERING, MAX_STEERING);
    myservo.writeMicroseconds(channalData[3]);

    MotorLeft.Driver(MotorLeft.GetData(throttle, steering, CHAN_LEFT));
    MotorRight.Driver(MotorRight.GetData(throttle, steering, CHAN_RIGHT));

#ifdef _DEBUG
    Serial.print("DATA OK :[");
    for (int a = 0; a < CHANNEL_NUM; a++) {
      Serial.print(channalData[a]);
      Serial.print(" ");
    }
    Serial.print("],throttle:");
    Serial.print(throttle);
    Serial.print(",steering:");
    Serial.println(steering);
#endif
    safe_ms = millis();
  }

  if (safe_ms > millis()) safe_ms = millis();
  if (millis() - safe_ms > SAFE_TIME_OUT) {
    MotorLeft.Free();
    MotorRight.Free();
    myservo.writeMicroseconds(1000);
    //MotorLeft.Brake();
    //MotorRight.Brake();
  }
}
