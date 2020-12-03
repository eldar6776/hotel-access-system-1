/**
 ******************************************************************************
 * File Name          : logger.h
 * Date               : 08/05/2016 23:15:16
 * Description        : data logger modul header
 ******************************************************************************
 *
 *
 ******************************************************************************
 */
#ifndef LOGGER_H
#define LOGGER_H    					102		// version 1.02


/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"


/* Exported defines    -------------------------------------------------------*/
#define EE_BUFFER_SIZE							32
#define LOG_SIZE								16
#define LOGGER_COMMAND_TIMEOUT					100

#define PIN_RESET					((uint8_t)0xd0)
#define POWER_ON_RESET				((uint8_t)0xd1)
#define SOFTWARE_RESET				((uint8_t)0xd2)
#define IWDG_RESET					((uint8_t)0xd3)
#define WWDG_RESET					((uint8_t)0xd4)
#define LOW_POWER_RESET				((uint8_t)0xd5)
#define FIRMWARE_UPDATE				((uint8_t)0xd6)

#define NO_EVENT                	((uint8_t)0xe0)
#define GUEST_CARD_VALID        	((uint8_t)0xe1)
#define GUEST_CARD_INVALID      	((uint8_t)0xe2)
#define HANDMAID_CARD_VALID     	((uint8_t)0xe3)
#define ENTRY_DOOR_CLOSED			((uint8_t)0xe4)
#define PC_COMMAND					((uint8_t)0xe5)
#define HANDMAID_SERVICE_END    	((uint8_t)0xe6)
#define MANAGER_CARD            	((uint8_t)0xe7)
#define SERVICE_CARD            	((uint8_t)0xe8)
#define ENTRY_DOOR_OPENED          	((uint8_t)0xe9)
#define MINIBAR_USED            	((uint8_t)0xea)
#define BALCON_DOOR_OPENED			((uint8_t)0xeb)
#define BALCON_DOOR_CLOSED			((uint8_t)0xec)
#define CARD_STACKER_ON				((uint8_t)0xed)		
#define CARD_STACKER_OFF			((uint8_t)0xee)
#define DO_NOT_DISTURB_SWITCH_ON 	((uint8_t)0xef)
#define DO_NOT_DISTURB_SWITCH_OFF	((uint8_t)0xf0)
#define HANDMAID_SWITCH_ON			((uint8_t)0xf1)
#define HANDMAID_SWITCH_OFF			((uint8_t)0xf2)
#define SOS_ALARM_TRIGGER			((uint8_t)0xf3)
#define SOS_ALARM_RESET				((uint8_t)0xf4)
#define FIRE_ALARM_TRIGGER			((uint8_t)0xf5)
#define FIRE_ALARM_RESET          	((uint8_t)0xf6)
#define UNKNOWN_CARD				((uint8_t)0xf7)
#define CARD_EXPIRED				((uint8_t)0xf8)
#define WRONG_ROOM					((uint8_t)0xf9)
#define WRONG_SYSTEM_ID				((uint8_t)0xfa)
#define CONTROLLER_RESET			((uint8_t)0xfb)
#define ENTRY_DOOR_NOT_CLOSED		((uint8_t)0xfc)
#define	DOOR_BELL_ACTIVE			((uint8_t)0xfd)

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
/* Exported types  -----------------------------------------------------------*/
typedef struct
{
	uint8_t log_event;
	uint8_t log_type;
	uint8_t log_group;
	uint8_t log_card_id[5];
	uint8_t log_time_stamp[6];
	
}LOGGER_EventTypeDef;

typedef enum
{
	LOGGER_OK			= 0x00,
	LOGGER_FULL			= 0x01,
	LOGGER_EMPTY		= 0x02,
	LOGGER_ERROR 		= 0x03,
	LOGGER_WRONG_ID 	= 0x04,
	LOGGER_BUSY			= 0x05
	
}LOGGER_StatusTypeDef;



/* Exported variables  -------------------------------------------------------*/
extern uint32_t logger_flags;
extern uint32_t logger_timer;
extern uint8_t aEepromBuffer[EE_BUFFER_SIZE];
extern uint8_t *p_spi_ee_buff;
extern uint16_t logger_list_count;
extern LOGGER_EventTypeDef LogEvent;
extern LOGGER_StatusTypeDef LOGGER_Status;


/* Exported macros     -------------------------------------------------------*/
#define LOGGER_StartTimer(TIME)     	(logger_timer = TIME)
#define IsLOGGER_TimerExpired() 		(logger_timer == 0)
#define LOGGER_StopTimer()      		(logger_timer = 0)
#define I2C_EE_WriteEnable()			(HAL_GPIO_WritePin(EE_WP_GPIO_Port, EE_WP_Pin, GPIO_PIN_RESET))
#define I2C_EE_WriteDisable()			(HAL_GPIO_WritePin(EE_WP_GPIO_Port, EE_WP_Pin, GPIO_PIN_SET))

/* Exported functions  -------------------------------------------------------*/
void LOGGER_Init(void);
LOGGER_StatusTypeDef LOGGER_Write(void);
LOGGER_StatusTypeDef LOGGER_Read(uint16_t log_id);
LOGGER_StatusTypeDef LOGGER_Delete(uint16_t log_id);


#endif
/******************************   END OF FILE  **********************************/
