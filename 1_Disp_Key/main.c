/*2019蓝桥杯_1数码管和矩阵键盘*/
/*目标实现：数码管显示矩阵键盘获得的数字*/

#include"main.h"

uint t0Cnt; //定时器t0计数
uchar MKEY;  //矩阵键盘状态寄存器
uchar keyGetNum[2]; //按键获得

uchar ledBuff[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};  //数码管显示缓存
uchar ledChar[11]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xfd};    //共阳数码管段码表，10：-


//main主函数
void main()
{
    P2=0xa0;P0=0;P2=0;  //关闭外设
    MKEY=0xff; //MKEY寄存器初始化

    TMOD=0;  //开定时器t0并设为模式0 16位自动重载
    TH0=0xfc;TL0=0x66;  //1ms@11.0592MHz
    TR0=1;ET0=1;EA=1;   //使能t0和中断

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
    
}


//key读取按键号
void key_get()
{
    //static uint keyCnt,tDelta;

}


void t0Server() interrupt 1 //T0:1ms
{
    static uint keyCnt;
    static ulong cntCnt;

    if(keyCnt==10)  //每10ms矩阵键盘扫描
    {
        keyCnt=0;
        keyscan();
    }else{keyCnt++;}

    if(cntCnt==99999999)    //毫秒表
    {
        cntCnt=0;
    }else
    {
        led_set(cntCnt);
        cntCnt++;
    }
    
    ledscan();
}