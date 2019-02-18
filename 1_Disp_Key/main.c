/*2019���ű�_1����ܺ;������*/
/*Ŀ��ʵ�֣��������ʾ������̻�õ�����*/

#include"main.h"

uint t0Cnt; //��ʱ��t0����
uchar MKEY;  //�������״̬�Ĵ���
uchar keyGetNum[2]; //�������

uchar ledBuff[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};  //�������ʾ����
uchar ledChar[11]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xfd};    //��������ܶ����10��-


//main������
void main()
{
    P2=0xa0;P0=0;P2=0;  //�ر�����
    MKEY=0xff; //MKEY�Ĵ�����ʼ��

    TMOD=0;  //����ʱ��t0����Ϊģʽ0 16λ�Զ�����
    TH0=0xfc;TL0=0x66;  //1ms@11.0592MHz
    TR0=1;ET0=1;EA=1;   //ʹ��t0���ж�

    while(1);
}


//led�����ɨ��
void ledscan()
{
    static uint i;

    P2=0xe0;P0=0xff;P2=0;   //����
    P2=0xc0;P0=1<<i;P2=0;   //λѡ
    P2=0xe0;P0=ledBuff[i];P2=0; //����

    if(i<7)
        i++;
    else
        i=0;
}


//led���������
void led_set(ulong numShow)
{
    uint i;
    uchar buf[8];

    for(i=7;i>0;i--)
    {
        buf[i]=numShow%10;
        numShow/=10;
    }

    for(i=0;i<8;i++)
    {
        if(buf[i]==0)
        {
            ledBuff[i]=0xff;
        }else
        {
            break;
        }
    }

    for(;i<8;i++)
    {
        ledBuff[i]=ledChar[buf[i]];
    }
}


//key����ɨ��
void keyscan()
{
    
}


//key��ȡ������
void key_get()
{
    //static uint keyCnt,tDelta;

}


void t0Server() interrupt 1 //T0:1ms
{
    static uint keyCnt;
    static ulong cntCnt;

    if(keyCnt==10)  //ÿ10ms�������ɨ��
    {
        keyCnt=0;
        keyscan();
    }else{keyCnt++;}

    if(cntCnt==99999999)    //�����
    {
        cntCnt=0;
    }else
    {
        led_set(cntCnt);
        cntCnt++;
    }
    
    ledscan();
}