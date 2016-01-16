#include "Arduino.h"

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

// nRF24L01(+) radio attached using Getting Started board
RF24 radio(9, 10);
RF24Network network(radio);
const uint16_t this_node = 1;  //设置本机ID
const uint16_t other_node = 0;

struct send_a //发送
{
  uint32_t node_ms;   //节点运行时间
};

struct receive_a  //接收
{
  uint32_t ms;
  uint16_t rf_CH0;
  uint16_t rf_CH1;
  uint16_t rf_CH2;
  uint16_t rf_CH3;
  uint16_t rf_CH4;
  uint16_t rf_CH5;
  uint16_t rf_CH6;
  uint16_t rf_CH7;
};

boolean nRF(int16_t * _channal_0, int16_t * _channal_1, int16_t * _channal_2, int16_t * _channal_3, int16_t * _channal_4, int16_t * _channal_5, int16_t * _channal_6, int16_t * _channal_7)
{
  network.update();
  // Is there anything ready for us?
  while ( network.available() )
  {
    RF24NetworkHeader header;
    receive_a rec;
    network.read(header, &rec, sizeof(rec));

    int16_t * _i = _channal_0;
    _i[0] = rec.rf_CH0;
    _i = _channal_1;
    _i[0] = rec.rf_CH1;
    _i = _channal_2;
    _i[0] = rec.rf_CH2;
    _i = _channal_3;
    _i[0] = rec.rf_CH3;
    _i = _channal_4;
    _i[0] = rec.rf_CH4;
    _i = _channal_5;
    _i[0] = rec.rf_CH5;
    _i = _channal_6;
    _i[0] = rec.rf_CH6;
    _i = _channal_7;
    _i[0] = rec.rf_CH7;

    {
      //Serial.print("Sending...");
      send_a sen = { millis() };  //把这些数据发送出去，对应前面的发送数组
      RF24NetworkHeader header(other_node);
      bool ok = network.write(header, &sen, sizeof(sen));
      return (ok ? true : false);
    }
  }
}
