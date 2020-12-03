/**
 ******************************************************************************
 * File Name          : eeprom.h
 * Date               : 28/02/2016 23:16:19
 * Description        : eeprom memory manager modul header
 ******************************************************************************
*
* DISPLAY           pins    ->  STM32F103 Rubicon controller
* ----------------------------------------------------------------------------
* DISPLAY   +3V3    pin 1   ->  controller +3V3
* DISPLAY   GND     pin 2   ->  controller VSS
* DISPLAY   CS      pin 3   ->  PA8
* DISPLAY   RST     pin 4   ->  PA3
* DISPLAY   DC      pin 5   ->  PA2
* DISPLAY   MOSI    pin 6   ->  PA7 - SPI1 MOSI
* DISPLAY   SCK     pin 7   ->  PA5 - SPI1 SCK
* DISPLAY   LED     pin 8   ->  PB7 - PWM TIM4 CH2
* DISPLAY   MISO    pin 9   ->  PA6 - SPI1 MISO
* SD CARD   CS      pin 10  ->  PA4
* 
*
******************************************************************************
*/
#ifndef __EEPROM_H__
#define __EEPROM_H__					221018	// version

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"


/* Defines    ----------------------------------------------------------------*/
#define USE_SPI_FLASH						1U              // enable spi flash
#define SPI_FLASH_MANUFACTURER_WINBOND      0x00EF4017U     //  JEDEC device id
#define SPI_FLASH_DENSITY                   0x08000000U     // available memory bytes
#define SPI_FLASH_END_ADDRESS				0x07FFFFFFU
#define SPI_FLASH_SECTORS                   16U
#define SPI_FLASH_PAGE_SIZE                 256U            // number of bytes per page
#define SPI_FLASH_64K_BLOCKS                128U       
#define SPI_FLASH_32K_BLOCKS                256U
#define SPI_FLASH_4K_BLOCKS 		        2048U


#define SPI_FLASH_4K_ERASE_TIMEOUT          400U            // datasheet max. 4k block erase time is 60 ms typical 
#define SPI_FLASH_32K_ERASE_TIMEOUT         1600U           // datasheet max. 32k block erase time is 120 ms typical
#define SPI_FLASH_64K_ERASE_TIMEOUT         2000U           // datasheet max. 64k block erase time is 150 ms typical
#define SPI_FLASH_CHIP_ERASE_TIMEOUT        100000U         // datasheet max. chip erase time is 20 s typical
#define SPI_FLASH_TIMEOUT                   500U            // 500 ms timeout 



#define SPI_FLASH_MANUFACTURER_ID			((uint8_t)0x90U) // get manufacturer id
#define SPI_FLASH_UNIQUE_ID					((uint8_t)0x4bU)
#define SPI_FLASH_ENABLE_RESET				((uint8_t)0x66U)
#define SPI_FLASH_EXTENDED_DEVICE_INFO		((uint8_t)0x00U)
#define SPI_FLASH_WRITE_STATUS_REG          ((uint8_t)0x01U)
#define SPI_FLASH_PAGE_PGM             		((uint8_t)0x02U) 
#define SPI_FLASH_READ                      ((uint8_t)0x03U)    
#define SPI_FLASH_WRITE_DISABLE            	((uint8_t)0x04U) 
#define SPI_FLASH_READ_STATUS_REG_1         ((uint8_t)0x05U)    
#define SPI_FLASH_WRITE_ENABLE             	((uint8_t)0x06U)
#define SPI_FLASH_READ_FAST					((uint8_t)0x0bU)  
#define SPI_FLASH_4K_BLOCK_ERASE            ((uint8_t)0x20U)
#define SPI_FLASH_READ_STATUS_REG_2         ((uint8_t)0x35U)
#define SPI_FLASH_PROTECT_SECTOR            ((uint8_t)0x36U)
#define SPI_FLASH_UNPROTECT_SECTOR          ((uint8_t)0x39U)
#define SPI_FLASH_READ_SECTOR_PROT          ((uint8_t)0x3cU)
#define SPI_FLASH_32K_BLOCK_ERASE           ((uint8_t)0x52U)
#define SPI_FLASH_CHIP_ERASE               	((uint8_t)0x60U)
#define SPI_FLASH_RESET                		((uint8_t)0x99U)
#define SPI_FLASH_JEDEC_ID                 	((uint8_t)0x9fU)
#define SPI_FLASH_RESUME_POWER_DOWN    		((uint8_t)0xabU)
#define SPI_FLASH_SEQUENTIAL_PGM            ((uint8_t)0xadU)
#define SPI_FLASH_SEQUENTIAL_PGM_2          ((uint8_t)0xafU)
#define SPI_FLASH_POWER_DOWN               	((uint8_t)0xb9U)   
#define SPI_FLASH_CHIP_ERASE_2     			((uint8_t)0xc7U)
#define SPI_FLASH_64K_BLOCK_ERASE           ((uint8_t)0xd8U)
#define SPI_FLASH_STATUS_BUSY_MASK          ((uint8_t)0x01U)
#define SPI_FLASH_STATUS_WRITE_ENABLE_MASK  ((uint8_t)0x02U)

