#include"sys.h"

uchar ledBuff[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
uchar ledChar[11]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf};

uchar keyVal,sysState,pwmDuty;
uint INT0cnt;


void main()
{
    uint i;
    
    P2=(P2&0x1f)|0xa0; BUZZ=0; RELAY=0; P2=P2&0x1f;
    TMOD=0;
    TH0=0xfc; TL0=0x66; //T1:1ms
    TR0=1; ET0=1; EA=1;
	
    while(1)
    {
        sonic_sendWave();
    }
}


void t0server() interrupt 1
{
    static uchar keyCnt,uartCnt,pwmCnt;
    //30ms键盘扫描
    if(++keyCnt==30) {keyCnt=0; keyscan();}
    //系统状态机
    switch(sysState)
    {
        case 0: //上电显示
        {
            led_set(keyVal);
        }sysModeSwitch(); break;
        
        case 1: //UART收发
        {
            static int i;
            i++;
            if(i==65535) {i=0; uart_sendbyte('C');}
            led_set(9600);
        }sysModeSwitch(); break;

        case 2: //PWM输出
        {
            led_set(pwmDuty*10);
            if(keyVal==33 && pwmDuty>=1 && pwmDuty<9) {keyVal=55; pwmDuty++;}
            if(keyVal==34 && pwmDuty>1 && pwmDuty<=9) {keyVal=55; pwmDuty--;}
        }sysModeSwitch(); break;

        case 3: //超声波测距
        {
            
        }sysModeSwitch(); break;

        default: sysState=0; break;
    }
    //PWM电平反转
    if(++pwmCnt==pwmDuty) {P2=(P2&0x1f)|0x80; P0=0xff; P2=P2&0x1f;}
    if(pwmCnt==10) {pwmCnt=0; P2=(P2&0x1f)|0x80; P0=0; P2=P2&0x1f;}
    //
    ledscan();
}

//////////////////////////////////////////////////

void led_set(ulong num)
{
    uchar buf[8];
    uint i;
    //分解
    for(i=0;i<8;i++)
    {
        buf[7-i]=num%10;
        num/=10;
    }
    //去前0
    for(i=0;i<8;i++)
    {
        if(buf[i]==0) {ledBuff[i]=0xff;}
        else {break;}
    }
    //刷新数码管
    for(;i<8;i++)
    {
        ledBuff[i]=ledChar[buf[i]];
    }
}


void ledscan()
{
    static uchar i;
    P2=(P2&0x1f)|0xe0; P0=0xff; P2=P2&0x1f; //消隐2
    P2=(P2&0x1f)|0xc0; P0=1<<i; P2=P2&0x1f; //位选2
    P2=(P2&0x1f)|0xe0; P0=ledBuff[i]; P2=P2&0x1f;   //段选2
    if(i<7) {i++;} else {i=0;}
}


void keyscan()
{
    static uchar keyState,keyNow;
    uchar kl,kr;
    //行扫描
    P30=P31=P32=P33=1; P44=P42=P35=P34=0;
    if(P30==0) {kl=1;}
    else if(P31==0) {kl=2;}
    else if(P32==0) {kl=3;}
    else if(P33==0) {kl=4;}
    else {kl=5;}
    //列扫描
    P44=P42=P35=P34=1; P30=P31=P32=P33=0;
    if(P44==0) {kr=1;}
    else if(P42==0) {kr=2;}
    else if(P35==0) {kr=3;}
    else if(P34==0) {kr=4;}
    else {kr=5;}
    //刷新键值
    keyNow=kl*10+kr;
    //键盘状态机
    switch(keyState)
    {
        case 0: //空闲
        {
            if(keyNow!=55) {keyState=1;}
        }break;
        case 1: //消抖
        {
            if(keyNow!=55) {keyVal=keyNow; keyState=2;}
            else {keyState=0;}  //误动作
        }break;
        case 2: //释放
        {
            if(keyNow==55) {keyState=0;}
        }break;
        default: keyState=0; break;
    }
}

void sysModeSwitch()
{
    switch(keyVal)
    {
        case 11: keyVal=55; sysState=0; break;  //显示键值
        case 12: keyVal=55; sysState=1; break;  //UART收发
        case 13: keyVal=55; sysState=2; pwmDuty=3; break;  //PWM输出
        case 14: keyVal=55; sysState=3; break;  //超声波测距
        default:break;
    }
}

//////////////////////////////////////////////////

void uart_init()
{
    SCON=0x50;  //方式1：8位UART
    AUXR |= 0x40;   //T1使能1T快速模式
    AUXR &= 0xFE;   //T1指定为波特率发生器
    TL1=0xe0; TH1=0xfe; //T2:9600bps(65536-(11059200L/4/9600))
    ET1=0; TR1=1; ES=1;   //使能串口接受中断
}

void uart_sendbyte(uchar dat)
{
    SBUF=dat;
    while(!TI); //等待发送完成
    TI=0;
}


void uart_sendstring(uchar *str)
{
    uchar *p;
    p = str;
    while(*p != '\0')
    {
        SBUF = *p;
		while(TI == 0);  //等待发送标志位置位
		TI = 0;
        p++;
    }
    if(RI!=1) {SBUF=0x00;}
}


void uart_get() interrupt 4 using 1
{

}

//////////////////////////////////////////////////

//PWM

//////////////////////////////////////////////////

void buzz(bit i)
{
    P2=(P2&0x1f)|0xa0;
    if(i==1) {BUZZ=1;}
    else {BUZZ=0;}
    P2=P2&0x1f;
}


void relay(bit i)
{
    P2=(P2&0x1f)|0xa0;
    if(i==1) {RELAY=1;}
    else {RELAY=0;}
    P2=P2&0x1f;
}

//////////////////////////////////////////////////

void sonic_sendWave()
{
	uint i=8;
	do
	{
		SONIC_TX=1;
		somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;
		SONIC_TX=0;
		somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;somenop;	
	}
	while(i--);
}