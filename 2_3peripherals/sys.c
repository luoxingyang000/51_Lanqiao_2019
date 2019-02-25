#include"sys.h"

uchar sysState; //ϵͳ״̬
uint keyVal;    //����ֵ
uint t0Cnt; //t0��ʱ������

uchar ledBuff[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
uchar ledChar[11]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf};

uchar timeNow[6]={11,11,11,11,11,11};   //��ǰʱ��
uchar hh,mm,ss;
uint tempreatureGet,tLSB,tMSB;    //��ȡ�¶Ȼ���ߵ�λ
uint voltageGet;    //AIN3��λ������


void main()
{
    P2=0xa0;P0=0;P2=0;  //�ر�����

    //������ʱ��
    TMOD=0;
	TL0=0x66;TH0=0xfc;  //1ms@12T
	TR0=1;EA=1;ET0=1;

    while(1);
}

//ledɨ��
void ledscan()
{
    static uchar i;

    P2=0xe0;P0=0xff;P2=0;   //����
    P2=0xc0;P0=1<<i;P2=0;   //λѡ
    P2=0xe0;P0=ledBuff[i];P2=0;  //����

    if(i<7) {i++;}
    else {i=0;}
}

//led�����趨
void led_set(ulong number)
{
    uint i;
    uchar buf[8];
    
    for(i=7;i>0;i--)
    {
        buf[i]=number%10;
        number/=10;
    }

    for(i=0;i<8;i++)
    {
        if(buf[i]==0) {ledBuff[i]=0xff;} else {break;}
    }

    for(;i<8;i++)
    {
        ledBuff[i]=ledChar[buf[i]];
    }
}

//����ɨ��
void keyscan()
{
    static uchar keyState,keyNow;
    uchar kl,kr;    //��ȡ��ֵ

    //��ɨ��
    P30=P31=P32=P33=1; P34=P35=P42=P44=0;
    if(P30==0) {kl=1;}
    else if(P31==0) {kl=2;}
    else if(P32==0) {kl=3;}
    else if(P33==0) {kl=4;}
    else {kl=5;}

    //��ɨ��
    P30=P31=P32=P33=0; P34=P35=P42=P44=1;
    if(P44==0) {kr=1;}
    else if(P42==0) {kr=2;}
    else if(P35==0) {kr=3;}
    else if(P34==0) {kr=4;}
    else {kr=5;}

    keyNow=kl*10+kr;    //��ֵ����

    //����״̬��
    switch(keyState)
    {
        case 0: //����
        {
            if(keyNow!=55) {keyState=1;}
        }break;
        case 1: //����
        {
            if(keyNow!=55)
            {
                keyVal=keyNow;  //���ض̰�ֵ
                keyState=2;
            }
            else {keyState=0;}
        }break;
        case 2: //�ͷ�
        {
            if(keyNow==55) {keyState=0;}
        }break;
        default:keyState=0;break;
    }
}

