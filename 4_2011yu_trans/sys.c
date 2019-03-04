#include "sys.h"

uint sysState,keyVal;
uint adcBuf;
bit start,dir;	//flow enable & direction
uchar recordNow,recordLast,recordAddr;	//weight record cache

uchar ledBuff[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
uchar ledChar[11]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf};	//10-

void main()
{
	P2=0xa0;P0=0;P2=0;	//closeBuzz
	//enableT0
	TMOD=0;
	TH0=0xfc;TL0=0xc66;	//1ms@12T
	TF0=0;TR0=1;ET0=1;EA=1;
	//
	while(1);
}


//t0 interrupt function
void t0Server() interrupt 1
{
	uint keyTimeCnt;
	//keyscan30ms
	if(++keyTimeCnt==30) {keyTimeCnt=0; keyscan();}
	//sysSwitch
	switch(sysState)
	{
		case 0:	//power-on
		{
			adcBuf=adc_read(0x03);
			led_set(adcBuf*4);	//readWeight
			P2=0xa0; P0 &= 0xef; P2=0;	//closeRelay
			P2=0x80; P0=0xff; P2=0;	//closeLED
			sysStatusChange();
		}break;
		case 1: //run
		{
			static uint ledFlowCnt,ledi;
			adcBuf=adc_read(0x03);	//readWeight
			//overweight
			if((adcBuf*4)>750) {sysState=2;}
			led_set(adcBuf*4);
			P2=0xa0; P0 &= 0xef; P2=0;	//closeRelay
			//flowStart
			if(start)
			{
				P2=0xa0; P0 |= 0x10; P2=0;	//openRelay
				if(++ledFlowCnt==200)	//0.2s
				{
					ledFlowCnt=0;
					if(dir)	//->
					{
						P2=0x80; P0=~(1<<ledi); P2=0;
						if(++ledi==8) {ledi=0;}
					}else	//<-
					{
						P2=0x80; P0=~(0x80>>ledi); P2=0;
						if(++ledi==8) {ledi=0;}
					}
				}
			}else
			{
				P2=0xa0; P0 &= 0xef; P2=0;	//closeRelay
				P2=0x80; P0=0xff; P2=0;	//closeLED
			}
			sysStatusChange();
		}break;
		case 2:	//overweight
		{
			P2=0xa0; P0 &= 0xef; P2=0;	//closeRelay
			P2=0x80; P0=0xff; P2=0;	//closeLED
			P2=0xa0; P0|=0x40; P2=0;	//openBuzz
			adcBuf=adc_read(0x03);	//readWeight
			recordNow=adcBuf*4;	//toWeight
			led_set(adcBuf*4);
			//auto reset
			if(recordNow<=750) {sysState=1; P2=0xa0; P0=0; P2=0;}
			if(recordNow!=recordLast)
			{
				//write weight record
				eeprom_write(recordAddr,adcBuf);
				if(recordAddr==0xff) {recordAddr=0x00;}
				else {recordAddr++;}
			}
			recordLast=recordNow;
		}break;
		default:sysState=0;break;
	}
	//ledscan&set
	ledscan();
}


//////////////////////////////////////////////////
//ledscan
void ledscan()
{
	static uint i;
	P2=0xe0; P0=0xff; P2=0;	//clear
	P2=0xc0; P0=1<<i; P2=0;	//com
	P2=0xe0; P0=ledBuff[i]; P2=0;	//segcode
	if(i<7) {i++;}
	else {i=0;}
}

//led number set
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

//matrix key scan
void keyscan()
{
	static uint keyState,keyNow;
	uint kl,kr;
	//line scan
	P30=P31=P32=P33=1; P44=P42=P35=P34=0;
	if(P30==0) {kl=1;}
	else if(P31==0) {kl=2;}
	else if(P32==0) {kl=3;}
	else if(P33==0) {kl=4;}
	else {kl=5;}
	//row scan
	P30=P31=P32=P33=0; P44=P42=P35=P34=1;
	if(P44==0) {kr=1;}
	else if(P42==0) {kr=2;}
	else if(P35==0) {kr=3;}
	else if(P34==0) {kr=4;}
	else {kr=5;}
	keyNow=kl*10+kr;
	//keySwitch
	switch(keyState)
	{
		case 0:	//idle
		{
			if(keyNow!=55) {keyState=1;}
		}break;
		case 1:	//deshake
		{
			if(keyNow!=55) {keyState=2; keyVal=keyNow;}	//returnKeyVal
			else {keyState=0;}	//errAction
		}break;
		case 2:	//release
		{
			if(keyNow==55) {keyState=0;}
		}break;
		default:keyState=0;break;
	}
}
//////////////////////////////////////////////////
//eeprom read
uchar eeprom_read(uchar add)
{
	uchar dat;
	//iic find addr
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(add);
	IIC_WaitAck();
	//iic read from addr
	IIC_Start();
	IIC_SendByte(0xa1);
	IIC_WaitAck();
	dat=IIC_RecByte();
	IIC_WaitAck();
	IIC_Ack(0);
	IIC_Stop();
	return dat;
}

//eeprom_write
void eeprom_write(uchar add,uchar dat)
{
	uint i,j;
	//iic find addr
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(add);
	IIC_WaitAck();
	//iic write addr
	IIC_SendByte(dat);
	IIC_WaitAck();
	IIC_Stop();
	//delay>10ms
	for(i=0;i<150;i++)
	{for(j=0;j<150;j++);}
}

//read adc
uchar adc_read(uchar ain)
{
	uchar dat;
	//iic find addr
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(ain);
	IIC_WaitAck();
	//iic read from addr
	IIC_Start();
	IIC_SendByte(0x91);
	IIC_WaitAck();
	dat=IIC_RecByte();
	IIC_WaitAck();
	IIC_Ack(0);
	IIC_Stop();
	return dat;
}
//////////////////////////////////////////////////
void sysStatusChange()
{
	switch(keyVal)
	{
		case 14:	//s4 start
		{sysState=1; start=1; keyVal=55; break;}
		case 21:	//s5 ->
			if(start==1)
			{dir=1; keyVal=55;} break;
		case 22: //s6 <-
			if(start==1)
			{dir=0; keyVal=55;} break;
		case 23: //s7 stop
		{sysState=1; start=0; keyVal=55; break;}
		default: break;
	}
}