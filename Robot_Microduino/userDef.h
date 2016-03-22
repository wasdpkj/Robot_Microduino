#include"arduino.h"

//#define _DEBUG  //DEBUG调试

#define NRF_CHANNEL 70  //nRF通道
#define BLE_SPEED 9600  //蓝牙接口速度

#define SAFE_TIME_OUT 250   //失控保护时间
#define MAX_THROTTLE 255 //最大油门 < 255
#define MAX_STEERING 512 //最大转向 < 512
#define CHANNEL_THROTTLE  2 //油门通道
#define CHANNEL_STEERING  1 //转向通道

#if defined(__AVR_ATmega32U4__) || (__AVR_ATmega1284P__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega128RFA1__)
#define motor_pin0A 8  //PWM
#define motor_pin0B 6
#define motor_pin1A 7  //PWM 
#define motor_pin1B 5
#define motor_fixL 1  //速度修正 -1到1之间
#define motor_fixR 1  //速度修正 -1到1之间
#else
#define motor_pin0A 6  //PWM
#define motor_pin0B 8
#define motor_pin1A 5  //PWM
#define motor_pin1B 7
#define motor_fixL -1 //速度修正 -1到1之间
#define motor_fixR -1 //速度修正 -1到1之间
#endif

