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

//nRF
#include "nRF.h"
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

#define MAX_THROTTLE 255 //最大油门 100~255
#define MAX_STEERING 200 //最大转向 100~512

int16_t channal_data[8];
int16_t throttle = 0;
int16_t steering = 0;

unsigned long safe_ms = millis();

void setup()
{
  Serial.begin(115200);
  Serial.println("Microduino!");

#if defined(__AVR_ATmega32U4__) || (__AVR_ATmega1284P__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega128RFA1__)
  MotorLeft.Fix(1);
  MotorRight.Fix(1);
#else
  MotorLeft.Fix(-1);
  MotorRight.Fix(-1);
#endif

  SPI.begin();		//初始化SPI总线
  radio.begin();
  network.begin(/*channel*/ 70, /*node address*/ this_node);

  Serial.println("nRF!");
}

void loop()
{
  if (nRF(&channal_data[0], &channal_data[1], &channal_data[2], &channal_data[3], &channal_data[4], &channal_data[5], &channal_data[6], &channal_data[7]))
  {
    safe_ms = millis();

    throttle = map(channal_data[1], 1000, 2000, -MAX_THROTTLE, MAX_THROTTLE);
    steering = map(channal_data[0], 1000, 2000, -MAX_STEERING, MAX_STEERING);

    Serial.print(throttle);
    Serial.print(",");
    Serial.println(steering);

    MotorLeft.Driver(MotorLeft.GetData(throttle, steering, CHAN_LEFT));
    MotorRight.Driver(MotorRight.GetData(throttle, steering, CHAN_RIGHT));
  }

  if (safe_ms > millis()) safe_ms = millis();
  if (millis() - safe_ms > 1000)
  {
    //MotorLeft.Free();
    //MotorRight.Free();
    MotorLeft.Brake();
    MotorRight.Brake();
  }
}
