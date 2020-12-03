/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__


/* Includes ------------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BUZZER_Pin GPIO_PIN_0
#define BUZZER_GPIO_Port GPIOA
#define LED_RED_Pin GPIO_PIN_1
#define LED_RED_GPIO_Port GPIOA
#define LED_GREEN_Pin GPIO_PIN_2
#define LED_GREEN_GPIO_Port GPIOA
#define MIFARE_CS_Pin GPIO_PIN_3
#define MIFARE_CS_GPIO_Port GPIOA
#define MIFARE_RST_Pin GPIO_PIN_4
#define MIFARE_RST_GPIO_Port GPIOA
#define ENC28J60_Pin GPIO_PIN_2
#define ENC28J60_GPIO_Port GPIOB
#define ENC28J60_INT_Pin GPIO_PIN_10
#define ENC28J60_INT_GPIO_Port GPIOB
#define ENC28J60_RST_Pin GPIO_PIN_11
#define ENC28J60_RST_GPIO_Port GPIOB
#define ENC28J60_CS_Pin GPIO_PIN_12
#define ENC28J60_CS_GPIO_Port GPIOB
#define USB_SEL_Pin GPIO_PIN_8
#define USB_SEL_GPIO_Port GPIOA
#define NRF24L01_IRQ_Pin GPIO_PIN_4
#define NRF24L01_IRQ_GPIO_Port GPIOB
#define NRF24L01_CSN_Pin GPIO_PIN_5
#define NRF24L01_CSN_GPIO_Port GPIOB
#define NRF24L01_CE_Pin GPIO_PIN_6
#define NRF24L01_CE_GPIO_Port GPIOB
#define EE_WP_Pin GPIO_PIN_7
#define EE_WP_GPIO_Port GPIOB


#define SYS_INFO 					"HW: DE-300617; FW: DE-260218 ";

#define DOOR_1_CHANEL				21
#define DOOR_1_ADDRESS				"0x14, 0x51, 0x26, 0x01, 0x18"

#define DOOR_2_CHANEL				22
#define DOOR_2_ADDRESS				"0x16, 0x42, 0x27, 0x02, 0x18"

#define DOOR_3_CHANEL				23
#define DOOR_3_ADDRESS				"0x18, 0x33, 0x28, 0x03, 0x18"

#define DOOR_4_CHANEL				24
#define DOOR_4_ADDRESS				"0x20, 0x24, 0x29, 0x04, 0x18"

#define DOOR_5_CHANEL				25
#define DOOR_5_ADDRESS				"0x22, 0x15, 0x30, 0x05, 0x18"

#define RADIO_BRIDGE_ADDRESS		"U, S, B, 2, R"
#define RADIO_SWITCH_TIME			1234	// 1,2s chanel switching time


/* #define USE_FULL_ASSERT    1U */
#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);
void ClearBuffer(unsigned char *buffer, unsigned int  size);
void Delay(volatile unsigned int nCount);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
