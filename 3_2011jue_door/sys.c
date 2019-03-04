#include"sys.h"

uchar keyVal;	//按键值
uchar sysState;

uchar ledBuff[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
uchar ledChar[12]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xff};
uchar timeBuf[6],hh,mm,ss;	//当前时间

uchar pwInput[6]={12,12,12,12,12,12};
ulong pw,pw1,pw2,pw3;

void main()
{
	uint i,j;
	
	P2=0xa0;P0=0;P2=0;	//关闭外设
	
	TMOD=0;	//1ms@12T
	TL0=0x66;TH0=0xFC;
	TF0=0;TR0=1;
	ET0=1;EA=1;
	
	//上电设定时间6:59:00
	timeBuf[0]=0; timeBuf[1]=6;
	timeBuf[2]=5; timeBuf[3]=9;
	timeBuf[4]=0; timeBuf[5]=0;
	time_set();
	
	//上电设定密码654321
	EA=0;
	pw1=65; pw2=43; pw3=21;
	eeprom_write(0x00,pw1);
	{
		//延时
		i=300;j=300;
		for(;i>0;i--)
		{for(;j>0;j--);}
	}
	eeprom_write(0x01,pw2);
	{
		//延时
		i=300;j=300;
		for(;i>0;i--)
		{for(;j>0;j--);}
	}
	eeprom_write(0x02,pw3);
	{
		//延时
		i=300;j=300;
		for(;i>0;i--)
		{for(;j>0;j--);}
	}
	EA=1;
	
	
	while(1);
}


//led数字设定
void led_set(ulong numShow)
{
	uchar buf[8];
	uint i;
	
	for(i=0;i<8;i++)
	{
		buf[7-i]=numShow%10;
		numShow/=10;
	}
	
	for(i=0;i<8;i++)
	{
		if(buf[i]==0) {ledBuff[i]=0xff;}
		else {break;}
	}
	
	for(;i<8;i++)
	{ledBuff[i]=ledChar[buf[i]];}
}


//数码管扫描
void ledscan()
{
	static uint i=0;
	
	P2=0xe0;P0=0xff;P2=0;	//消隐
	P2=0xc0;P0=1<<i;P2=0;	//位选
	P2=0xe0;P0=ledBuff[i];P2=0;	//段码
	
	if(i<7) {i++;}
	else {i=0;}
}


//key键盘
void keyscan()
{
	static uchar keyState,keyNow;
	uchar kl,kr;
	
	//行扫描
	P30=P31=P32=P33=1; P34=P35=P42=P44=0;
	if(P30==0) {kl=1;}
	else if(P31==0) {kl=2;}
	else if(P32==0) {kl=3;}
	else if(P33==0) {kl=4;}
	else {kl=5;}
	
	//列扫描
	P30=P31=P32=P33=0; P34=P35=P42=P44=1;
	if(P44==0) {kr=1;}
	else if(P42==0) {kr=2;}
	else if(P35==0) {kr=3;}
	else if(P34==0) {kr=4;}
	else {kr=5;}
	
	keyNow=kl*10+kr;
	
	//键盘状态机
	switch(keyState)
	{
		case 0:	//闲置
		{
			if(keyNow!=55) {keyState=1;}	//按下
		}break;
		
		case 1:	//消抖
		{
			if(keyNow!=55)
			{
				keyVal=keyNow;	//返回短按
				keyState=2;
			}
			else {keyState=0;}	//认为误触
		}break;
		
		case 2:	//抬起
		{
			if(keyNow==55) {keyState=0;}
		}break;
		
		default:keyState=0;break;
	}
}


