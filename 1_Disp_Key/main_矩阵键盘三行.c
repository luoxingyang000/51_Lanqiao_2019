#include "STC15.h"	 
#define  u8 unsigned char

u8 code smg_du[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x00}; //0-9 
u8 code smg_wei[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

// 	 三行矩阵键盘
#define SetKey(x) P4 = (x>>3) | (x>>4);P3 = x
#define GetKey() ((P4&0x10)<<3) | ((P4&0x04)<<4) | (P3&0x3f)

u8 Trg = 0;
u8 Cont = 0;

u8 ReadKey()
{
    u8 ReadData;
    u8 key_value=0;
    SetKey(0x0f);
    ReadData = GetKey();
    SetKey(0xf0);
    ReadData = (ReadData | GetKey()) ^ 0xff;
    Trg = ReadData & (ReadData ^ Cont);
    Cont = ReadData;	

    if(Trg)
    {
        switch(Trg)
        {
            case 0x88: key_value = 4;break;
            case 0x84: key_value = 5;break;	
            case 0x82: key_value = 6;break;
            case 0x81: key_value = 7;break;

            case 0x48: key_value = 8;break;
            case 0x44: key_value = 9;break;	
            case 0x42: key_value = 10;break;
            case 0x41: key_value = 11;break;

            case 0x28: key_value = 12;break;
            case 0x24: key_value = 13;break;	
            case 0x22: key_value = 14;break;
            case 0x21: key_value = 15;break;

            case 0x18: key_value = 16;break;
            case 0x14: key_value = 17;break;	
            case 0x12: key_value = 18;break;
            case 0x11: key_value = 19;break;
        }
    }

	if(Cont)
	{
	
	}

	if(Trg==0&Cont==0)
	{
	
	}

    return key_value;	
}

void Timer_Init(void) //1ms
{
		AUXR |= 0x80;	//1T timer	
		TMOD &= 0xF0;	// 16bit 
		TL0 = 0xCD;		
		TH0 = 0xD4;		
		TF0 = 0;		
		TR0 = 1;		
		ET0 = 1;
		EA=1; 
}

bit key_flag;
void main(void)
{
		u8 key_val=0;
		P2=0xa0;P0=0x00;P2=0x00; // close buzzer and relay
		Timer_Init(); //1ms 
		while(1)
		{
			  if(key_flag) 	//10ms
				{
					 key_flag=0;
					 key_val=ReadKey();
					 switch(key_val)                       
					 {                                              
							case 4:  break;
							case 5:  break;
							case 6:  break;
							case 7:  break;
							case 8:  break;
							case 9:  break;
							case 10: break;
							case 11: break;
							case 12: break;
							case 13: break;
							case 14: break;
							case 15: break;
							case 16: break;
							case 17: break;
							case 18: break;
							case 19: break;
					 } 
				}
		 }
}

void timer0() interrupt 1  using 1                   
{
	static int key_count=0,smg_count=0,i=0;
	key_count++;smg_count++;
	if(key_count==10)			//10ms
	{
	 key_count=0;
	 key_flag=1;
	}
	
	if(smg_count==3)		//3ms
	{
			smg_count=0;
			P2=0xc0;P0=0;P2=0;				//消影
			P2=0xe0;P0=~smg_du[i];P2=0;
			P2=0xc0;P0=smg_wei[i];P2=0;
			i++;
			if(i==8) i=0;
	}
}