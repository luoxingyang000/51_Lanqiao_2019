#include"stc15.h"
#include"intrins.h"
#include"ds1302.h"
#include"iic.h"
#include"onewire.h"

sbit BUZZ=P0^6;
sbit RELAY=P0^4;
sbit SONIC_TX=P1^0;
sbit SONIC_RX=P1^1;

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;

void led_set(ulong num);
void ledscan();
void keyscan();
void sysChange();

void uart_init();
void uart_sendbyte(uchar dat);
void uart_sendstring(uchar *str);

void sonic_sendWave();

void buzz(bit i);
void relay(bit i);