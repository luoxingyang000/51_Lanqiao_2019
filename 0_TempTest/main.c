/*ULuo only for temporary testing and the code will be changed anytime*/

#include"stc15f2k60s2.h"

void main()
{
    //�ر�����
    P2=0xa0;P0=0;P2=0;
    //LED��
    P2=0x80;P0=0;P2=0;
    //���������ȫ��
    P2=0xc0;P0=0xff;P2=0;
    P2=0xe0;P0=0;P2=0;

    while(1);
}