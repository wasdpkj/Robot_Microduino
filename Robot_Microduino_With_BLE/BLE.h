#include "arduino.h"
//======================
/*
  [head,2byte,0xAA 0xBB] [type,1byte,TYPE_NUM] [data,16byte] [body,1byte,getChecksum()]
  Example:
  AA BB C8 DC 05 DC 05 D0 07 EF 03 DC 05 DC 05 DC 05 DC 05 E3
*/

//-------------------------
#define _DEBUG
//#define _DEBUG_HEX

#include <SoftwareSerial.h>
SoftwareSerial mySerial(4, 5); // RX, TX#define _DEBUG

#define BLE Serial1
//#define BLE mySerial
#define DEBUG Serial

#define BUFFER_MAX 256
#define CHANNEL_NUM 8
#define TYPE_NUM 0xC8

int16_t RC_data[8];
int type;

byte getChecksum(byte length, byte cmd, byte mydata[])
{
  //三个参数分别为： 数据长度  ，  指令代码  ，  实际数据数组
  byte checksum = 0;
  checksum ^= (length & 0xFF);
  checksum ^= (cmd & 0xFF);
  for (int i = 0; i < length; i++)
  {
    checksum ^= (mydata[i] & 0xFF);
  }
  return checksum;
}

byte inChar, inCache;
byte buffer[BUFFER_MAX];
unsigned long num = 0;
boolean Available()
{
  if (BLE.available() > 0) {
    inCache = inChar;
    inChar = BLE.read();

    buffer[num] = inChar;
#ifdef _DEBUG
    //DEBUG.print(inCache,HEX);
    //DEBUG.print(inChar,HEX);
#endif
    if (num > BUFFER_MAX - 1)  {
      num = 0;
      return false;
    }
    else
      num++;
    delayMicroseconds(200);
    return true;
  }
  return false;
}

boolean sta = false;
boolean error = false;
int8_t Protocol(byte _channel, uint8_t _num)
{
  if (sta) {
    sta = false;
    num = 0;
    if (inChar == _channel) {
      error = false;
      type = 1;
#ifdef _DEBUG
      DEBUG.print("\n\rType[");
      DEBUG.print(_channel, HEX);
      DEBUG.println("]");
#endif
      return -1;  //2
    }
    else  {
      error = true;
      type = 0;
      return 0;
    }
  }

  if (inChar == 0xBB && inCache == 0xAA) {
    sta = true;
#ifdef _DEBUG
    DEBUG.println("\n\r");
    DEBUG.println("\n\r\t---START");
#endif
    return -1;  //1
  }

  if (num  == (_num * 2 + 1) && !error) {
    inCache = buffer[_num * 2];
    buffer[_num * 2] = NULL;
    inChar = getChecksum(_num * 2, 200, buffer);

#ifdef _DEBUG
    DEBUG.print("Sum[");
    DEBUG.print(inChar, HEX);
    DEBUG.print("],");
    DEBUG.print("Num[");
    DEBUG.print(_num * 2);
    DEBUG.print("]");
#ifdef _DEBUG_HEX
    DEBUG.print(":");
    for (long a = 0; a < num; a++) {
      DEBUG.print(buffer[a], HEX);
      DEBUG.print(" ");
    }
#endif
    DEBUG.println(" ");
#endif

    num = 0;
    if (!error && inCache == inChar) {
#ifdef _DEBUG
      DEBUG.println("Checksum OK");
#endif
      //      read_data(_num * 2, buffer);
      ///      delay(8);
#ifdef _DEBUG
      DEBUG.println("\n\r\t---END");
#endif
      return 1;
    }
    else
      return 0;
  }
  else
    return -1;
}


//=============================
int p;
static uint8_t read8(byte* _Buf)  {
  return (uint8_t)_Buf[p++] & 0xff;
}

static uint16_t read16(byte* _Buf) {
  uint16_t t = read8(_Buf);
  t += (uint16_t)read8(_Buf) << 8;
  return t;
}

static uint32_t read32(byte* _Buf) {
  uint32_t t = read16(_Buf);
  t += (uint32_t)read16(_Buf) << 16;
  return t;
}
/*
  int p;
  int16_t read16(byte* _Buf) {
  byte _Buf1 = _Buf[p++];
  byte _Buf2 = _Buf[p++];
  int16_t r = (_Buf1 & 0xff) | ((_Buf2 << 8) & 0xff00);
  return r;
  }
*/

void read_data(int16_t * _channal_0, int16_t * _channal_1, int16_t * _channal_2, int16_t * _channal_3, int16_t * _channal_4, int16_t * _channal_5, int16_t * _channal_6, int16_t * _channal_7)
{
  p = 0;
  uint8_t _num = CHANNEL_NUM;

  int16_t _bufin[_num];

  int16_t * _i = _channal_0;
  _i[0] = read16(buffer);
  _i = _channal_1;
  _i[0] = read16(buffer);
  _i = _channal_2;
  _i[0] = read16(buffer);
  _i = _channal_3;
  _i[0] = read16(buffer);
  _i = _channal_4;
  _i[0] = read16(buffer);
  _i = _channal_5;
  _i[0] = read16(buffer);
  _i = _channal_6;
  _i[0] = read16(buffer);
  _i = _channal_7;
  _i[0] = read16(buffer);
  //    _bufin[i] = read16(buffer);
}
