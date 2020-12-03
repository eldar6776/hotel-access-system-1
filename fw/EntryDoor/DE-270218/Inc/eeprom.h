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
#ifndef EEPROM_H
#define EEPROM_H   		102 		// version 1.02

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Defines    ----------------------------------------------------------------*/
#define USE_SPI_FLASH		

#define EE_BUFFER_SIZE						256
#define FLASH_EE_DENSITY           			8388608      // available memory bytes
#define FLASH_EE_NUMBER_OF_64K_BLOCKS      	128        
#define FLASH_EE_NUMBER_OF_32K_BLOCKS      	256       
#define FLASH_EE_NUMBER_OF_4K_BLOCKS 		2048    
#define FLASH_EE_BLOCK_PAGE_COUNT      		16
#define FLASH_EE_PAGE_BYTE_SIZE        		256    		// number of bytes per page
#define FLASH_EE_END_ADDRESS				0
#define FLASH_EE_TIMEOUT					20
#define FLASH_EE_MANUFACTURER_ID			((uint8_t)0x90)	// winbond id
#define FLASH_EE_UNIQUE_ID					((uint8_t)0x4b)
#define FLASH_EE_ENABLE_RESET				((uint8_t)0x66)
#define FLASH_EE_EXTENDED_DEVICE_INFO		((uint8_t)0x00)
#define SPI_EE_WRITE_STATUS_REG       		((uint8_t)0x01)
#define SPI_EE_PAGE_PGM             		((uint8_t)0x02) 
#define SPI_EE_READ                			((uint8_t)0x03)    
#define SPI_EE_WRITE_DISABLE            	((uint8_t)0x04) 
#define SPI_EE_READ_STATUS_REG_1       		((uint8_t)0x05)    
#define SPI_EE_WRITE_ENABLE             	((uint8_t)0x06)
#define SPI_EE_READ_FAST					((uint8_t)0x0b)  
#define SPI_EE_4K_BLOCK_ERASE				((uint8_t)0x20)
#define SPI_EE_READ_STATUS_REG_2       		((uint8_t)0x35)
#define SPI_EE_PROTECT_SECTOR				((uint8_t)0x36)
#define SPI_EE_UNPROTECT_SECTOR				((uint8_t)0x39)
#define SPI_EE_READ_SECTOR_PROT				((uint8_t)0x3c)
#define SPI_EE_32K_BLOCK_ERASE        		((uint8_t)0x52)
#define SPI_EE_CHIP_ERASE               	((uint8_t)0x60)
#define SPI_EE_RESET                		((uint8_t)0x99)
#define SPI_EE_JEDEC_ID                 	((uint8_t)0x9f)
#define SPI_EE_RESUME_POWER_DOWN    		((uint8_t)0xab)
#define SPI_EE_SEQUENTIAL_PGM				((uint8_t)0xad)
#define SPI_EE_SEQUENTIAL_PGM_2				((uint8_t)0xaf)
#define SPI_EE_POWER_DOWN               	((uint8_t)0xb9)   
#define SPI_EE_CHIP_ERASE_2     			((uint8_t)0xc7)
#define SPI_EE_64K_BLOCK_ERASE				((uint8_t)0xd8)
#define STATUS_REG_BUSY_MASK				((uint8_t)0x01)
#define STATUS_REG_WRITE_ENABLED_MASK		((uint8_t)0x02)

#define I2C_EE_DENSITY 						16384      	// available memory bytes
#define I2C_EE_NUMBER_OF_PAGE				256			// number of pages
#define I2C_EE_PAGE_BYTE_SIZE   			64         	// number of bytes per page
#define I2C_EE_END_ADDRESS					0x3fff
#define I2C_EE_READ    						0xa1
#define I2C_EE_WRITE   						0xa0
#define I2C_EE_TIMEOUT						5
#define I2C_EE_WRITE_DELAY					15
#define I2C_EE_TRIALS						100



/** ==========================================================================*/
/**                                                                           */
/**     	S Y S T E M   C O N F I G   M E M O R Y   A D D R E S S E      	  */ 
/**                                                                           */
/** ==========================================================================*/
#define EE_ROOM_STATUS_ADDRESS				((uint16_t)0x0020)	// room status address
#define EE_FW_UPDATE_BYTE_CNT				((uint16_t)0x0021)	// firmware update byte count
#define EE_FW_UPDATE_STATUS					((uint16_t)0x0025)	// firmware update status
#define EE_ROOM_TEMPERATURE_SETPOINT		((uint16_t)0x0026)	// room setpoint temperature in degree of Celsious
#define EE_ROOM_TEMPERATURE_DIFFERENCE		((uint16_t)0x0027)	// room tempreature on / off difference
#define EE_RS485_INTERFACE_ADDRESS			((uint16_t)0x0028)	// rs485 device address
#define EE_RS485_GROUP_ADDRESS				((uint16_t)0x002a)	// rs485 group broadcast address
#define EE_RS485_BROADCAST_ADDRESS			((uint16_t)0x002c)	// rs485 broadcast address msb
#define EE_RS485_BAUDRATE					((uint16_t)0x002e)	// rs485 interface baudrate

