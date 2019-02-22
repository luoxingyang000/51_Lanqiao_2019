/*2019���ű�_1����ܺ;������*/
/*Ŀ��ʵ�֣��������ʾ������̻�õ�����*/

#include"main.h"

sbit KL1=P3^0;sbit KL2=P3^1;sbit KL3=P3^2;sbit KL4=P3^3;
sbit KR1=P4^4;sbit KR2=P4^2;sbit KR3=P3^5;sbit KR4=P3^4;    //�������IO����

uint t0Cnt; //��ʱ��t0����
bit isTimer;    //�������ʹ��
uint keyVal[2]={0,0};  //�������ֵ�볤�̰�״̬

uchar ledBuff[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};  //�������ʾ����
uchar ledChar[11]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xfd};    //��������ܶ����10��-


//main������
void main()
{
    P2=0xa0;P0=0;P2=0;  //�ر�����

    TMOD=0;  //����ʱ��t0����Ϊģʽ0 16λ�Զ�����
    TH0=0xfc;TL0=0x66;  //1ms@11.0592MHz
    TR0=1;ET0=1;EA=1;   //ʹ��t0���ж�

    isTimer=1;  //�������ʹ��

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
    static uchar keyState=0;  //����״̬��
    static uchar keyNow,keyLast;
    uchar kl,kr;    //��ʱ���м�ֵ

    //������ɨ��
    KR1=KR2=KR3=KR4=0; KL1=KL2=KL3=KL4=1;
    if(KL1==0){kl=1;}
    else if(KL2==0){kl=2;}
    else if(KL3==0){kl=3;}
    else if(KL4==0){kl=4;}
    else{kl=5;}

    //������ɨ��
    KL1=KL2=KL3=KL4=0; KR1=KR2=KR3=KR4=1;
    if(KR1==0){kr=1;}
    else if(KR2==0){kr=2;}
    else if(KR3==0){kr=3;}
    else if(KR4==0){kr=4;}
    else{kr=5;}

    keyNow=kl*10+kr;    //����ɨ���ֵ

    //�������״̬��
    switch(keyState)
    {
        case 0: //����
        {
            if(keyNow!=keyLast) {keyState=1;}  //����
        }break;

        case 1: //����
        {
            if(keyNow==keyLast) {keyState=2;}
            else {keyState=0;}  //��Ϊ��
        }break; 

        case 2: //������
        {
            if(keyNow==keyLast) {keyState=3;}
            else
            {
                keyVal[0]=keyNow;keyVal[1]=0;   //���ض̰�ֵ
                keyState=0;
            }
        }break;

        case 3: //�жϳ��̰�
        {
            if(keyNow==keyLast)
            {
                static uchar keyCnt;
                if(keyCnt++ > 80)  //0.8s
                {
                    keyVal[0]=keyNow;keyVal[1]=1;   //���س���ֵ
                    keyState=4; keyCnt=0;
                }
            }
            else
            {
                keyVal[0]=keyNow;keyVal[1]=0;   //���ض̰�ֵ
                keyState=0;
            }
        }break;

        case 4: //��������
        {
            if(keyNow!=keyLast) {keyState=0;}
        }break;

        default:keyState=0;break;
    }

    keyLast=keyNow; //���¼�ֵ
}


void t0Server() interrupt 1 //T0:1ms
{
    static uint keyTimeCnt;
    static ulong cntCnt;

    //ÿ10ms�������ɨ��
    if(++keyTimeCnt==10)  
    {
        keyTimeCnt=0;
        keyscan();
    }

    //�����
    if(isTimer==1)
    {
        if(cntCnt==99999999)    
        {
            cntCnt=0;
        }else
        {
            led_set(cntCnt);
            cntCnt++;
        }
    }

    //���������Ӧ����
    if(keyVal[0]==22)
    {
        switch(keyVal[1])
        {
            case 0:
            {
                if(isTimer==0){isTimer=1;} 
                else{isTimer=0;}
            }break;

            case 1:
            {
                cntCnt=0; led_set(1); isTimer=1;
            }break;

            default:break;
        }
        keyVal[0]=keyVal[1]=0;  //������λ
    }
    ledscan();
}