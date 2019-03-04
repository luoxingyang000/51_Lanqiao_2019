#include"stc15.h"
#include"iic.h"

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;

void ledscan();
void led_set(ulong num);
void keyscan();

uchar eeprom_read(uchar add);
void eeprom_write(uchar add,uchar dat);
uchar adc_read(uchar ain);

void sysStatusChange();