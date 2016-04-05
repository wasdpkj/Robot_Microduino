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
  Microduino Core/Core+/CoreUSB/CoreRF
  Microduino Module BLE/nRF
  Microduino Shield Robot

  功能:
  可兼容joypad遥控(nRF)与手机遥控(BLE)
  开机自动识别是否有nRF模块,进入不同模式
*/

#include "userDef.h"
#include "nrf_Protocol.h"

///////////////////////////////////////////////////////////
#include <Microduino_Motor.h>
Motor MotorLeft(motor_pin0A, motor_pin0B);
Motor MotorRight(motor_pin1A, motor_pin1B);

///////////////////////////////////////////////////////////
#include <Microduino_Protocol_SoftSer.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(4, -1); // RX, TX (D5与电机冲突 屏蔽 只用RX)
Protocol bleProtocol(&mySerial, TYPE_NUM);  //软串口,校验数据类

///////////////////////////////////////////////////////////
boolean Mode = 0; //nrf或者ble模式
uint16_t channal_data[CHANNEL_NUM]; //8通道数据
int16_t throttle = 0; //油门
int16_t steering = 0; //转向

unsigned long safe_ms = millis();

void setup() {
  Serial.begin(9600);
  Serial.println("Hello Microduino!");

  if (nrfBegin()) {
    Mode = true;
  }
  else {
    Mode = false;
    bleProtocol.begin(BLE_SPEED);
  }

  MotorLeft.Fix(motor_fixL);
  MotorRight.Fix(motor_fixR);
}

void loop() {
  boolean _Error = true;
  if (Mode) {
    _Error = !nrfParse(channal_data);
  }
  else {
    switch (bleProtocol.parse(channal_data, MODE_WHILE)) {
      case P_NONE:  //DATA NONE
        break;
      case P_FINE:  //DATA OK
        _Error = false;
        break;
      case P_ERROR: //DATA ERROR
        mySerial.stopListening();
        mySerial.listen();
        break;
      case P_TIMEOUT: //DATA TIMEOUT
        mySerial.stopListening();
        mySerial.listen();
        break;
    }
  }

  if (!_Error) {
    safe_ms = millis();

    throttle = map(channal_data[CHANNEL_THROTTLE - 1], 1000, 2000, -MAX_THROTTLE, MAX_THROTTLE);
    steering = map(channal_data[CHANNEL_STEERING - 1], 1000, 2000, -MAX_STEERING, MAX_STEERING);

    MotorLeft.Driver(MotorLeft.GetData(throttle, steering, CHAN_LEFT));
    MotorRight.Driver(MotorRight.GetData(throttle, steering, CHAN_RIGHT));

#ifdef _DEBUG
    Serial.print(" \t DATA OK :[");
    for (int a = 0; a < CHANNEL_NUM; a++) {
      Serial.print(channal_data[a]);
      Serial.print(" ");
    }
    Serial.print("],throttle:");
    Serial.print(throttle);
    Serial.print(",steering:");
    Serial.println(steering);
#endif
  }

  if (safe_ms > millis()) safe_ms = millis();
  if (millis() - safe_ms > SAFE_TIME_OUT) {
    MotorLeft.Free();
    MotorRight.Free();
    //MotorLeft.Brake();
    //MotorRight.Brake();
  }
}
