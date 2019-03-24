#include"sys.h"

uchar ledChar[11]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf};
uchar ledBuff[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

bit isFlow,isShowLevel;	//��ͣLED��ת���Ƿ���ʾ���ȵȼ�
uint keyVal,sysStatus;
uint lightMode,lightLevel,lightFlowInterval;	//LED���ȵȼ�����ת���
uint voltVal,pwmDuty;

void main()
{
	uint i;
	
	relayopt(0);buzzopt(0);	//�ر�����

	//����T1
	TMOD=0;
	TL0=0x66; TH0=0xfc;
	TR0=1; ET0=1; EA=1;
	
	//ϵͳ�ϵ��ʼ��
	for(i=0;i<8;i++) {ledBuff[i]=0xff;}
	lightMode=isFlow=1;
	voltVal=read_volt();
	lightFlowInterval=read_interval();
	
	while(1)
	{
		//ϵͳ״̬����while��
		switch(sysStatus)
		{
			case 0:	//
			{
				
			}break;
			default: sysStatus=0; break;
		}
	}
}


//T0�жϷ����� : 1ms
void sysINT() interrupt 1
{
	static uchar keyscanCnt,pwmCnt,led;
	//��������30msɨ��
	if(++keyscanCnt==30) {keyscanCnt=0; fourkeyscan();}
	//�����ɨ��
	ledscan();
	
	//ϵͳ״̬����INT
	switch(sysStatus)
	{
		case 0:	//������ʾ��ת
		{
			static uint flowCnt,i;
			if(flowCnt==lightFlowInterval && isFlow==1)	//���趨���
			{
				switch(lightMode)	//�л���ʾģʽ
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


//T0�жϷ����� : N/A
void T0server() interrupt 3
{
	
}


//////////////////////////////////////////////////I/O


//��������ɨ��
void fourkeyscan()
{
	static uchar keyNow,keyStatus;
	
	//������ɨ��
	if(P33==0) {keyNow=4;}
	else if(P32==0) {keyNow=3;}
	else if(P31==0) {keyNow=2;}
	else if(P30==0) {keyNow=1;}
	else {keyNow=5;}
	
	//����״̬��
	switch(keyStatus)
	{
		case 0:	//����
		{
			if(keyNow!=5) {keyStatus=1;}
		}break;
		case 1:	//����
		{
			if(keyNow!=5) {keyVal=keyNow; keyStatus=2;}
			else {keyStatus=0;}	//��
		}break;
		case 2:	//�ͷ�
		{
			if(keyNow==5) {keyStatus=0;}
		}break;
		default: keyStatus=0; break;
	}
}


//������ʾ����ʾ���ȵȼ�
void disp_lightlevel()
{
	
}


//����״̬������ģʽ����ת���
void disp_flowinterval()
{
	
}


//�����ɨ��
void ledscan()
{
	static uint i;
	P2=(P2&0x1f)|0xe0; P0=0xff; P2=P2&0x1f;	//����
	P2=(P2&0x1f)|0xc0; P0=1<<i; P2=P2&0x1f;	//λѡ
	P2=(P2&0x1f)|0xe0; P0=ledBuff[i]; P2=P2&0x1f;	//����
	if(i<7) {i++;}
	else {i=0;}
}


//�̵�������
void relayopt(bit i)
{
	if(i)
	{P2=(P2&0x1f)|0xa0; RELAY=1; P2=P2&0x1f;}
	else
	{P2=(P2&0x1f)|0xa0; RELAY=0; P2=P2&0x1f;}
}


//����������
void buzzopt(bit i)
{
	if(i)
	{P2=(P2&0x1f)|0xa0; BUZZ=1; P2=P2&0x1f;}
	else
	{P2=(P2&0x1f)|0xa0; BUZZ=0; P2=P2&0x1f;}
}


//////////////////////////////////////////////////I2C


//��ȡRb2��λ����ѹ
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


//��ȡEEPROM
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


//дEEPROM
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
	{	//��ʱ5ms
		i=60; j=135;
		do
		{while (--j);}
		while (--i);
	}
}