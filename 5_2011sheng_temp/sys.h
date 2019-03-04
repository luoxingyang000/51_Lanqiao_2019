#include"stc15.h"
#include"iic.h"
#include"onewire.h"

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;

void led_set(ulong num);
void ledscan();
void keyscan();

uchar eeprom_read(uchar add);
void eeprom_write(uchar add,uchar dat);
uchar ds18b20_read();

void display_refresh();