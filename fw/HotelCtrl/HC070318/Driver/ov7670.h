/**
  * @file    ov7670.h 
  * @author  WB R&D Team - openmcu666
  * @version V0.1
  * @date    2015.6.23
  * @brief   OV7670 Camera Driver
  */
 
#ifndef _OV7670_H_
#define _OV7670_H_


#include "stm32f4xx.h"

#define DCMI_DR_ADDRESS       	0x50050028   


uint16_t OV7670_Check(void);
uint8_t OV7670_Init(void);
void CameraStart(void);
void cam_stop(void);

#endif /* _OV7670_H_ */

/******************* (C) COPYRIGHT 2015 WangBao *****END OF FILE****/
