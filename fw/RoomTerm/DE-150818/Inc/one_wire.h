/**
 ******************************************************************************
 * File Name          : one_wire.h
 * Date               : 17/11/2016 00:59:00
 * Description        : one wire communication modul header
 ******************************************************************************
 */
 
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ONEWIRE_H__
#define __ONEWIRE_H__                            RT200119    // firmware version


/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx.h"


/* Exporeted Types   ---------------------------------------------------------*/
typedef enum
{
	ONEWIRE_INIT            = ((uint8_t)0x00U),
    ONEWIRE_RECEIVE         = ((uint8_t)0x01U),
	ONEWIRE_PACKET_PENDING  = ((uint8_t)0x02U),
	ONEWIRE_PACKET_RECEIVED = ((uint8_t)0x03U),
	ONEWIRE_PACKET_SEND     = ((uint8_t)0x04U),
	ONEWIRE_ERROR           = ((uint8_t)0x05U)
	
}OnewireStateTypeDef;


typedef struct
{
	uint8_t	sensor_id;
	uint8_t rom_code[8];
	int temperature;
	
}TempSensorTypeDef;

extern TempSensorTypeDef ds18b20_1;


/* Exporeted Define   --------------------------------------------------------*/
#define ONEWIRE_BUF_SIZE                        96U
#define ONEWIRE_PACKET_SIZE                     64U
#define ONEWIRE_RX_TX_DELAY                     2U
#define ONEWIRE_UPDATE_TIME                     89U
#define ONEWIRE_TRANSFER_TIMEOUT                30U
#define ONEWIRE_PENDING_TIMEOUT                 567U
#define ONEWIRE_ROOM_CONTROLLER_ADDRESS         10U
#define ONEWIRE_2400                            ((uint32_t)2400U)
#define ONEWIRE_4800                            ((uint32_t)4800U)
#define ONEWIRE_9600                            ((uint32_t)9600U)
#define ONEWIRE_19200                           ((uint32_t)19200U)
#define ONEWIRE_38400                           ((uint32_t)38400U)
#define ONEWIRE_57600                           ((uint32_t)57600U)
#define ONEWIRE_115200                          ((uint32_t)115200U)
#define SOH                                     ((uint8_t)0x01U) 	/* start of command packet */
#define STX                                     ((uint8_t)0x02U) 	/* start of data packet */
#define EOT                                     ((uint8_t)0x04U) 	/* end of transmission */
#define ACK                                     ((uint8_t)0x06U) 	/* acknowledge */
#define NAK                                     ((uint8_t)0x15U) 	/* negative acknowledge */
/* Log event defines    -------------------------------------------------------*/
#define FANCOIL_RPM_SENSOR_ERROR                ((uint8_t)0xc0U)
#define FANCOIL_NTC_SENSOR_ERROR                ((uint8_t)0xc1U)
#define FANCOIL_LO_TEMP_ERROR                   ((uint8_t)0xc2U)
#define FANCOIL_HI_TEMP_ERROR                   ((uint8_t)0xc3U)
#define FANCOIL_FREEZING_PROTECTION             ((uint8_t)0xc4U)
#define THERMOSTAT_NTC_SENSOR_ERROR             ((uint8_t)0xc5U)
#define THERMOSTAT_ERROR                        ((uint8_t)0xc6U) // + 0-e = event (0xd0-0xde)

#define PIN_RESET                               ((uint8_t)0xd0U)
#define POWER_ON_RESET				            ((uint8_t)0xd1U)
#define SOFTWARE_RESET				            ((uint8_t)0xd2U)
#define IWDG_RESET					            ((uint8_t)0xd3U)
#define WWDG_RESET					            ((uint8_t)0xd4U)
#define LOW_POWER_RESET				            ((uint8_t)0xd5U)
#define FIRMWARE_UPDATED                        ((uint8_t)0xd6U)
#define FIRMWARE_UPDATE_FAIL		            ((uint8_t)0xd7U)
#define BOOTLOADER_UPDATED			            ((uint8_t)0xd8U)
#define BOOTLOADER_UPDATE_FAIL		            ((uint8_t)0xd9U)
#define IMAGE_UPDATED				            ((uint8_t)0xdaU)
#define IMAGE_UPDATE_FAIL			            ((uint8_t)0xdbU)
#define DISPLAY_FAIL				            ((uint8_t)0xdcU)
#define DRIVER_OR_FUNCTION_FAIL                 ((uint8_t)0xddU)
#define ONEWIRE_BUS_EXCESSIVE_ERROR             ((uint8_t)0xdeU)

