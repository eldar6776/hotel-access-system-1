/**
 ******************************************************************************
 * File Name          : one_wire.c
 * Date               : 17/11/2016 00:59:00
 * Description        : one wire communication modul header
 ******************************************************************************
 */
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ONEWIRE_H__
#define __ONEWIRE_H__					221018	// version


/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"


/* Exporeted Types   ---------------------------------------------------------*/
typedef struct
{
	uint8_t	sensor_id;
	uint8_t rom_code[8];
	int temperature;
	
}TempSensorTypeDef;


/* Exporeted Define   --------------------------------------------------------*/
#define ONEWIRE_PACKET_SIZE							64U
#define ONEWIRE_RX_TX_DELAY                         2U
#define ONEWIRE_TRANSFER_TIMEOUT					30U
#define ONEWIRE_UPDATE_TIME                         89U
#define ONEWIRE_PENDING_TIMEOUT						567U
#define ONEWIRE_POWER_ON_DELAY                      5678U
#define ONEWIRE_MAX_TRANSFER_ERROR                  10U
#define ONEWIRE_THERMOSTAT_FIRST_ADDRESS            1U
#define ONEWIRE_THERMOSTAT_LAST_ADDRESS             9U
#define ONEWIRE_INTERFACE_ADDRESS                   10U
#define ONEWIRE_2400								((uint32_t)2400U)
#define ONEWIRE_4800								((uint32_t)4800U)
#define ONEWIRE_9600								((uint32_t)9600U)
#define ONEWIRE_19200								((uint32_t)19200U)
#define ONEWIRE_38400								((uint32_t)38400U)
#define ONEWIRE_57600								((uint32_t)57600U)
#define ONEWIRE_115200								((uint32_t)115200U)



/* Exporeted Variable   ------------------------------------------------------*/
extern uint32_t onewire_flags;
extern uint8_t onewire_device_connected;
extern uint8_t onewire_thermostat_address;
extern uint16_t temperature_measured;
extern uint8_t temperature_setpoint;
extern uint8_t temperature_difference;
extern uint8_t thermostat_id;
extern uint8_t thermostat_addresse[9];
extern uint8_t thermostat_setpoint[9];
extern uint8_t thermostat_ambient_temperature[9];
extern uint8_t thermostat_fancoil_temperature[9];
extern uint8_t thermostat_display_image_id[9];
extern uint8_t thermostat_display_image_time[9];
extern uint8_t thermostat_buzzer_repeat_time[9];
extern uint8_t thermostat_buzzer_sound_mode[9];


/* Exporeted  Macro  ---------------------------------------------------------*/
#define ONEWIRE_DalasSensorConnected()              (onewire_flags |= (1U << 0))
#define ONEWIRE_DalasSensorNotConnected()           (onewire_flags &= (~ (1U << 0)))
#define IsONEWIRE_DalasSensorConnected()            ((onewire_flags & (1U << 0)) != 0U)

#define ONEWIRE_ThermostatConnected()               (onewire_flags |= (1U << 1))
#define ONEWIRE_ThermostatNotConnected()            (onewire_flags &= (~ (1U << 1)))
#define IsONEWIRE_ThermostatConnected()             ((onewire_flags & (1U << 1)) != 0U)

#define ONEWIRE_AmbientNtcSensorConnected()         (onewire_flags |= (1U << 2))
#define ONEWIRE_AmbientNtcSensorNotConnected()      (onewire_flags &= (~ (1U << 2)))
#define IsONEWIRE_AmbientNtcSensorConnected()       ((onewire_flags & (1U << 2)) != 0U)

#define ONEWIRE_FancoilNtcSensorConnected()         (onewire_flags |= (1U << 3))
#define ONEWIRE_FancoilNtcSensorNotConnected()      (onewire_flags &= (~ (1U << 3)))
#define IsONEWIRE_FancoilNtcSensorConnected()       ((onewire_flags & (1U << 3)) != 0U)

