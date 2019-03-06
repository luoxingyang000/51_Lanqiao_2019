#include"stc15.h"

sbit BUZZ=P0^6;
sbit RELAY=P0^4;

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;

void led_set();
void ledscan();
void keyscan();