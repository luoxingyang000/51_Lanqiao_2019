#include"stc15.h"
#include"intrins.h"

sbit BUZZ=P0^6;
sbit RELAY=P0^4;
sbit PWMbit=P1^4;
sbit SONIC_TX=P1^0;
sbit SONIC_RX=P1^1;

#define somenop {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;

void led_set(ulong num);
void ledscan();
void keyscan();
void sysModeSwitch();

void uart_init();
void uart_sendbyte(uchar dat);
void uart_sendstring(uchar *str);

void sonic_sendWave();

void buzz(bit i);
void relay(bit i);