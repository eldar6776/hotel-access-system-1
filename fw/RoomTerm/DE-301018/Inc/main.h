/**
 ******************************************************************************
 * File Name          : main.c
 * Date               : 10.3.2018.
 * Description        : Hotel Room Thermostat Program Header
 ******************************************************************************
 *
 *
 ******************************************************************************
 */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__                              301018		// date version

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx.h"

/* Program code switch -------------------------------------------------------*/
//#define USE_FULL_ASSERT       1U	// use for full assert and debug of hal parameters 
//#define USE_DEBUGGER          2U	// if using serial wire debugger remap PA13 & PA14 to swdio & swclk 
//#define WRITE_DEFAULT         3U	// used for firs time flash and eeprom initialization
//#define DEMO_MODE             4U	// used for room controller demo presentation
//#define WATCHDOOG_ENABLE      5U  // 

#define I2C_EE_DENSITY 						    16384U      	// available memory bytes
#define I2C_EE_NUMBER_OF_PAGE				    256U			// number of pages
#define I2C_EE_PAGE_BYTE_SIZE   			    64U         	// number of bytes per page
#define I2C_EE_END_ADDRESS					    0x3FFFU
#define I2C_EE_READ    						    0xA1U
#define I2C_EE_WRITE   						    0xA0U
#define I2C_EE_TIMEOUT						    5U
#define I2C_EE_WRITE_DELAY					    15U
#define I2C_EE_TRIALS						    100U
                    

/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Base address of the Flash sectors */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base address of Sector 0, 32 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08008000) /* Base address of Sector 1, 32 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08010000) /* Base address of Sector 2, 32 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x08018000) /* Base address of Sector 3, 32 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08020000) /* Base address of Sector 4, 128 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08040000) /* Base address of Sector 5, 256 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08080000) /* Base address of Sector 6, 256 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x080C0000) /* Base address of Sector 7, 256 Kbytes */

/* Exported constants --------------------------------------------------------*/
#define BUZZER_OFF                      0U
#define BUZZER_CLICK                    1U
#define BUZZER_SHORT                    2U
#define BUZZER_MIDDLE                   3U
#define BUZZER_LONG                     4U



void Error_Handler(void);


#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