#define USE_I2C_EE                          1U
#define I2C_EE_DENSITY 						16384U  // available memory bytes
#define I2C_EE_NUMBER_OF_PAGE				256U    // number of pages
#define I2C_EE_PAGE_BYTE_SIZE   			64U     // number of bytes per page
#define I2C_EE_END_ADDRESS					0x3FFFU
#define I2C_EE_READ    						0xA1U
#define I2C_EE_WRITE   						0xA0U
#define I2C_EE_TIMEOUT						5U
#define I2C_EE_WRITE_DELAY					15U
#define I2C_EE_TRIALS						100U



/** ==========================================================================*/
/**     	S Y S T E M   C O N F I G   M E M O R Y   A D D R E S S E      	  */
/** ==========================================================================*/
#define EE_ROOM_STATUS_ADDRESS				((uint16_t)0x0020U)	// room status address
#define EE_FW_UPDATE_BYTE_CNT				((uint16_t)0x0021U)	// firmware update byte count
#define EE_FW_UPDATE_STATUS					((uint16_t)0x0025U)	// firmware update status
#define EE_ROOM_TEMPERATURE_SETPOINT		((uint16_t)0x0026U)	// room setpoint temperature in degree of Celsious
#define EE_ROOM_TEMPERATURE_DIFFERENCE		((uint16_t)0x0027U)	// room tempreature on / off difference
#define EE_RS485_INTERFACE_ADDRESS			((uint16_t)0x0028U)	// rs485 device address
#define EE_RS485_GROUP_ADDRESS				((uint16_t)0x002aU)	// rs485 group broadcast address
#define EE_RS485_BROADCAST_ADDRESS			((uint16_t)0x002cU)	// rs485 broadcast address msb
#define EE_RS485_BAUDRATE					((uint16_t)0x002eU)	// rs485 interface baudrate
#define EE_ROOM_POWER_TIMEOUT 				((uint16_t)0x0030U)	// room power expiry date time
#define EE_SYSTEM_ID_ADDRESS				((uint16_t)0x0036U)	// system id (system unique number)
#define EE_BEDDING_REPL_COUNTER_ADDRESS		((uint16_t)0x0038U)	// rfid signal user selected options
#define	EE_BEDDING_REPL_PERIOD_ADDRESS		((uint16_t)0x0039U)	// do-not-disturb signal user selected options
#define EE_BUZZER_CONFIG					((uint16_t)0x003aU)	// buzzer sound signal user selected options
#define EE_LCD_BRIGHTNESS					((uint16_t)0x003bU)
#define EE_MIFARE_KEY_A                     ((uint16_t)0x0040U)	// mifare access authentication key A
#define EE_MIFARE_KEY_B                     ((uint16_t)0x0048U)	// mifare access authentication key B 
#define EE_PERMITED_GROUP_ADDRESS 			((uint16_t)0x0050U)
#define EE_MIFARE_PERMITED_GROUP_1_ID		((uint16_t)0x0050U)	// mifare access permited group 1 id
#define EE_MIFARE_PERMITED_GROUP_2_ID		((uint16_t)0x0051U)	// mifare access permited group 2 id
#define EE_MIFARE_PERMITED_GROUP_3_ID		((uint16_t)0x0052U)	// mifare access permited group 3 id
#define EE_MIFARE_PERMITED_GROUP_4_ID		((uint16_t)0x0053U)	// mifare access permited group 4 id
#define EE_MIFARE_PERMITED_GROUP_5_ID		((uint16_t)0x0054U)	// mifare access permited group 5 id
#define EE_MIFARE_PERMITED_GROUP_6_ID		((uint16_t)0x0055U)	// mifare access permited group 6 id
#define EE_MIFARE_PERMITED_GROUP_7_ID		((uint16_t)0x0056U)	// mifare access permited group 7 id
#define EE_MIFARE_PERMITED_GROUP_8_ID		((uint16_t)0x0057U)	// mifare access permited group 8 id
#define EE_MIFARE_PERMITED_GROUP_9_ID		((uint16_t)0x0058u)	// mifare access permited group 9 id
#define EE_MIFARE_PERMITED_GROUP_10_ID		((uint16_t)0x0059U)	// mifare access permited group 10 id
#define EE_MIFARE_PERMITED_GROUP_11_ID		((uint16_t)0x005aU)	// mifare access permited group 11 id
#define EE_MIFARE_PERMITED_GROUP_12_ID		((uint16_t)0x005bU)	// mifare access permited group 12 id
#define EE_MIFARE_PERMITED_GROUP_13_ID		((uint16_t)0x005cU)	// mifare access permited group 13 id
#define EE_MIFARE_PERMITED_GROUP_14_ID		((uint16_t)0x005dU)	// mifare access permited group 14 id
#define EE_MIFARE_PERMITED_GROUP_15_ID		((uint16_t)0x005eU)	// mifare access permited group 15 id
#define EE_MIFARE_PERMITED_GROUP_16_ID		((uint16_t)0x005fU)	// mifare access permited group 16 id
#define EE_PERMITTED_ADDRESS_1				((uint16_t)0x0060U)	// additional permited address 1
#define EE_PERMITTED_ADDRESS_2				((uint16_t)0x0062U)	// additional permited address 2
#define EE_PERMITTED_ADDRESS_3				((uint16_t)0x0064U)	// additional permited address 3
#define EE_PERMITTED_ADDRESS_4				((uint16_t)0x0066U)	// additional permited address 4
#define EE_PERMITTED_ADDRESS_5				((uint16_t)0x0068U)	// additional permited address 5
#define EE_PERMITTED_ADDRESS_6				((uint16_t)0x006au)	// additional permited address 6
#define EE_PERMITTED_ADDRESS_7				((uint16_t)0x006cU)	// additional permited address 7
#define EE_PERMITTED_ADDRESS_8				((uint16_t)0x006eU)	// additional permited address 8
#define EE_ONEWIRE_ADDRESS_1                ((uint16_t)0x0070U)	// onewire Lux thermostat address 1 or DS18B20 id 1
#define EE_ONEWIRE_ADDRESS_2                ((uint16_t)0x0071U)	// onewire Lux thermostat address 2 or DS18B20 id 2
#define EE_ONEWIRE_ADDRESS_3                ((uint16_t)0x0072U)	// onewire Lux thermostat address 3 or DS18B20 id 3
#define EE_ONEWIRE_ADDRESS_4                ((uint16_t)0x0073U)	// onewire Lux thermostat address 4 or DS18B20 id 4
#define EE_ONEWIRE_ADDRESS_5                ((uint16_t)0x0074U)	// onewire Lux thermostat address 5 or DS18B20 id 5
#define EE_ONEWIRE_ADDRESS_6                ((uint16_t)0x0075U)	// onewire Lux thermostat address 6 or DS18B20 id 6
#define EE_ONEWIRE_ADDRESS_7                ((uint16_t)0x0076U)	// onewire Lux thermostat address 7 or DS18B20 id 7
#define EE_ONEWIRE_ADDRESS_8                ((uint16_t)0x0077U)	// onewire Lux thermostat address 8 or DS18B20 id 8
#define EE_ONEWIRE_ADDRESS_9                ((uint16_t)0x0078U)	// onewire Lux thermostat address 8
#define EE_DISPLAY_STATUS_ADDRESS           ((uint16_t)0x0080U)	// display status flags
#define EE_BUZZER_VOLUME_ADDRESS            ((uint16_t)0x0081U)	// buzzer volume address
#define EE_DOORLOCK_FORCE_ADDRESS           ((uint16_t)0x0082U)	// doorlock force address

