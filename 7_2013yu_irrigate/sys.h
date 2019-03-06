#include"ds1302.h"
#include"iic.h"
#include"stc15.h"

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;

void ledset_normal();
void ledset_humdityset();
void ledscan();
void keyscan();

void ds1302_read();
uchar eeprom_read(uchar add);
void eeprom_write(uchar add,uchar dat);
uchar pcf8591_read(uchar add);

void buzz(bit i);
void relay(bit i);