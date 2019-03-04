#include"sys.h"

uchar ledBuff[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
uchar ledChar[11]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf};
uchar sysState,keyVal;
uchar lightVal;
uint waterCnt,totalCnt;


void main()
{
	P2=0xa0; P0=0, P2=0;
	TMOD=0;
	TH0=0Xfc; TL0=0x66; 	//1ms
	TR0=1; ET0=1; EA=1;
	while(1);
}


void t0Server() interrupt 1
{
	static keyCnt,lightCnt;
	if(++keyCnt==30) {keyCnt=0; keyscan();}
	//switchSYSstate
	switch(sysState)
	{
		case 0:	//stand-by
		{
			if(keyVal==13) {sysState=1;}
			P2=0x80; P0=~0; P2=0;
			disp_set(waterCnt);
		}break;
		case 1:	//water flow
		{
			static addCnt;
			if(++addCnt==100) {addCnt=0; waterCnt++;}
			totalCnt=waterCnt/2;
			disp_set(waterCnt);
			P2=0x80; P0=~0x80; P2=0;
			if(keyVal==14) {sysState=2;}
		}break;
		case 2:	//stop
		{
			P2=0x80; P0=~0; P2=0;
			totalCnt=waterCnt/2;
			disp_set(totalCnt);
			if(keyVal==13) {sysState=1; waterCnt=0; totalCnt=0;}
		}break;
		default: sysState=0; break;
	}
	//readLIGHTval
	if(++lightCnt==100) {lightCnt=0; lightVal=read_pcf8591(0x01);}
	if(lightVal<=64) {P2=0x80; P0=~0x01; P2=0;}	//<1.25V
	else {P2=0x80; P0=~0; P2=0;}
	ledscan();
}

//////////////////////////////////////////////////

void disp_set(uint num)
{
	uchar buf[4]; uint i;
	ledBuff[0]=0xff;
	ledBuff[1]=~(~ledChar[0]|0x80);	//plus numpoint
	ledBuff[2]=ledChar[5];
	ledBuff[3]=ledChar[0];
	for(i=0;i<4;i++)
	{buf[3-i]=num%10; num/=10;}
	for(i=0;i<4;i++)
	{ledBuff[i+4]=ledChar[buf[i]];}
	ledBuff[5]=~(~ledBuff[5]|0x80);
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
	static uchar keyState,keyNow;
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
		case 0: if(keyNow!=55) keyState=1; break;
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

//////////////////////////////////////////////////

uchar read_pcf8591(uchar add)
{
	uchar dat;
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(add);
	IIC_WaitAck();
	IIC_Start();
	IIC_SendByte(0x91);
	IIC_WaitAck();
	dat=IIC_RecByte();
	IIC_Stop();
	return dat;
}