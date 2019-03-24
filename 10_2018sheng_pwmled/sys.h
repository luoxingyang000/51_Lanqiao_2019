#include"stc15.h"
#include"intrins.h"
#include"iic.h"

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;

sbit RELAY=P0^4;
sbit BUZZ=P0^6;

void fourkeyscan();
void disp_lightlevel();
void disp_flowinterval();
void ledscan();
void relayopt(bit i);
void buzzopt(bit i);

uchar read_volt();
uchar read_interval();
void write_interval(uchar dat);