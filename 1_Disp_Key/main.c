/*2019蓝桥杯_1数码管和矩阵键盘*/
/*目标实现：数码管显示秒并可启停和重置*/

#include"main.h"

sbit KL1=P3^0;sbit KL2=P3^1;sbit KL3=P3^2;sbit KL4=P3^3;
sbit KR1=P4^4;sbit KR2=P4^2;sbit KR3=P3^5;sbit KR4=P3^4;    //矩阵键盘IO定义

uint t0Cnt; //定时器t0计数
bit isTimer;    //毫秒计数使能
uint keyVal[2]={0,0};  //矩阵键盘值与长短按状态

uchar ledBuff[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};  //数码管显示缓存
uchar ledChar[11]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xbf};    //共阳数码管段码表，10：-


//main主函数
void main()
{
    P2=0xa0;P0=0;P2=0;  //关闭外设

    TMOD=0;  //开定时器t0并设为模式0 16位自动重载
    TH0=0xfc;TL0=0x66;  //1ms@11.0592MHz
    TR0=1;ET0=1;EA=1;   //使能t0和中断

    isTimer=1;  //毫秒计数使能

    while(1);
}


//led数码管扫描
void ledscan()
{
    static uint i;

    P2=0xe0;P0=0xff;P2=0;   //消隐
    P2=0xc0;P0=1<<i;P2=0;   //位选
    P2=0xe0;P0=ledBuff[i];P2=0; //段码

    if(i<7)
        i++;
    else
        i=0;
}


//led数码管设置
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


//key按键扫描
void keyscan()
{
    static uchar keyState,keyNow;  //按键状态机
    uchar kl,kr;    //临时行列键值

    //键盘行扫描
    KR1=KR2=KR3=KR4=0; KL1=KL2=KL3=KL4=1;
    if(KL1==0){kl=1;}
    else if(KL2==0){kl=2;}
    else if(KL3==0){kl=3;}
    else if(KL4==0){kl=4;}
    else{kl=5;}

    //键盘列扫描
    KL1=KL2=KL3=KL4=0; KR1=KR2=KR3=KR4=1;
    if(KR1==0){kr=1;}
    else if(KR2==0){kr=2;}
    else if(KR3==0){kr=3;}
    else if(KR4==0){kr=4;}
    else{kr=5;}

    keyNow=kl*10+kr;    //更新扫描键值

    //矩阵键盘状态机
    switch(keyState)
    {
        case 0: //闲置
        {
            if(keyNow!=55) {keyState=1;}  //按下
        }break;
        case 1: //消抖
        {
            if(keyNow!=55) 
            {
                keyVal[0]=keyNow;keyVal[1]=0;   //返回短按
                keyState=2;
            }
            else {keyState=0;}  //认为误触
        }break; 
        case 2: //消抖后
        {
            if(keyNow==55) {keyState=0;}
        }break;
        default:keyState=0;break;
    }
}

void mode_show(uchar i)
{
    switch(i)
    {
        case 0: //暂停
        {
            ledBuff[0]=ledChar[10];
            ledBuff[1]=ledChar[0];
            ledBuff[2]=ledChar[10];
        }break;

        case 1: //启动
        {
            ledBuff[0]=ledChar[10];
            ledBuff[1]=ledChar[1];
            ledBuff[2]=ledChar[10];
        }break;

        case 2: //清零
        {
            ledBuff[0]=ledChar[10];
            ledBuff[1]=ledChar[2];
            ledBuff[2]=ledChar[10];
        }break;
    }
}


void t0Server() interrupt 1 //T0:1ms
{
    static uint keyTimeCnt;
    static ulong cntCnt;

    //每30ms矩阵键盘扫描
    if(++keyTimeCnt==30)  
    {
        keyTimeCnt=0;
        keyscan();
    }

    //毫秒表
    if(isTimer==1)
    {
        if(cntCnt==9999999)    
        {
            cntCnt=0;
            mode_show(2);
            ledBuff[7]=ledChar[0];
        }else
        {
            led_set(cntCnt/1000);
            mode_show(1);
            cntCnt++;
        }
    }
    

    //矩阵键盘响应服务
    switch(keyVal[0])
    {
        case 12:
        {
            if(isTimer==0){isTimer=1;} 
            else{isTimer=0; mode_show(0);}
            keyVal[0]=keyVal[1]=0;  //按键复位
        }break;

        case 13:
        {
            cntCnt=0;
            mode_show(2);
            ledBuff[7]=ledChar[0];
            keyVal[0]=keyVal[1]=0;  //按键复位
        }break;
    }

    ledscan();
}