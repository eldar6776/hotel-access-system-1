/**
 ******************************************************************************
 * File Name          : main.c
 * Date               : 9.3.2018.
 * Description        : Hotel Room Controller Program Code Header
 ******************************************************************************
 *
 *
 ******************************************************************************
 */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__					170318	// version


/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx.h"


/* Exported Define    --------------------------------------------------------*/
//#define USE_FULL_ASSERT	1U	// use for full assert and debug of hal parameters 
#define USE_DEBUGGER		2U	// if using serial wire debugger remap PA13 & PA14 to swdio & swclk 
#define WRITE_DEFAULT		3U	// used for firs time flash and eeprom initialization
//#define DEMO_MODE			4U	// used for room controller demo presentation


/* Exported types ------------------------------------------------------------*/
extern CRC_HandleTypeDef hcrc;
extern I2C_HandleTypeDef hi2c1;
extern SPI_HandleTypeDef hspi2;
extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern IWDG_HandleTypeDef hiwdg;
extern RTC_HandleTypeDef hrtc;
extern RTC_TimeTypeDef time;
extern RTC_DateTypeDef date;



/* Exported Macro ------------------------------------------------------------*/
/* Exporeted Variable  -------------------------------------------------------*/
extern const char sys_info_hw[];
extern const char sys_info_fw[];
extern const uint8_t ascii_1608[];
extern const uint16_t ascii_1220[];
extern const uint16_t ascii_1626[];

/* Exported Function  ------------------------------------------------------- */
void Error_Handler(void);

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