void t0Server() interrupt 1
{
    static uint keyTimeCnt;
    //static uint cursor=0; //���λ��

    //ÿ30ms�������ɨ��
    if(++keyTimeCnt==30)
    {
        keyTimeCnt=0;
        keyscan();
    }

    //ϵͳ״̬
    switch(sysState)
    {
        case 0: //�ϵ��趨ʱ��
        {
            static uint cursor=0; //���λ��

            if(keyVal!=55)
            {
                switch(keyVal)
                {
                    case 12: timeNow[cursor]=1;keyVal=55;cursor++;break;
                    case 13: timeNow[cursor]=2;keyVal=55;cursor++;break;
                    case 14: timeNow[cursor]=3;keyVal=55;cursor++;break;
                    case 22: timeNow[cursor]=4;keyVal=55;cursor++;break;
                    case 23: timeNow[cursor]=5;keyVal=55;cursor++;break;
                    case 24: timeNow[cursor]=6;keyVal=55;cursor++;break;
                    case 32: timeNow[cursor]=7;keyVal=55;cursor++;break;
                    case 33: timeNow[cursor]=8;keyVal=55;cursor++;break;
                    case 34: timeNow[cursor]=9;keyVal=55;cursor++;break;
                    case 43: timeNow[cursor]=0;keyVal=55;cursor++;break;
                    default:break;
                }
            }
            if(cursor==6)
            {
                hh=timeNow[0]*10+timeNow[1]; mm=timeNow[2]*10+timeNow[3]; ss=timeNow[4]*10+timeNow[5];
                cursor=0;sysState=1;
                Write_Ds1302(0x8e,0);   //ȥд����
                Write_Ds1302(0x84,timeNow[0]*16+timeNow[1]);
                Write_Ds1302(0x82,timeNow[2]*16+timeNow[3]);
                Write_Ds1302(0x80,timeNow[4]*16+timeNow[5]);  //д��ʱ����
            }else
            {
                ledBuff[0]=ledChar[timeNow[0]]; ledBuff[1]=ledChar[timeNow[1]];
                ledBuff[2]=ledChar[10];
                ledBuff[3]=ledChar[timeNow[2]]; ledBuff[4]=ledChar[timeNow[3]];
                ledBuff[5]=ledChar[10];
                ledBuff[6]=ledChar[timeNow[4]]; ledBuff[7]=ledChar[timeNow[5]];
            }
        }break;

        case 1: //������ʾʱ��
        {
            Write_Ds1302(0x8e,0);   //ȥд����
            hh=Read_Ds1302(0x85);
            mm=Read_Ds1302(0x83);
            ss=Read_Ds1302(0x81);   //��ʱ����
            timeNow[0]=hh/16; timeNow[1]=hh%16;
            timeNow[2]=mm/16; timeNow[3]=mm%16;
            timeNow[4]=ss/16; timeNow[5]=ss%16;

            //ˢ��ʱ����ʾ
            ledBuff[0]=ledChar[timeNow[0]]; ledBuff[1]=ledChar[timeNow[1]];
            ledBuff[2]=ledChar[10];
            ledBuff[3]=ledChar[timeNow[2]]; ledBuff[4]=ledChar[timeNow[3]];
            ledBuff[5]=ledChar[10];
            ledBuff[6]=ledChar[timeNow[4]]; ledBuff[7]=ledChar[timeNow[5]];

            //�л�״̬
            if(keyVal==13) {sysState=2;keyVal=0;}
            if(keyVal==14) {sysState=3;keyVal=0;}
        }break;

        case 2: //��ʾ�¶�
        {
            static uint i;
            EA=0;
            if(init_ds18b20()==0)
            {
                Write_DS18B20(0xcc);    //����ROM���
                Write_DS18B20(0x44);    //����tת��
                i=0;
                Write_DS18B20(0xcc);    //����ROM���
                Write_DS18B20(0xbe);    //д��ȡָ��
                tLSB=Read_DS18B20();    //�ȶ�ȡ��λ
                tMSB=Read_DS18B20();
                tempreatureGet=((tMSB<<8)+tLSB)>>4;
                led_set(tempreatureGet);
            }
            EA=1;

            //�л�״̬
            if(keyVal==12) {sysState=1;EA=1;keyVal=0;}
            if(keyVal==14) {sysState=3;EA=1;keyVal=0;}
        }break;

        case 3: //��ʾADC
        {
            IIC_Start();
            IIC_SendByte(0x90); //Ѱַ
            if(IIC_WaitAck()!=0)
            {
                IIC_Stop();
                ledBuff[5]=ledChar[10];
            }
            IIC_SendByte(0x03); //AIN3
            IIC_Start();
            IIC_SendByte(0x91); //Ѱַ��
            IIC_RecByte();  //�����ֽ�
            voltageGet=IIC_RecByte();   //��ȡ����
            IIC_Stop();
            led_set(voltageGet);    //��ʾ����

            //�л�״̬
            if(keyVal==12) {sysState=1;keyVal=0;}
            if(keyVal==13) {sysState=2;keyVal=0;}
        }break;

        default:sysState=0;break;
    }

    ledscan(); 
}