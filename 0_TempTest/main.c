/*ULuo only for temporary testing and the code will be changed anytime*/

#include"stc15f2k60s2.h"

unsigned int t0Cnt,t1Cnt;
unsigned char P0buff0,P0buff1;

void main()
{
    unsigned int i,j;

    //�ر�����
    P2=0xa0;P0=0;P2=0;

    TMOD=0; //���ö�ʱ��T0��T1Ϊģʽ0
    TH0=0xfc;TL0=0x66;  //1ms@11.0592MHz
    TH1=0x4c;TL0=0; //50ms@11.0592MHz
    TF0=0;TF1=0;  //���T0T1�����־
    TR0=1;TR1=1;ET0=1;ET1=1;EA=0;   //��ʱ�����ж�ʹ��
	
	P0buff0=P0buff1=0xff;

    while(1)
    {
        {
            for(i=0;i<100;i++)
            {for(j=0;j<100;j++);}
        }
        P0buff0=~P0buff0;
        P0=P0buff0;
    }
}


void t0Server() interrupt 1 //T0
{

}

void t1Server() interrupt 3 //T1
{

    
}