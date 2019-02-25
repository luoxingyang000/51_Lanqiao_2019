#include"stc15f2k60s2.h"
#include"ds1302.h"
#include"iic.h"
#include"onewire.h"

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;

void ledscan();
void led_set(ulong number);
void keyscan();
