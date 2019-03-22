#include"sys.h"

sbit PWMbit=P1^4;
sbit SONIC_TX=P1^0;
sbit SONIC_RX=P1^1;

uint pwmFreq,pwmDuty;


void Delay13us()		//@11.0592MHz
{
	unsigned char i;
	_nop_();
	_nop_();
	i = 33;
	while (--i);
}


void sonicTrig()
{
    
}


void main()
{
    P2=(P2&0x1f)|0xa0; P0=0; P2=P2&0x1f;

    TMOD=0;
    TR0=1; ET0=1;
    TR1=0; ET1=0;
    EA=1;

    pwmFreq=1000; pwmDuty=75;

    TL0=(65536-pwmFreq/100)%256;
    TH0=(65536-pwmFreq/100)/256;

    while(1)
    {
        static uint i;
        i++;
        if(i==10000)
        {
            i=0;
            pwmDuty--;
        }
        if(pwmDuty==1) {pwmDuty=99;}
    }

}


void pwmFlip() interrupt 1
{
    static uint t0Cnt;
    t0Cnt++;
    if(t0Cnt==pwmDuty) {PWMbit=0;}
    if(t0Cnt==100) {t0Cnt=0; PWMbit=1;}
}