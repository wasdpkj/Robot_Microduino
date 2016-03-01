#include "Arduino.h"

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

// nRF24L01(+) radio attached using Getting Started board
RF24 radio(9, 10);
RF24Network network(radio);
const uint16_t this_node = 1;  //设置本机ID
const uint16_t other_node = 0;

struct send_a { //发送
  uint32_t node_ms;   //节点运行时间
};

struct receive_a { //接收
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

boolean nrfParse(uint16_t * _channal) {
  network.update();
  // Is there anything ready for us?
  while ( network.available() ) {
    RF24NetworkHeader header;
    receive_a rec;
    network.read(header, &rec, sizeof(rec));

    _channal[0] = rec.rf_CH0;
    _channal[1] = rec.rf_CH1;
    _channal[2] = rec.rf_CH2;
    _channal[3] = rec.rf_CH3;
    _channal[4] = rec.rf_CH4;
    _channal[5] = rec.rf_CH5;
    _channal[6] = rec.rf_CH6;
    _channal[7] = rec.rf_CH7;

    {
      //Serial.print("Sending...");
      send_a sen = { millis() };  //把这些数据发送出去，对应前面的发送数组
      RF24NetworkHeader header(other_node);
      boolean ok = network.write(header, &sen, sizeof(sen));
      return ok;
    }
  }
}

boolean nrfBegin() {
  SPI.begin();    //初始化SPI总线
  radio.begin();
  if (! radio.setDataRate(RF24_250KBPS))  {
    return false;
  }
  else  {
    network.begin(/*channel*/ NRF_CHANNEL, /*node address*/ this_node);
    return true;
  }
}
