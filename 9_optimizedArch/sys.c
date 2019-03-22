#include"sys.h"

uchar ledBuff[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
uchar ledChar[11]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf};
uchar timeNow[6],hh,mm,ss;

bit isDS1302,isPWM,isSONIC,isUART;
uchar keyVal,sysState;


void main()
{
    P2=(P2&0x1f)|0xa0; BUZZ=0; RELAY=0; P2=P2&0x1f; //�ر�����

    //д��Ĭ��DS1302ʱ�� 00:00:00
    Write_Ds1302(0x8e,0);   //ȥд����
    Write_Ds1302(0x84,0x0c);
    Write_Ds1302(0x82,0x1e);
    Write_Ds1302(0x80,0x00);

    TMOD &= 0xF0; AUXR |= 0x80; //T1:2ms@1T
    TL0=0x9a; TH0=0xa9;
    TR0=1; TF0=0; ET0=1; EA=1;

    while(1)
    {
        //ϵͳ״̬��
        switch(sysState)
        {
            case 0: //��ˮ��
            {
                static uchar i,j1,j2;
                for(i=0;i<8;i++)
                {
                    P2=(P2&0x1f)|0x80; P0=~(1<<i); P2=P2&0x1f;
                    for(j1=0;j1<255;j1++) {for(j2=0;j2<255;j2++);}
                }
                led_set(keyVal);
            }break;
            
            case 1: //DS1302ʱ��
            {
                static bit enDS1302;
                if(enDS1302)
                {
                    //��ȡDS1302
                    Write_Ds1302(0x8e,0);
                    hh=Read_Ds1302(0x85);
                    mm=Read_Ds1302(0x83);
                    ss=Read_Ds1302(0x81);
                    timeNow[1]=hh%16; timeNow[2]=hh/16;
                    timeNow[4]=mm%16; timeNow[3]=mm/16; 
                    timeNow[6]=ss%16; timeNow[5]=ss/16;
                    //ˢ��ʱ����ʾ
                    ledBuff[0]=ledChar[timeNow[1]];
                    ledBuff[1]=ledChar[timeNow[2]];
                    ledBuff[3]=ledChar[timeNow[3]];
                    ledBuff[4]=ledChar[timeNow[4]];
                    ledBuff[6]=ledChar[timeNow[5]];
                    ledBuff[7]=ledChar[timeNow[6]];
                    ledBuff[2]=ledBuff[5]=ledChar[10];
                }
            }break;

            case 2: //DS18B20�¶�
            {

            }break;

            case 3: //��ʾ����ADC
            {

            }break;

            case 4: //��ʾEEPROM��������
            {

            }break;

            case 5: //����UART
            {

            }break;

            case 6: //1KHz�ɵ�ռ�ձ�PWM
            {

            }break;

            case 7: //���������
            {

            }break;

            default: sysState=0; break;
        }
        //
    }
}


void sysMainINT() interrupt 1
{
    static uchar keyCnt;
    //30ms����ɨ��
    if(++keyCnt==15) {keyCnt=0; keyscan();}
    //�����ɨ��
    ledscan();
    //����ϵͳ״̬�л�
    sysChange();
}

//////////////////////////////////////////////////

void led_set(ulong num)
{
    uchar buf[8];
    uint i;
    //�ֽ�
    for(i=0;i<8;i++)
    {
        buf[7-i]=num%10;
        num/=10;
    }
    //ȥǰ0
    for(i=0;i<8;i++)
    {
        if(buf[i]==0) {ledBuff[i]=0xff;}
        else {break;}
    }
    //ˢ�������
    for(;i<8;i++)
    {
        ledBuff[i]=ledChar[buf[i]];
    }
}


void ledscan()
{
    static uchar i;
    P2=(P2&0x1f)|0xe0; P0=0xff; P2=P2&0x1f; //����2
    P2=(P2&0x1f)|0xc0; P0=1<<i; P2=P2&0x1f; //λѡ2
    P2=(P2&0x1f)|0xe0; P0=ledBuff[i]; P2=P2&0x1f;   //��ѡ2
    if(i<7) {i++;} else {i=0;}
}


void keyscan()
{
    static uchar keyState,keyNow;
    uchar kl,kr;
    //��ɨ��
    P30=P31=P32=P33=1; P44=P42=P35=P34=0;
    if(P30==0) {kl=1;}
    else if(P31==0) {kl=2;}
    else if(P32==0) {kl=3;}
    else if(P33==0) {kl=4;}
    else {kl=5;}
    //��ɨ��
    P44=P42=P35=P34=1; P30=P31=P32=P33=0;
    if(P44==0) {kr=1;}
    else if(P42==0) {kr=2;}
    else if(P35==0) {kr=3;}
    else if(P34==0) {kr=4;}
    else {kr=5;}
    //ˢ�¼�ֵ
    keyNow=kl*10+kr;
    //����״̬��
    switch(keyState)
    {
        case 0: //����
        {
            if(keyNow!=55) {keyState=1;}
        }break;
        case 1: //����
        {
            if(keyNow!=55) {keyVal=keyNow; keyState=2;}
            else {keyState=0;}  //����
        }break;
        case 2: //�ͷ�
        {
            if(keyNow==55) {keyState=0;}
        }break;
        default: keyState=0; break;
    }
}


void sysChange()
{
    //�����л�ϵͳ״̬
    switch(keyVal)
    {
        case 11:    //�л�default��ˮ��
        {
            keyVal=55; sysState=0;
        }break;

        case 12:    //�л�DS1302ʱ��
        {
            isDS1302=1;
            keyVal=55; sysState=1;
        }break;

        case 13:   //�л�DS18B20�¶�
        {
            keyVal=55; sysState=2;
        }break;

        case 21:   //�л�ADC����
        {
            keyVal=55; sysState=3;
        }break;

        case 22:    //�л�EEPROM���ػ�����
        {
            keyVal=55; sysState=4;
        }break;

        case 31:    //�л�UART
        {
            keyVal=55; sysState=5;
        }break;

        case 32:    //�л��ɵ�PWM
        {
            keyVal=55; sysState=6;
        }break;

        case 33:    //�л����������
        {
            keyVal=55; sysState=7;
        }break;

        default: break;
    }
}

//////////////////////////////////////////////////

