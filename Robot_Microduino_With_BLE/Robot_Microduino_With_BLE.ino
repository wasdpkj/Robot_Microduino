#include "def.h"

//Motor
#include <Microduino_Motor.h>

#if defined(__AVR_ATmega32U4__) || (__AVR_ATmega1284P__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega128RFA1__)
#define motor_pin0A 8  //PWM
#define motor_pin0B 6
#define motor_pin1A 7  //PWM 
#define motor_pin1B 5
#else
#define motor_pin0A 6  //PWM
#define motor_pin0B 8
#define motor_pin1A 5  //PWM
#define motor_pin1B 7
#endif

Motor MotorLeft(motor_pin0A, motor_pin0B);
Motor MotorRight(motor_pin1A, motor_pin1B);

#include <SoftwareSerial.h>
#include "BLE.h"

//nRF
#include "nRF.h"
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

#define SAFE_TIME_OUT 250

#define MAX_THROTTLE 255 //最大油门 100~255
#define MAX_STEERING 512 //最大转向 100~512

boolean Mode = 0;
int16_t channal_data[8];
int16_t throttle = 0;
int16_t steering = 0;

unsigned long safe_ms = millis();

void setup()
{
  Serial.begin(38400);
  Serial.println("Microduino!");

  SPI.begin();		//初始化SPI总线
  radio.begin();
  if (! radio.setDataRate(RF24_250KBPS))
  {
    Mode = 0;
    BLE.begin(9600);
    delay(200);
    Serial.println("BLE!");
    for (int a = 0; a < 3; a++)
    {
      BLE.println("AT+BAUD2");
      delay(200);
    }
    BLE.begin(38400);
  }
  else
  {
    Mode = 1;
    network.begin(/*channel*/ 70, /*node address*/ this_node);
    Serial.println("nRF!");
  }

#if defined(__AVR_ATmega32U4__) || (__AVR_ATmega1284P__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega128RFA1__)
  MotorLeft.Fix(1);
  MotorRight.Fix(1);
#else
  MotorLeft.Fix(-1);
  MotorRight.Fix(-1);
#endif
}

unsigned long _Error_num=0;
void loop()
{
  boolean _Error = true;
  if (Mode)
    _Error = !nRF(&channal_data[0], &channal_data[1], &channal_data[2], &channal_data[3], &channal_data[4], &channal_data[5], &channal_data[6], &channal_data[7]);
  else {
    if (Available()) {
      int8_t _Protocol = Protocol(TYPE_NUM, CHANNEL_NUM);
      if (!_Protocol) {
#ifdef _DEBUG
        DEBUG.println("\n\r \t \t DATA ERROR");
#endif
        _Error = true;
        _Error_num=0;
      }
      else if (_Protocol != -1) {
        read_data(&channal_data[0], &channal_data[1], &channal_data[2], &channal_data[3], &channal_data[4], &channal_data[5], &channal_data[6], &channal_data[7]);
#ifdef _DEBUG
        DEBUG.println("\n\r \t DATA OK");
#endif
        _Error = false;
        _Error_num++;
        DEBUG.print("\t\t\t");
        DEBUG.println(_Error_num);
      }
    }
  }

  if (!_Error)
  {
    safe_ms = millis();

    throttle = map(channal_data[1], 1000, 2000, -MAX_THROTTLE, MAX_THROTTLE);
    steering = map(channal_data[0], 1000, 2000, -MAX_STEERING, MAX_STEERING);

#ifdef _DEBUG
    Serial.print(throttle);
    Serial.print(",");
    Serial.println(steering);
#endif

    MotorLeft.Driver(MotorLeft.GetData(throttle, steering, CHAN_LEFT));
    MotorRight.Driver(MotorRight.GetData(throttle, steering, CHAN_RIGHT));
  }

  if (safe_ms > millis()) safe_ms = millis();
  if (millis() - safe_ms > SAFE_TIME_OUT)
  {
    MotorLeft.Free();
    MotorRight.Free();
    //MotorLeft.Brake();
    //MotorRight.Brake();
  }
}