#define EE_ROOM_POWER_TIMEOUT 				((uint16_t)0x0030)	// room power expiry date time

#define EE_SYSTEM_ID_ADDRESS				((uint16_t)0x0036)	// system id (system unique number)
#define EE_BEDDING_REPL_COUNTER_ADDRESS		((uint16_t)0x0038)	// rfid signal user selected options
#define	EE_BEDDING_REPL_PERIOD_ADDRESS		((uint16_t)0x0039)	// do-not-disturb signal user selected options
#define EE_BUZZER_CONFIG					((uint16_t)0x003a)	// buzzer sound signal user selected options
#define EE_LCD_BRIGHTNESS					((uint16_t)0x003b)
#define EE_MIFARE_AUTHENTICATION_KEY_A		((uint16_t)0x0040)	// mifare access authentication key A
#define EE_MIFARE_AUTHENTICATION_KEY_B		((uint16_t)0x0048)	// mifare access authentication key B 
#define EE_PERMITED_GROUP_ADDRESS 			((uint16_t)0x0050)
#define EE_MIFARE_PERMITED_GROUP_1_ID		((uint16_t)0x0050)	// mifare access permited group 1 id
#define EE_MIFARE_PERMITED_GROUP_2_ID		((uint16_t)0x0051)	// mifare access permited group 2 id
#define EE_MIFARE_PERMITED_GROUP_3_ID		((uint16_t)0x0052)	// mifare access permited group 3 id
#define EE_MIFARE_PERMITED_GROUP_4_ID		((uint16_t)0x0053)	// mifare access permited group 4 id
#define EE_MIFARE_PERMITED_GROUP_5_ID		((uint16_t)0x0054)	// mifare access permited group 5 id
#define EE_MIFARE_PERMITED_GROUP_6_ID		((uint16_t)0x0055)	// mifare access permited group 6 id
#define EE_MIFARE_PERMITED_GROUP_7_ID		((uint16_t)0x0056)	// mifare access permited group 7 id
#define EE_MIFARE_PERMITED_GROUP_8_ID		((uint16_t)0x0057)	// mifare access permited group 8 id
#define EE_MIFARE_PERMITED_GROUP_9_ID		((uint16_t)0x0058)	// mifare access permited group 9 id
#define EE_MIFARE_PERMITED_GROUP_10_ID		((uint16_t)0x0059)	// mifare access permited group 10 id
#define EE_MIFARE_PERMITED_GROUP_11_ID		((uint16_t)0x005a)	// mifare access permited group 11 id
#define EE_MIFARE_PERMITED_GROUP_12_ID		((uint16_t)0x005b)	// mifare access permited group 12 id
#define EE_MIFARE_PERMITED_GROUP_13_ID		((uint16_t)0x005c)	// mifare access permited group 13 id
#define EE_MIFARE_PERMITED_GROUP_14_ID		((uint16_t)0x005d)	// mifare access permited group 14 id
#define EE_MIFARE_PERMITED_GROUP_15_ID		((uint16_t)0x005e)	// mifare access permited group 15 id
#define EE_MIFARE_PERMITED_GROUP_16_ID		((uint16_t)0x005f)	// mifare access permited group 16 id


#define EE_LOG_LIST_START_ADDRESS  			((uint16_t)0x0100)	// beginning of log list
#define EE_LOG_LIST_END_ADDRESS   			I2C_EE_END_ADDRESS	// end of log list

