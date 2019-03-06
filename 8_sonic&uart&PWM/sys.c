#include"sys.h"

uchar ledBuff[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
uchar ledChar[11]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf};

uchar keyVal,sysState;


void main()
{
    P2=0xa0; P0=0; P2=0;
    TMOD=0;
    TH0=0xfc; TL0=0x66;
    TR0=1; ET0=1; EA=1;
    while(1);
}


void t0server() interrupt 1
{
    
}

//////////////////////////////////////////////////

void led_set(ulong num)
{

}


void ledscan()
{

}


void keyscan()
{
    static uchar keyState,keyNow;
    uchar kl,kr;
    //行扫描
}

//////////////////////////////////////////////////

void uart_init()
{

}


void uart_sendbyte(uchar dat)
{

}


void uart_sendstring(uchar *str)
{

}


void uart_get() interrupt 4
{

}

//////////////////////////////////////////////////

void pwm_init()
{

}


void pwm_set(uint cycle,uchar duty)
{

}