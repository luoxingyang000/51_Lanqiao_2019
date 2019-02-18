/*ULuo only for temporary testing and the code will be changed anytime*/

#include"stc15f2k60s2.h"

unsigned int t0Cnt,t1Cnt;
unsigned char P0buff0,P0buff1;

void main()
{
    //�ر�����
    P2=0xa0;P0=0;P2=0;
    //LED��
    P2=0x80;P0=0;P2=0;
    //���������ȫ��
    P2=0xc0;P0=0xff;P2=0;
    P2=0xe0;P0=0;P2=0;

    P0buff0=P0buff1=0;

    TMOD=0; //���ö�ʱ��T0��T1Ϊģʽ0
    TH0=0xfc;TL0=0x66;  //1ms@11.0592MHz
    TH1=0x4c;TL0=0; //50ms@11.0592MHz
    TF0=0;TF1=0;  //���T0T1�����־
    TR0=1;TR1=1;ET0=1;ET1=1;EA=1;   //��ʱ�����ж�ʹ��

    while(1);
}


void t0Server() interrupt 1 //T0
{
    if(t0Cnt==1000) //1s?
    {
        t0Cnt=0;
        P0buff0=~P0buff0;
        P2=0x80;P0=P0buff0;P2=0;
    }else
    {
        t0Cnt++;
    }
}

void t1Server() interrupt 3 //T1
{
    if(t1Cnt==4) //0.2s
    {
        t1Cnt=0;
        P0buff1=~P0buff1;
        P2=0xe0;P0=P0buff1;P2=0;
    }else
    {
        t1Cnt++;
    }
    
}