#ifdef USE_SPI_FLASH
	
	#define EE_IMAGE_1_ADDRESS				((uint32_t)0x00000000)	// ROOM_NUMBER_IMAGE
	#define EE_IMAGE_2_ADDRESS				((uint32_t)0x00030000)	// DO_NOT_DISTURB_IMAGE
	#define EE_IMAGE_3_ADDRESS				((uint32_t)0x00060000)	// BEDDING_REPLACEMENT_IMAGE	
	#define EE_IMAGE_4_ADDRESS				((uint32_t)0x00090000)	// CLEAN_UP_IMAGE
	#define EE_IMAGE_5_ADDRESS				((uint32_t)0x000c0000)	// GENERAL_CLEAN_UP_IMAGE
	#define EE_IMAGE_6_ADDRESS				((uint32_t)0x000f0000)	// CARD_VALID_IMAGE	
	#define EE_IMAGE_7_ADDRESS				((uint32_t)0x00120000)	// CARD_INVALID_IMAGE
	#define EE_IMAGE_8_ADDRESS				((uint32_t)0x00150000)	// WRONG_ROOM_IMAGE
	#define EE_IMAGE_9_ADDRESS				((uint32_t)0x00180000)	// TIME_EXPIRED_IMAGE
	#define EE_IMAGE_10_ADDRESS				((uint32_t)0x001b0000)	// FIRE_ALARM_IMAGE
	#define EE_IMAGE_11_ADDRESS				((uint32_t)0x001e0000)	// FIRE_EXIT_IMAGE
	#define EE_IMAGE_12_ADDRESS				((uint32_t)0x00210000)	// MINIBAR_IMAGE
	#define EE_IMAGE_13_ADDRESS				((uint32_t)0x00240000)	// ROOM_OUT_OF_SERVICE_IMAGE
	#define EE_IMAGE_14_ADDRESS				((uint32_t)0x00270000)	// SOS_ALARM_IMAGE
	#define EE_IMAGE_15_ADDRESS				((uint32_t)0x002a0000)	// SPI flash - new image 13
	#define EE_IMAGE_16_ADDRESS				((uint32_t)0x002d0000)	// SPI flash - new image 16
	#define EE_IMAGE_17_ADDRESS				((uint32_t)0x00300000)	// SPI flash - new image 17
	#define EE_IMAGE_18_ADDRESS				((uint32_t)0x00330000)	// SPI flash - new image 18
	#define EE_IMAGE_19_ADDRESS				((uint32_t)0x00360000)	// SPI flash - new image 19
	#define EE_NEW_FIRMWARE_ADDRESS			((uint32_t)0x00390000)	// NEW FIRMWARE DOWNLOAD SPACE
	#define EE_NEW_BOOTLOADER_ADDRESS		((uint32_t)0x003c0000)	// NEW BOOTLOADER BINARY
	#define EE_SMALL_FONT_ADDRESS			((uint32_t)0x003f0000)	// SMALL FONTS
	#define EE_MIDDLE_FONT_ADDRESS			((uint32_t)0x00420000)	// MIDDLE FONTS
	#define EE_BIG_FONT_ADDRESS				((uint32_t)0x00450000)	// BIG FONTS
	#define EE_NEW_IMAGE_ADDRESS			((uint32_t)0x00480000)	// DISPLAY IMAGE DOWNLOAD SPACE
	#define EE_OLD_FIRMWARE_ADDRESS			((uint32_t)0x004b0000)	// BACKUP FIRMWARE BINARY
	#define EE_DEFAULT_FIRMWARE_ADDRESS		((uint32_t)0x004e0000)	// DEFAULT FIRMWARE BINARY
	 

#endif // USE SPI FLASH



/* Types  --------------------------------------------------------------------*/
/**
*   EEPROM FUNCTION RETURN STATUS
*/  
typedef enum
{
    EE_OK               = 0x00,
    EE_ERROR            = 0x01,
    EE_BUSY             = 0x02
	
}EEPROM_StatusTypeDef;

/* Variables  ----------------------------------------------------------------*/
extern SPI_HandleTypeDef hspi1;
extern I2C_HandleTypeDef hi2c1;
extern uint8_t aEepromBuffer[EE_BUFFER_SIZE];
extern uint8_t *p_spi_ee_buff;


/* Macros   ------------------------------------------------------------------*/
#define SPI_EE_CS_Low()         (HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_RESET))
#define SPI_EE_CS_High()        (HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_SET))
#define I2C_EE_WriteEnable()	(HAL_GPIO_WritePin(EE_WP_GPIO_Port, EE_WP_Pin, GPIO_PIN_RESET))
#define I2C_EE_WriteDisable()	(HAL_GPIO_WritePin(EE_WP_GPIO_Port, EE_WP_Pin, GPIO_PIN_SET))

/* Function prototypes    ---------------------------------------------------*/
uint8_t SPI_FLASH_ReadStatusRegister(void);
void SPI_FLASH_WriteStatusRegister(uint8_t status);
uint32_t SPI_FLASH_ReadDeviceInfo(void);
uint8_t SPI_FLASH_ReadSectorProtectionRegister(uint32_t address);
void SPI_FLASH_UnprotectSector(uint32_t address);
void SPI_FLASH_ProtectSector(uint32_t address);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_Write_Disable(void);
EEPROM_StatusTypeDef SPI_FLASH_ReleasePowerDown(void);
EEPROM_StatusTypeDef SPI_FLASH_Erase(uint32_t address, uint8_t erase_type);
uint8_t SPI_FLASH_ReadByte(uint32_t address);
uint16_t SPI_FLASH_ReadInt(uint32_t address);
void SPI_FLASH_WriteByte(uint32_t address, uint8_t data);
void SPI_FLASH_WriteInt(uint32_t address, uint16_t data);
void SPI_FLASH_WritePage(uint32_t address, uint8_t *data, uint16_t size);
void SPI_FLASH_ReadPage(uint32_t address, uint8_t *data, uint16_t size);

#endif
/******************************   END OF FILE  **********************************/