#define NO_EVENT                	            ((uint8_t)0xe0U)
#define GUEST_CARD_VALID        	            ((uint8_t)0xe1U)
#define GUEST_CARD_INVALID      	            ((uint8_t)0xe2U)
#define HANDMAID_CARD_VALID                     ((uint8_t)0xe3U)
#define ENTRY_DOOR_CLOSED                       ((uint8_t)0xe4U)
#define PRESET_CARD					            ((uint8_t)0xe5U)
#define HANDMAID_SERVICE_END    	            ((uint8_t)0xe6U)
#define MANAGER_CARD            	            ((uint8_t)0xe7U)
#define SERVICE_CARD            	            ((uint8_t)0xe8U)
#define ENTRY_DOOR_OPENED          	            ((uint8_t)0xe9U)
#define MINIBAR_USED            	            ((uint8_t)0xeaU)
#define BALCON_DOOR_OPENED                      ((uint8_t)0xebU)
#define BALCON_DOOR_CLOSED			            ((uint8_t)0xecU)
#define CARD_STACKER_ON				            ((uint8_t)0xedU)		
#define CARD_STACKER_OFF			            ((uint8_t)0xeeU)
#define DO_NOT_DISTURB_SWITCH_ON 	            ((uint8_t)0xefU)
#define DO_NOT_DISTURB_SWITCH_OFF	            ((uint8_t)0xf0U)
#define HANDMAID_SWITCH_ON			            ((uint8_t)0xf1U)
#define HANDMAID_SWITCH_OFF			            ((uint8_t)0xf2U)
#define SOS_ALARM_TRIGGER			            ((uint8_t)0xf3U)
#define SOS_ALARM_RESET				            ((uint8_t)0xf4U)
#define FIRE_ALARM_TRIGGER			            ((uint8_t)0xf5U)
#define FIRE_ALARM_RESET          	            ((uint8_t)0xf6U)
#define UNKNOWN_CARD				            ((uint8_t)0xf7U)
#define CARD_EXPIRED				            ((uint8_t)0xf8U)
#define WRONG_ROOM					            ((uint8_t)0xf9U)
#define WRONG_SYSTEM_ID				            ((uint8_t)0xfaU)
#define CONTROLLER_RESET			            ((uint8_t)0xfbU)
#define ENTRY_DOOR_NOT_CLOSED		            ((uint8_t)0xfcU)
#define	DOOR_BELL_ACTIVE			            ((uint8_t)0xfdU)
#define	DOOR_LOCK_USER_OPEN			            ((uint8_t)0xfeU)


/* Exporeted Variable   ------------------------------------------------------*/
extern __IO uint32_t onewire_flags;
extern __IO uint8_t onewire_interface_address;


/* Exporeted  Macro  ---------------------------------------------------------*/
#define ONEWIRE_SensorConnected()					(onewire_flags |= (1U << 0))
#define ONEWIRE_SensorNotConnected()				(onewire_flags &= (~ (1U << 0)))
#define IsONEWIRE_SensorConnected()					((onewire_flags & (1U << 0)) != 0U)

#define ONEWIRE_ListenAddressSet()                  (onewire_flags |= (1U << 1))
#define ONEWIRE_ListenAddressReset()                (onewire_flags &= (~ (1U << 1)))
#define IsONEWIRE_ListenAddressActiv()              ((onewire_flags & (1U << 1)) != 0U)

#define ONEWIRE_RoomControllerConnectedSet()        (onewire_flags |= (1U << 2))
#define ONEWIRE_RoomControllerConnectedReset()      (onewire_flags &= (~ (1U << 2)))
#define IsONEWIRE_RoomControllerConnected()         ((onewire_flags & (1U << 2)) != 0U)

#define ONEWIRE_SendThermostatVariableSet()         (onewire_flags |= (1U << 3))
#define ONEWIRE_SendThermostatVariableReset()       (onewire_flags &= (~ (1U << 3)))
#define IsONEWIRE_SendThermostatVariableActiv()     ((onewire_flags & (1U << 3)) != 0U)