#define ONEWIRE_UpdateLockSet()                     (onewire_flags |= (1U << 4))
#define ONEWIRE_UpdateLockReset()                   (onewire_flags &= (~ (1U << 4)))
#define IsONEWIRE_UpdateLockActiv()                 ((onewire_flags & (1U << 4)) != 0U)

#define ONEWIRE_TimeUpdateSet()                     (onewire_flags |= (1U << 5))
#define ONEWIRE_TimeUpdateReset()                   (onewire_flags &= (~ (1U << 5)))
#define IsONEWIRE_TimeUpdateActiv()                 ((onewire_flags & (1U << 5)) != 0U)

#define ONEWIRE_DisplayUpdateSet()                  ((onewire_flags |= (1U << 6)), ONEWIRE_UpdateLockSet())
#define ONEWIRE_DisplayUpdateReset()                (onewire_flags &= (~ (1U << 6)))
#define IsONEWIRE_DisplayUpdateActiv()              ((onewire_flags & (1U << 6)) != 0U)

#define ONEWIRE_SetpointUpdatetSet()                ((onewire_flags |= (1U << 7)), ONEWIRE_UpdateLockSet())
#define ONEWIRE_SetpointUpdateReset()               (onewire_flags &= (~ (1U << 7)))
#define IsONEWIRE_SetpointUpdateActiv()             ((onewire_flags & (1U << 7)) != 0U)

#define ONEWIRE_ButtonUpdateSet()                   ((onewire_flags |= (1U << 8)), ONEWIRE_UpdateLockSet())
#define ONEWIRE_ButtonUpdateReset()                 (onewire_flags &= (~ (1U << 8)))
#define IsONEWIRE_ButtonUpdateActiv()               ((onewire_flags & (1U << 8)) != 0U)

#define ONEWIRE_ClearErrorSet()                     ((onewire_flags |= (1U << 9)), ONEWIRE_UpdateLockSet())
#define ONEWIRE_ClearErrorReset()                   (onewire_flags &= (~ (1U << 9)))
#define IsONEWIRE_ClearErrorActiv()                 ((onewire_flags & (1U << 9)) != 0U)

#define ONEWIRE_UpdateSelectetdSet()                (onewire_flags |= (1U << 10))
#define ONEWIRE_UpdateSelectetdReset()              (onewire_flags &= (~ (1U << 10)))
#define IsONEWIRE_UpdateSelectetdActiv()            ((onewire_flags & (1U << 10)) != 0U)

#define TemperatureRegulatorOn()					(temperature_setpoint   |=(1U << 7))
#define TemperatureRegulatorOff()					(temperature_setpoint   &=(~ (1U << 7)))
#define IsTemperatureRegulatorOn()					((temperature_setpoint  & (1U << 7)) != 0U)
#define IsTemperatureRegulatorHeating()				((temperature_setpoint  & (1U << 6))!= 0U)
#define IsTemperatureRegulatorCooling()				((temperature_setpoint  & (1U << 6)) == 0U)
#define TemperatureRegulatorOneCycleOn()			(temperature_difference |=(1U << 7))
#define TemperatureRegulatorOneCycleOff()			(temperature_difference &=(~ (1U << 7)))
#define IsTemperatureRegulatorOneCycleOn()			((temperature_difference & (1U << 7)) != 0U)


/* Exported Function   -------------------------------------------------------*/
void ONEWIRE_Init(void);
void ONEWIRE_Service(void);
void ONEWIRE_ScanBus(void);
void ONEWIRE_ThermostatDisplayDoorBell(void);
void ONEWIRE_ThermostatDisplayUserInterface(void);
void ONEWIRE_ThermostatDisplayConditionError(void);
uint8_t IsONEWIRE_ThermostatDisplayDoorBellActive(void);


#endif
/******************************   END OF FILE  **********************************/

