typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;

#include"stc15.h"
#include"ds1302.h"
#include"iic.h"

void led_set(ulong numShow);
void ledscan();
void keyscan();

void time_read();
void time_set();
void time_show();

uchar eeprom_read(uchar add);
void eeprom_write(uchar add,uchar dat);

void lock(bit open);
void buzz(bit open);