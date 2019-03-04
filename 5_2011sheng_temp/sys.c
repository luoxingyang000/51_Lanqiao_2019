#include"sys.h"

uchar ledBuff[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
uchar ledChar[11]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf};

uint sysState,keyVal;
uint tMax,tMin,tNow;	//tempreature


void main()
{
	P2=0xa0;P0=0;P2=0;	//closeAll
	TMOD=0;
	TH0=0xfc;TL0=0x66;	//1ms@12T
	TR0=1;ET0=1;EA=1;
	//read data
	tNow=ds18b20_read();
	tMax=eeprom_read(0x00);
	tMin=eeprom_read(0x01);
	while(1);
}


void t0Server() interrupt 1
{
	static uint keyTimeCnt,tRefreshCnt;
	if(++keyTimeCnt==30) {keyTimeCnt=0; keyscan();}
	if(++tRefreshCnt==1000) 
	{
		tRefreshCnt=0;
		tNow=ds18b20_read();
	}
	//key action
	switch(keyVal)
	{
		case 13: if(tMax>=tMin && tMax>=0 && tMax<99) tMax++; eeprom_write(0x00,tMax); keyVal=55; break;
		case 14: if(tMax>tMin && tMin>=0 && tMin<99) tMin++; eeprom_write(0x01,tMin); keyVal=55; break;
		case 23: if(tMax>tMin && tMax>0 && tMax<=99) tMax--; eeprom_write(0x00,tMax); keyVal=55; break;
		case 24: if(tMax>=tMin && tMin>0 && tMin<=99) tMin--; eeprom_write(0x01,tMin); keyVal=55; break;
		default:break;
	}
	//temp overload
	if(tNow>tMax)
	{
		//P3.4openPWM@1kHz/30%
		
	}
	display_refresh();
	ledscan();
}


void pwmServer() interrupt 7
{
	CCF0=0; P34=!P34;
}


/////////////////////////////////////////////

void led_set(ulong num)
{
	uchar buf[8];
	uint i;
	for(i=0;i<8;i++)
	{
		buf[7-i]=num%10;
		num/=10;
	}
	for(i=0;i<8;i++)
	{
		if(buf[i]==0) {ledBuff[i]=0xff;}
		else {break;}
	}
	for(;i<8;i++)
	{ledBuff[i]=ledChar[buf[i]];}
}

void ledscan()
{
	static uint i;
	P2=0xe0; P0=0xff; P2=0;
	P2=0xc0; P0=1<<i; P2=0;
	P2=0xe0; P0=ledBuff[i]; P2=0;
	if(i<7) {i++;}
	else {i=0;}
}

void keyscan()
{
	static uint keyState,keyNow;
	uchar kl,kr;
	P30=P31=P32=P33=1; P44=P42=P35=P34=0;
	if(P30==0) {kl=1;}
	else if(P31==0) {kl=2;}
	else if(P32==0) {kl=3;}
	else if(P33==0) {kl=4;}
	else {kl=5;}
	P30=P31=P32=P33=0; P44=P42=P35=P34=1;
	if(P44==0) {kr=1;}
	else if(P42==0) {kr=2;}
	else if(P35==0) {kr=3;}
	else if(P34==0) {kr=4;}
	else {kr=5;}
	keyNow=kl*10+kr;
	switch(keyState)
	{
		case 0: 
		{
			if(keyNow!=55) {keyState=1;}
		}break;
		case 1:
		{
			if(keyNow!=55) {keyVal=keyNow; keyState=2;}
			else {keyState=0;}
		}break;
		case 2:
		{
			if(keyNow==55) {keyState=0;}
		}break;
		default: keyState=0; break;
	}
}
/////////////////////////////////////////////
uchar eeprom_read(uchar add)
{
	uchar dat;
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(add);
	IIC_WaitAck();
	IIC_Start();
	IIC_SendByte(0xa1);
	IIC_WaitAck();
	dat=IIC_RecByte();
	IIC_WaitAck();
	IIC_Ack(0);
	IIC_Stop();
	return dat;
}

void eeprom_write(uchar add,uchar dat)
{
	uint i,j;
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(add);
	IIC_WaitAck();
	IIC_SendByte(dat);
	IIC_WaitAck();
	IIC_Stop();
	for(i=0;i<150;i++)
	{for(j=0;j<150;j++);}
}

uchar ds18b20_read()
{
	uchar tL,tH,t;
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);
	tL=Read_DS18B20();
	tH=Read_DS18B20();
	t=(tL>>4)+(tH<<4);
	if(t==85) {t=0;}
	return t;
}

/////////////////////////////////////////////

void display_refresh()
{
	tMax=eeprom_read(0x00);
	tMin=eeprom_read(0x01);
	ledBuff[0]=ledChar[tMax/10]; ledBuff[1]=ledChar[tMax%10];
	ledBuff[2]=ledChar[tMin/10]; ledBuff[3]=ledChar[tMin%10];
	ledBuff[4]=ledBuff[5]=0xff;
	ledBuff[6]=ledChar[tNow/10]; ledBuff[7]=ledChar[tNow%10];
}