#define EE_LOG_LIST_START_ADDRESS  			((uint16_t)0x0100U)	// beginning of log list
#define EE_LOG_LIST_END_ADDRESS   			I2C_EE_END_ADDRESS	// end of log list

#ifdef USE_SPI_FLASH
	
	#define EE_IMAGE_1_ADDRESS				((uint32_t)0x00000000U)	// ROOM_NUMBER_IMAGE
	#define EE_IMAGE_2_ADDRESS				((uint32_t)0x00030000U)	// DO_NOT_DISTURB_IMAGE
	#define EE_IMAGE_3_ADDRESS				((uint32_t)0x00060000U)	// BEDDING_REPLACEMENT_IMAGE	
	#define EE_IMAGE_4_ADDRESS				((uint32_t)0x00090000U)	// CLEAN_UP_IMAGE
	#define EE_IMAGE_5_ADDRESS				((uint32_t)0x000c0000U)	// GENERAL_CLEAN_UP_IMAGE
	#define EE_IMAGE_6_ADDRESS				((uint32_t)0x000f0000U)	// CARD_VALID_IMAGE	
	#define EE_IMAGE_7_ADDRESS				((uint32_t)0x00120000U)	// CARD_INVALID_IMAGE
	#define EE_IMAGE_8_ADDRESS				((uint32_t)0x00150000U)	// WRONG_ROOM_IMAGE
	#define EE_IMAGE_9_ADDRESS				((uint32_t)0x00180000U)	// TIME_EXPIRED_IMAGE
	#define EE_IMAGE_10_ADDRESS				((uint32_t)0x001b0000U)	// FIRE_ALARM_IMAGE
	#define EE_IMAGE_11_ADDRESS				((uint32_t)0x001e0000U)	// FIRE_EXIT_IMAGE
	#define EE_IMAGE_12_ADDRESS				((uint32_t)0x00210000U)	// MINIBAR_IMAGE
	#define EE_IMAGE_13_ADDRESS				((uint32_t)0x00240000U)	// ROOM_OUT_OF_SERVICE_IMAGE
	#define EE_IMAGE_14_ADDRESS				((uint32_t)0x00270000u)	// SOS_ALARM_IMAGE
	#define EE_IMAGE_15_ADDRESS				((uint32_t)0x002a0000U)	// SPI flash - new image 15
	#define EE_IMAGE_16_ADDRESS				((uint32_t)0x002d0000U)	// SPI flash - new image 16
	#define EE_IMAGE_17_ADDRESS				((uint32_t)0x00300000U)	// SPI flash - new image 17
	#define EE_IMAGE_18_ADDRESS				((uint32_t)0x00330000U)	// SPI flash - new image 18
	#define EE_IMAGE_19_ADDRESS				((uint32_t)0x00360000U)	// SPI flash - new image 19
	#define EE_NEW_FIRMWARE_ADDRESS			((uint32_t)0x00390000U)	// NEW FIRMWARE DOWNLOAD SPACE
	#define EE_NEW_BOOTLOADER_ADDRESS       ((uint32_t)0x003c0000U)	// NEW BOOTLOADER BINARY
	#define EE_SMALL_FONT_ADDRESS			((uint32_t)0x003f0000U)	// SMALL FONTS
	#define EE_MIDDLE_FONT_ADDRESS			((uint32_t)0x00420000U)	// MIDDLE FONTS
	#define EE_BIG_FONT_ADDRESS				((uint32_t)0x00450000U)	// BIG FONTS
	#define EE_NEW_IMAGE_ADDRESS			((uint32_t)0x00480000U)	// DISPLAY IMAGE DOWNLOAD SPACE
	#define EE_OLD_FIRMWARE_ADDRESS			((uint32_t)0x004b0000U)	// BACKUP FIRMWARE BINARY
	#define EE_DEFAULT_FIRMWARE_ADDRESS		((uint32_t)0x004e0000U)	// DEFAULT FIRMWARE BINARY
	 
