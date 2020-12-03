
#ifndef __TOUCH_H
#define __TOUCH_H
#include "stm32f4xx.h"

#define CONFIG_FT5X0X_MULTITOUCH    //¶¨Òå¶àµã´¥Ãþ
//°´¼ü×´Ì¬	 
#define Key_Down 0x01
#define Key_Up   0x00 

struct _ts_event
{
    u16    x1;
    u16    y1;
    u16    x2;
    u16    y2;
    u16    x3;
    u16    y3;
    u16    x4;
    u16    y4;
    u16    x5;
    u16    y5;
    u8     touch_point;
	  u8     Key_Sta;             //±Ê¸Ë×´Ì¬	
};
extern struct _ts_event ts_event; 

#define FT5x0x_Write_ADD  0x70
#define FT5x0x_Read_ADD   0x71


void Pen_Int_Set(u8 en);
void ft5x0x_readCTP(u8 *PCTP_Par,u8 ValFlag);
u8 ft5x0x_readid(u8 RegIndex);

u8 ft5x0x_read_xy(void);
void ft5x0x_init(void);
#endif 