void t0Server() interrupt 1
{
	static uint keyTimeScan;
	
	if(++keyTimeScan==30)	//30ms扫描一次键盘
	{
		keyTimeScan=0; keyscan();
	}
	
	//系统状态切换
	switch(sysState)
	{
		case 0:	//上电
		{
			uchar h;
			time_read(); time_show();
			h=timeBuf[0]*10+timeBuf[1];
			sysState=1;
		}break;
		
		case 1:	//正常工作
		{
			static uint cursor=0;
			uint i;
			
			if(cursor==0) {for(i=0;i<8;i++) {ledBuff[i]=0xff;}}
			
			//读取eeprom密码
			pw1=eeprom_read(0x00);
			pw2=eeprom_read(0x01);
			pw3=eeprom_read(0x02);
			pw=pw1*10000+pw2*100+pw3;
			
			//获得密码
			switch(keyVal)
			{
				case 11: if(cursor<8) {pwInput[cursor]=0; ledBuff[cursor+2]=ledChar[0]; cursor++; keyVal=55;} break;
				case 12: if(cursor<8) {pwInput[cursor]=1; ledBuff[cursor+2]=ledChar[1]; cursor++; keyVal=55;} break;
				case 13: if(cursor<8) {pwInput[cursor]=2; ledBuff[cursor+2]=ledChar[2]; cursor++; keyVal=55;} break;
				case 14: if(cursor<8) {pwInput[cursor]=3; ledBuff[cursor+2]=ledChar[3]; cursor++; keyVal=55;} break;
				case 21: if(cursor<8) {pwInput[cursor]=4; ledBuff[cursor+2]=ledChar[4]; cursor++; keyVal=55;} break;
				case 22: if(cursor<8) {pwInput[cursor]=5; ledBuff[cursor+2]=ledChar[5]; cursor++; keyVal=55;} break;
				case 23: if(cursor<8) {pwInput[cursor]=6; ledBuff[cursor+2]=ledChar[6]; cursor++; keyVal=55;} break;
				case 24: if(cursor<8) {pwInput[cursor]=7; ledBuff[cursor+2]=ledChar[7]; cursor++; keyVal=55;} break;
				case 31: if(cursor<8) {pwInput[cursor]=8; ledBuff[cursor+2]=ledChar[8]; cursor++; keyVal=55;} break;
				case 32: if(cursor<8) {pwInput[cursor]=9; ledBuff[cursor+2]=ledChar[9]; cursor++; keyVal=55;} break;
				case 33: //跳转设置密码
				{
					sysState=2; cursor=0;
					for(i=0;i<6;i++) {pwInput[i]=55;ledBuff[i+2]=ledBuff[0]=ledBuff[1]=0xff;}
				}break;
				case 34: //复位
				{
					cursor=0;
					for(i=0;i<6;i++) {pwInput[i]=55;ledBuff[i+2]=ledBuff[0]=ledBuff[1]=0xff;}
				}
				case 43: //确认
				{
					static uint lockCnt;
					ulong pwBuf;
					pwBuf=pwInput[0]*100000+pwInput[1]*10000+pwInput[2]*1000+pwInput[3]*100+pwInput[4]*10+pwInput[5];
					if(pwBuf==pw)
					{
						for(i=0;i<6;i++) {ledBuff[i+2]=ledBuff[0]=ledBuff[1]=0xff;}
						lock(1);	//开锁
						if(++lockCnt==500) {lockCnt=0;pwBuf=0;lock(0);}	//5s关锁
					}else
					{
						cursor=0; lock(0);
						for(i=0;i<6;i++) {pwInput[i]=55;ledBuff[i+2]=ledBuff[0]=ledBuff[1]=0xff;}
					}
				}
				//led_set(pw);
				default:break;
			}
			ledBuff[0]=ledBuff[1]=ledChar[10];
		}			
		
		case 2:	//
		{
			
		}
		
		default:sysState=1;break;
	}
	
	ledscan();
}


//ds1302读时间
void time_read()
{
	Write_Ds1302(0x8e,0);	//去写保护
	hh=Read_Ds1302(0x85);
	mm=Read_Ds1302(0x83);
	ss=Read_Ds1302(0x81);
	timeBuf[0]=hh/16;	timeBuf[1]=hh%16;
	timeBuf[2]=mm/16; timeBuf[3]=mm%16;
	timeBuf[4]=ss/16;	timeBuf[5]=ss%16;
}


//ds1302写时间
void time_set()
{
	Write_Ds1302(0x8e,0);	//去写保护
	Write_Ds1302(0x84,timeBuf[0]*16+timeBuf[1]);	//hh
	Write_Ds1302(0x82,timeBuf[2]*16+timeBuf[3]);	//mm
	Write_Ds1302(0x80,timeBuf[4]*16+timeBuf[5]);	//ss
}


//time时间显示
void time_show()
{
	ledBuff[2]=ledBuff[5]=ledChar[10];
	ledBuff[0]=ledChar[timeBuf[0]]; ledBuff[1]=ledChar[timeBuf[1]];
	ledBuff[3]=ledChar[timeBuf[2]]; ledBuff[4]=ledChar[timeBuf[3]];
	ledBuff[6]=ledChar[timeBuf[4]]; ledBuff[7]=ledChar[timeBuf[5]];
}


//eeprom读
uchar eeprom_read(uchar add)
{
	uchar dat;
	IIC_Start();
	IIC_SendByte(0xa0);	//写
	IIC_WaitAck();
	IIC_SendByte(add);	//读的地址
	IIC_WaitAck();
	IIC_Start();
	IIC_SendByte(0xa1);	//读
	IIC_WaitAck();
	dat=IIC_RecByte();	//读地址
	IIC_Ack(0);
	IIC_Stop();
	return dat;
}


//eeprom写
void eeprom_write(uchar add,uchar dat)
{
	IIC_Start();
	IIC_SendByte(0xa0);	//写
	IIC_WaitAck();
	IIC_SendByte(add);	//写的地址
	IIC_WaitAck();
	IIC_SendByte(dat);	//写地址
	IIC_WaitAck();
	IIC_Stop();
}


//开闭锁
void lock(bit open)
{
	if(open)
	{
		P2=0xa0;P0=0x20;P2=0;
	}else
	{
		P2=0xa0;P0=0;P2=0;
	}
}

//蜂鸣
void buzz(bit open)
{
	if(open)
	{
		P2=0xa0;P0=0x40;P2=0;
	}else
	{
		P2=0xa0;P0=0;P2=0;
	}
}