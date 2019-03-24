#include"sys.h"

uchar ledChar[11]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf};
uchar ledBuff[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

bit isFlow,isShowLevel;	//启停LED流转、是否显示亮度等级
uint keyVal,sysStatus;
uint lightMode,lightLevel,lightFlowInterval;	//LED亮度等级、流转间隔
uint voltVal,pwmDuty;

void main()
{
	uint i;
	
	relayopt(0);buzzopt(0);	//关闭外设

	//配置T1
	TMOD=0;
	TL0=0x66; TH0=0xfc;
	TR0=1; ET0=1; EA=1;
	
	//系统上电初始化
	for(i=0;i<8;i++) {ledBuff[i]=0xff;}
	lightMode=isFlow=1;
	voltVal=read_volt();
	lightFlowInterval=read_interval();
	
	while(1)
	{
		//系统状态机：while主
		switch(sysStatus)
		{
			case 0:	//
			{
				
			}break;
			default: sysStatus=0; break;
		}
	}
}


//T0中断服务函数 : 1ms
void sysINT() interrupt 1
{
	static uchar keyscanCnt,pwmCnt,led;
	//独立按键30ms扫描
	if(++keyscanCnt==30) {keyscanCnt=0; fourkeyscan();}
	//数码管扫描
	ledscan();
	
	//系统状态机：INT
	switch(sysStatus)
	{
		case 0:	//正常显示流转
		{
			static uint flowCnt,i;
			if(flowCnt==lightFlowInterval && isFlow==1)	//到设定间隔
			{
				switch(lightMode)	//切换显示模式
				{
					case 1: {led=1<<i; if(i<8){i++;}else{i=0; lightMode++;} break;}
					case 2: {led=0x80>>i; if(i<8){i++;}else{i=0; lightMode++;} break;}
					case 3:
					{
						if(i==0) {led=0x7e;}
						else if(i==1) {led=0xbd;}
						else if(i==2) {led=0xdb;}
						else {led=0xe7;}
						if(i<4){i++;}else{i=0; lightMode++;}
					}break;
					case 4:
					{
						if(i==0) {led=0xe7;}
						else if(i==1) {led=0xdb;}
						else if(i==2) {led=0xbd;}
						else {led=0x7e;}
						if(i<4){i++;}else{i=0; lightMode=1;}
					}break;
					default: lightMode=1; break;
				}
			}
		}break;
	}
	
	//pwmLIGHT
	if(sysStatus==0 && isFlow==1)
	{
		if(++pwmCnt==pwmDuty) {P2=(P2&0x1f)|0x80; P0=0xff; P2=P2&0x1f;}
		if(pwmCnt==10) {pwmCnt=0; P2=(P2&0x1f)|0x80; P0=led; P2=P2&0x1f;}
	}
}


//T0中断服务函数 : N/A
void T0server() interrupt 3
{
	
}


//////////////////////////////////////////////////I/O


//独立按键扫描
void fourkeyscan()
{
	static uchar keyNow,keyStatus;
	
	//独立列扫描
	if(P33==0) {keyNow=4;}
	else if(P32==0) {keyNow=3;}
	else if(P31==0) {keyNow=2;}
	else if(P30==0) {keyNow=1;}
	else {keyNow=5;}
	
	//键盘状态机
	switch(keyStatus)
	{
		case 0:	//闲置
		{
			if(keyNow!=5) {keyStatus=1;}
		}break;
		case 1:	//消抖
		{
			if(keyNow!=5) {keyVal=keyNow; keyStatus=2;}
			else {keyStatus=0;}	//误触
		}break;
		case 2:	//释放
		{
			if(keyNow==5) {keyStatus=0;}
		}break;
		default: keyStatus=0; break;
	}
}


//正常显示：显示亮度等级
void disp_lightlevel()
{
	
}


//设置状态：运行模式与流转间隔
void disp_flowinterval()
{
	
}


//数码管扫描
void ledscan()
{
	static uint i;
	P2=(P2&0x1f)|0xe0; P0=0xff; P2=P2&0x1f;	//消隐
	P2=(P2&0x1f)|0xc0; P0=1<<i; P2=P2&0x1f;	//位选
	P2=(P2&0x1f)|0xe0; P0=ledBuff[i]; P2=P2&0x1f;	//段码
	if(i<7) {i++;}
	else {i=0;}
}


//继电器作动
void relayopt(bit i)
{
	if(i)
	{P2=(P2&0x1f)|0xa0; RELAY=1; P2=P2&0x1f;}
	else
	{P2=(P2&0x1f)|0xa0; RELAY=0; P2=P2&0x1f;}
}


//蜂鸣器作动
void buzzopt(bit i)
{
	if(i)
	{P2=(P2&0x1f)|0xa0; BUZZ=1; P2=P2&0x1f;}
	else
	{P2=(P2&0x1f)|0xa0; BUZZ=0; P2=P2&0x1f;}
}


//////////////////////////////////////////////////I2C


//读取Rb2电位器电压
uchar read_volt()
{
	uchar dat;
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(0x03);
	IIC_WaitAck();
	IIC_Start();
	IIC_SendByte(0x91);
	IIC_WaitAck();
	dat=IIC_RecByte();
	IIC_Ack(0);
	IIC_Stop();
	return dat;
}


//读取EEPROM
uchar read_interval()
{
	uchar dat;
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(0x00);
	IIC_WaitAck();
	IIC_Start();
	IIC_SendByte(0xa1);
	IIC_WaitAck();
	dat=IIC_RecByte();
	IIC_Ack(0);
	IIC_Stop();
	return dat;
}


//写EEPROM
void write_interval(uchar dat)
{
	uchar i,j;
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(0x00);
	IIC_WaitAck();
	IIC_SendByte(dat);
	IIC_WaitAck();
	IIC_Ack(0);
	IIC_Stop();
	{	//延时5ms
		i=60; j=135;
		do
		{while (--j);}
		while (--i);
	}
}