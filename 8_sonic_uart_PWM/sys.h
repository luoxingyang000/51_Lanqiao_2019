#include"stc15.h"

sbit BUZZ=P0^6;
sbit RELAY=P0^4;

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

void pwm_set(uint freq,uchar duty);

void buzz(bit i);
void relay(bit i);