#define ONEWIRE_UpdateDisplayImageSet()             (onewire_flags |= (1U << 4))
#define ONEWIRE_UpdateDisplayImageReset()           (onewire_flags &= (~ (1U << 4)))
#define IsONEWIRE_UpdateDisplayImageActiv()         ((onewire_flags & (1U << 4)) != 0U)

#define ONEWIRE_HVAC_ContactorOn()                  (onewire_flags |= (1U << 5))
#define ONEWIRE_HVAC_ContactorOff()                 (onewire_flags &= (~ (1U << 5)))
#define IsONEWIRE_HVAC_ContactorOn()                ((onewire_flags & (1U << 5)) != 0U)

#define ONEWIRE_WindowSwitchClosed()                (onewire_flags |= (1U << 6))
#define ONEWIRE_WindowSwitchOpen()                  (onewire_flags &= (~ (1U << 6)))
#define IsONEWIRE_WindowSwitchClosed()              ((onewire_flags & (1U << 6)) != 0U)

#define ONEWIRE_PowerContactorOn()                  (onewire_flags |= (1U << 7))
#define ONEWIRE_PowerContactorOff()                 (onewire_flags &= (~ (1U << 7)))
#define IsONEWIRE_PowerContactorOn()                ((onewire_flags & (1U << 7)) != 0U)

#define ONEWIRE_DoorBellOn()                        (onewire_flags |= (1U << 8))
#define ONEWIRE_DoorBellOff()                       (onewire_flags &= (~ (1U << 8)))
#define IsONEWIRE_DoorBellOn()                      ((onewire_flags & (1U << 8)) != 0U)

#define ONEWIRE_HVAC_ContactorActivSet()            (onewire_flags |= (1U << 9))
#define ONEWIRE_HVAC_ContactorActivReset()          (onewire_flags &= (~ (1U << 9)))
#define IsONEWIRE_HVAC_ContactorActiv()             ((onewire_flags & (1U << 9)) != 0U)

#define ONEWIRE_WindowSwitchActivSet()              (onewire_flags |= (1U << 10))
#define ONEWIRE_WindowSwitchActivReset()            (onewire_flags &= (~ (1U << 10)))
#define IsONEWIRE_WindowSwitchActiv()               ((onewire_flags & (1U << 10)) != 0U)

#define ONEWIRE_PowerContactorActivSet()            (onewire_flags |= (1U << 11))
#define ONEWIRE_PowerContactorActivReset()          (onewire_flags &= (~ (1U << 11)))
#define IsONEWIRE_PowerContactorActiv()             ((onewire_flags & (1U << 11)) != 0U)

#define ONEWIRE_ButtonReadyForDrawingSet()          (onewire_flags |= (1U << 12))
#define ONEWIRE_ButtonReadyForDrawingReset()        (onewire_flags &= (~ (1U << 12)))
#define IsONEWIRE_ButtonReadyForDrawingActiv()      ((onewire_flags & (1U << 12)) != 0U)

#define ONEWIRE_ThermostatVariableReceivedSet()     (onewire_flags |= (1U << 13))
#define ONEWIRE_ThermostatVariableReceivedReset()   (onewire_flags &= (~ (1U << 13)))
#define IsONEWIRE_ThermostatVariableReceivedActiv() ((onewire_flags & (1U << 13)) != 0U)

#define ONEWIRE_SendNewButtonStateSet()             (onewire_flags |= (1U << 14))
#define ONEWIRE_SendNewButtonStateReset()           (onewire_flags &= (~ (1U << 14)))
#define IsONEWIRE_SendNewButtonStateActiv()         ((onewire_flags & (1U << 14)) != 0U)

#define ONEWIRE_ForceThermostatOnSet()              (onewire_flags |= (1U << 15))
#define ONEWIRE_ForceThermostatOnReset()            (onewire_flags &= (~ (1U << 15)))
#define IsONEWIRE_ForceThermostatOnActiv()          ((onewire_flags & (1U << 15)) != 0U)

#define ONEWIRE_ForceThermostatOffSet()             (onewire_flags |= (1U << 16))
#define ONEWIRE_ForceThermostatOffReset()           (onewire_flags &= (~ (1U << 16)))
#define IsONEWIRE_ForceThermostatOffActiv()         ((onewire_flags & (1U << 16)) != 0U)


/* Exported Function   -------------------------------------------------------*/
void ONEWIRE_Init(void);


#endif
/******************************   END OF FILE  **********************************/

