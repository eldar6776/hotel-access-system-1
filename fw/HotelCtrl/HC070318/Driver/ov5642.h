/**
  * @file    ov5642.h
  * @author  WB R&D Team - openmcu666
  * @version V0.1
  * @date    2015.11.09
  * @brief   OV5642 Camera Driver
  */
 
#ifndef _OV5642_H_
#define _OV5642_H_

#include "stm32f4xx.h"
 
#define DCMI_DR_ADDRESS       	0x50050028   

uint8_t ov5642_init(void);

#endif /* _OV5642_H_ */
