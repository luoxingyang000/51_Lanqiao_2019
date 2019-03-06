#include"sys.h"

uchar ledBuff[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
uchar ledChar[11]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf};
	
uchar timeNow[6]={0,8,3,0,0,0};
uchar hh,mm,ss;
uchar keyVal,sysState;
uchar humdityVal,humdityLimit;
bit isBuzzEnable;
	

void main()
{
	P2=0xa0;P0=0;P2=0;
	TMOD=0;
	TH0=0xfc; TL0=0x66;
	TR0=1; ET0=1; EA=1;
	//timeset
	Write_Ds1302(0x8e,0);
	Write_Ds1302(0x84,timeNow[0]*16+timeNow[1]);
	Write_Ds1302(0x82,timeNow[2]*16+timeNow[3]);
	Write_Ds1302(0x80,timeNow[4]*16+timeNow[5]);
	//setHUMDITYlimit
	eeprom_write(0x00,50);
	isBuzzEnable=1;
	while(1);
}


void t0server() interrupt 1
{
	static keyCnt,humdityCnt;
	if(++keyCnt==30) {keyCnt=0; keyscan();}
	//refreshHUMDITY
	if(++humdityCnt==100)
	{
		humdityCnt=0;
		ds1302_read();
		humdityVal=(char)((float)pcf8591_read(0x03)*0.39);
	}
	//sysSTATE
	switch(sysState)
	{
		case 0:	//auto
		{
			humdityLimit=eeprom_read(0x00);
			ds1302_read();
			if(humdityVal<humdityLimit) {relay(1);}
			else {relay(0);}
			switch(keyVal)
			{
				case 23: keyVal=55; sysState=2; break;
				case 24: keyVal=55; sysState=1; break;
				default: break;
			}
			ledset_normal();
		}break;
		case 1:	//manmade
		{
			humdityLimit=eeprom_read(0x00);
			ds1302_read();
			if(humdityVal<humdityLimit)
			{
				if(isBuzzEnable) {buzz(1);}
				else {buzz(0);}
			}else {buzz(0);}
			switch(keyVal)
			{
				case 21: keyVal=55; relay(0); break;
				case 22: keyVal=55; relay(1); break;
				case 23: keyVal=55; isBuzzEnable=!isBuzzEnable; break;
				case 24: keyVal=55; sysState=0; break;
				default: break;
			}
			ledset_normal();
		}break;
		case 2:	//humdityset
		{
			switch(keyVal)
			{
				case 21: keyVal=55; humdityLimit--; break;
				case 22: keyVal=55; humdityLimit++; break;
				case 23: keyVal=55; eeprom_write(0x00,humdityLimit); sysState=0; break;
				case 24: keyVal=55; sysState=1; break;
				default: break;
			}
			ledset_humdityset();
		}break;
		default: sysState=0; break;
	}
	ledscan();
}

//////////////////////////////////////////////////

void ledset_normal()
{
	ledBuff[0]=ledChar[timeNow[0]];
	ledBuff[1]=ledChar[timeNow[1]];
	ledBuff[2]=ledChar[10];
	ledBuff[3]=ledChar[timeNow[2]];
	ledBuff[4]=ledChar[timeNow[3]];
	ledBuff[5]=0xff;
	ledBuff[6]=ledChar[humdityVal/10];
	ledBuff[7]=ledChar[humdityVal%10];
}


void ledset_humdityset()
{
	uchar a1,a2;
	a2=humdityLimit%10; a1=humdityLimit/10;
	ledBuff[0]=ledBuff[1]=ledChar[10];
	ledBuff[2]=ledBuff[3]=ledBuff[4]=ledBuff[5]=0xff;
	ledBuff[6]=ledChar[a1];
	ledBuff[7]=ledChar[a2];
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
		case 0:
		{
			if(keyNow!=55) {keyState=1;}
		}break;
		case 1:
		{
			if(keyNow!=55) {keyState=2; keyVal=keyNow;}
		}break;
		case 2:
		{
			if(keyNow==55) {keyState=0;}
		}break;
		default: keyState=0; break;
	}
}

//////////////////////////////////////////////////

void ds1302_read()
{
	hh=Read_Ds1302(0x85);
	mm=Read_Ds1302(0x83);
	ss=Read_Ds1302(0x81);
	timeNow[0]=hh/16; timeNow[1]=hh%16;
	timeNow[2]=mm/16; timeNow[3]=mm%16;
	timeNow[4]=ss/16; timeNow[5]=ss%16;
}


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

uchar pcf8591_read(uchar add)
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
	IIC_Ack(0);
	IIC_Stop();
	return dat;
}

//////////////////////////////////////////////////

void buzz(bit i)
{
	P2=(P2&0x1f)|0xa0;
	if(i==1) {P0|=0x40;}
	if(i==0) {P0&=0xbf;}
	P2&=0x1f;
}


void relay(bit i)
{
	P2=(P2&0x1f)|0xa0;
	if(i==1) {P0|=0x10;}
	if(i==0) {P0&=0xef;}
	P2&=0x1f;
}