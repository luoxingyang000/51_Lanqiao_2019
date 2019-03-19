#include"sys.h"

uchar ledBuff[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
uchar ledChar[11]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf};

uchar keyVal,sysState;
uint pwmFreq,pwmDuty;
bit UARTBUSY;

void main()
{
    uint i;
    uart_init(); uart_sendbyte(0x31);
    
    P2=(P2&0x1f)|0xa0; BUZZ=0; RELAY=0; P2=P2&0x1f;
    TMOD=0;
    TH1=0xfc; TL1=0x66; //T1:1ms
    INT_CLKO |= 0x07;
    TR1=1; ET1=1; EA=1;

    pwmFreq=1000; pwmDuty=60;
	
    while(1)
    {
        
    }
}


void t0server() interrupt 3
{
    static uchar keyCnt,uartCnt;
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

        }sysModeSwitch(); break;

        case 2: //PWM输出
        {
            pwm_set(pwmFreq,pwmDuty);
            led_set(pwmFreq);
            if(keyVal==31) {keyVal=55; pwmFreq+=50;}
            if(keyVal==32) {keyVal=55; pwmFreq-=50;}
            if(keyVal==33 && pwmDuty<=95) {keyVal=55; pwmDuty+=5;}
            if(keyVal==34 && pwmDuty>=5) {keyVal=55; pwmDuty-=5;}
        }sysModeSwitch(); break;

        case 3: //超声波测距
        {

        }sysModeSwitch(); break;

        default: sysState=0; break;
    }
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
        case 13: keyVal=55; sysState=2; break;  //PWM输出
        case 14: keyVal=55; sysState=3; break;  //超声波测距
        default:break;
    }
}

//////////////////////////////////////////////////

void uart_init()
{
    SCON=0x50;  //方式1：8位UART
    T2L=0xe0; T2H=0xfe; //T2:9600bps(65536-(11059200L/4/9600))
    AUXR|=0x15; //T2使能1T与UART模式
    EA=1; ES=1;   //使能串口接受中断
}

void uart_sendbyte(uchar dat)
{
    SBUF=dat;
    while(!TI);
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

void pwm_set(uint freq,uchar duty)
{
    TH0=(65535-(freq/256))/256;
    TL0=(65535-(freq/256))%256;  //配置T0分频
    TR0=1;  //T0使能

    CMOD=0x04;  //PCA以T0为时基
    CR=1;   //PCA定时器使能

    CCAPM0=0x42;    //ECOM0=PWM0=1,PWM0/P1.1
    PCA_PWM0=0x00;  //8位PWM
    CCAP0H=CCAP0L=(float)((100-duty)/100)*0xff;  //配置占空比
}

void pwm_flip() interrupt 1
{
    
}

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