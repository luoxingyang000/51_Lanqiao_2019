#include"stc15.h"
#include"iic.h"

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;

void disp_set(uint num);
void ledscan();
void keyscan();

uchar read_pcf8591(uchar add);