#endif // USE SPI FLASH



/* Types  --------------------------------------------------------------------*/
/**
*   EEPROM FUNCTION RETURN STATUS
*/  
typedef enum
{
    EE_FLASH_OK         = ((uint8_t)0x00U),
    EE_FLASH_ERROR      = ((uint8_t)0x01U),
    EE_FLASH_BUSY       = ((uint8_t)0x02U),
    EE_FLASH_TIMEOUT    = ((uint8_t)0x03U)
	
}EEPROM_StatusTypeDef;


/* Variables  ----------------------------------------------------------------*/
/* Macros   ------------------------------------------------------------------*/
//#define FLASH_CS_Low()		(HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_RESET))
//#define FLASH_CS_High()		(HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_SET))


/* Function prototypes    ---------------------------------------------------*/
uint8_t SPI_FLASH_ReadByte(uint32_t address);
uint16_t SPI_FLASH_ReadInt(uint32_t address);
void SPI_FLASH_ReadPage(uint32_t address, uint8_t *data, uint16_t size);
void SPI_FLASH_WritePage(uint32_t address, uint8_t *data, uint16_t size);
void SPI_FLASH_WriteStatusRegister(uint8_t status);
void SPI_FLASH_UnprotectSector(uint32_t address);
uint8_t SPI_FLASH_WaitReadyStatus(uint32_t timeout);
uint8_t SPI_FLASH_ReleasePowerDown(void);
void SPI_FLASH_Erase(uint32_t address, uint8_t erase_type);
void EEPROM_Save(uint16_t ee_address, uint8_t* value, uint8_t size);


#endif
/******************************   END OF FILE  **********************************/
