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
#define __MAIN_H__					221018	// version


/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx.h"


/* Exported Define    --------------------------------------------------------*/
//#define USE_FULL_ASSERT	        1U 	    // use for full assert and debug of hal parameters 
//#define USE_DEBUGGER		        2U 	    // if using serial wire debugger remap PA13 & PA14 to swdio & swclk 

#define SPI_DRIVER                  1U      // spi driver fail error log define
#define I2C_DRIVER                  2U      // i2c driver fail error log define
#define USART_DRIVER                3U      // uart driver fail error log define
#define RTC_DRIVER                  4U      // rtc driver fail error log define
#define TMR_DRIVER                  5U      // timer driver fail error log define

#define CAP_FUNC                    11U     // capacitive sensor service fail error log define
#define RC522_FUNC                  12U     // mifare card read service fail error log define
#define ONEWIRE_FUNC                13U     // onewire service fail error log define
#define RS485_FUNC                  14U     // rs485 service fail error log define
#define MAIN_FUNC                   15U     // main function fail error log define
#define DISPLAY_FUNC                16U     // display sevice fail error log define
#define LOGGER_FUNC                 17U     // logger sevice fail error log define
#define DIO_FUNC                    18U     // digital input-output service fail error log define
#define EEPROM_FUNC                 19U     // eeprom service fail error log define
#define SIGNAL_FUNC                 20U     // signal service fail error log define


/* Exported types ------------------------------------------------------------*/
extern CRC_HandleTypeDef hcrc;
extern I2C_HandleTypeDef hi2c1;
extern SPI_HandleTypeDef hspi2;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern RTC_HandleTypeDef hrtc;

#ifndef	USE_DEBUGGER
extern IWDG_HandleTypeDef hiwdg;
#endif


/* Exporeted Variable  -------------------------------------------------------*/
extern const char sys_info_hw[];
extern const char sys_info_fw[];


/* Exported Macro ------------------------------------------------------------*/
/* Exported Function  ------------------------------------------------------- */
void Error_Handler(uint8_t function, uint8_t driver);
void BootloaderExe(void);
void MX_TIM1_Init(void);
void MX_TIM3_Init(void);

#endif /* __MAIN_H__ */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
