/**
  * @file    ov2640.h
  * @author  WB R&D Team - openmcu666
  * @version V0.1
  * @date    2015.6.24
  * @brief   OV2640 Camera Driver
  */
 
#ifndef _OV2640_H_
#define _OV2640_H_


#include "stm32f4xx.h"

#define DCMI_DR_ADDRESS       	0x50050028   

 #define R_ID  0x61
 #define W_ID  0x60
uint16_t OV2640_Check(void);
uint8_t OV2640_Init(void);
void CameraStart(void);


#endif /* _OV2640_H_ */

/******************* (C) COPYRIGHT 2015 WangBao *****END OF FILE****/
