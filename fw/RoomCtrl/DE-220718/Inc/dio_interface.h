/**
 ******************************************************************************
 * File Name          : dio_interface.h
 * Date               : 28/02/2016 23:16:19
 * Description        : digital in/out and capacitive sensor  modul header
 ******************************************************************************
 *
 *  LUX Room Controller			->  STM32F103C8 GPIO
 * ----------------------------------------------------------------------------
 *	DIN 0  Indor Card Reader	->	PB0
 *	DIN 1  SOS Alarm Switch		->	PB1
 *	DIN 2  SOS Reset Switch		->	PB2
 *	DIN 3  Call Handmaid Switch	->	PB3
 *	DIN 4  Minibar Sensor		->	PB4
 *	DIN 5  Balcony Door Sensor	->	PB5
 *	DIN 6  DND Switch			->	PB6
 *	DIN 7  Entry Door Sensor	->	PB7
 *
 *	DOUT 0 Power Contactor		->	PA4
 *	DOUT 1 DND Modul Power		->	PA5
 *	DOUT 2 Door Bell			->	PA7
 *
 ******************************************************************************
 */
 
 
 /* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DIO_INTERFACE_H__
#define __DIO_INTERFACE_H__					221018	// version


/* Include  ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"


/* Exporeted Types   ---------------------------------------------------------*/
/* Exporeted Define   --------------------------------------------------------*/
/* Exporeted Variable   ------------------------------------------------------*/
extern __IO uint32_t dio_flags;
extern __IO uint32_t din_0_7;
extern __IO uint32_t din_state;
extern __IO uint32_t din_cap_sen;
extern __IO uint32_t dout_0_7;
extern __IO uint32_t dout_0_7_remote;
extern uint8_t buzzer_volume;
extern uint8_t doorlock_force;
extern uint8_t hc595_dout;


/* Exporeted  Macro  ---------------------------------------------------------*/
/***********************************************************************
**	 7 4 H C 5 9 5 	  S H I F T 	 R E G I S T E R 	C O N T R O L
***********************************************************************/
#define LED_HandmaidGreen_On()        	(hc595_dout |= (1U << 0))
#define LED_HandmaidGreen_Off()       	(hc595_dout &= (~(1U << 0)))
#define LED_HandmaidGreen_Toggle()    	((hc595_dout & (1U << 0)) ? LED_HandmaidGreen_Off() : LED_HandmaidGreen_On())
#define IsLED_HandmaidGreen_On()        (hc595_dout & (1U << 0))
#define LED_RoomStatusRed_On()      	(hc595_dout |= (1U << 1))
#define LED_RoomStatusRed_Off()     	(hc595_dout &= (~(1U << 1)))
#define LED_RoomStatusRed_Toggle() 		((hc595_dout & (1U << 1)) ? LED_RoomStatusRed_Off() : LED_RoomStatusRed_On())
#define LED_DoorBellBlue_On()     		(hc595_dout |= (1U << 2))
#define LED_DoorBellBlue_Off()    		(hc595_dout &= (~(1U << 2)))
#define LED_DoorBellBlue_Toggle()		((hc595_dout & (1U << 2)) ? LED_DoorBellBlue_Off() : LED_DoorBellBlue_On())
#define LED_RfidReaderWhite_On()        (hc595_dout |= (1U << 3))
#define LED_RfidReaderWhite_Off()       (hc595_dout &= (~(1U << 3)))
#define LED_RfidReaderWhite_Toggle()    ((hc595_dout & (1U << 3)) ? LED_RfidReaderWhite_Off() : LED_RfidReaderWhite_On())
#define DOUT_3_SetHigh()				(hc595_dout |= (1U << 4))
#define DOUT_3_SetLow()					(hc595_dout &= (~(1U << 4)))
#define Get_DOUT_3_State()				(hc595_dout & (1U << 4))
#define DOUT_4_SetHigh()				(hc595_dout |= (1U << 5))
#define DOUT_4_SetLow()					(hc595_dout &= (~(1U << 5)))
#define Get_DOUT_4_State()				(hc595_dout &  (1U << 5))
#define DISPLAY_RST_SetHigh()			(hc595_dout |= (1U << 6))
#define DISPLAY_RST_SetLow()			(hc595_dout &= (~(1U << 6)))
#define RC522_RST_SetHigh()				(hc595_dout |= (1U << 7))
#define RC522_RST_SetLow()				(hc595_dout &= (~(1U << 7)))
/***********************************************************************
**	 		A U X I L I A R Y	 			F L A G S 		
***********************************************************************/
#define DoNotDisturb_On()				(dio_flags |= (1U << 0))
#define DoNotDisturb_Off()				(dio_flags &= (~ (1U << 0)))
#define IsDonNotDisturbActiv()			((dio_flags & (1U << 0)) != 0U)

#define DoorBellSwitchPressed()			(dio_flags |= (1U << 1))
#define DoorBellSwitchReleased()		(dio_flags &= (~ (1U << 1)))
#define IsDoorBellSwitchPressed()		((dio_flags & (1U << 1)) != 0U)

#define HandmaidSwitchPressed()			(dio_flags |= (1U << 2))
#define HandmaidSwitchReleased()		(dio_flags &= (~ (1U << 2)))
#define IsHandmaidSwitchPressed()		((dio_flags & (1U << 2)) != 0U)

#define SosAlarm_On()					(dio_flags |= (1U << 3))
#define SosAlarm_Off()					(dio_flags &= (~ (1U << 3)))
#define IsSosAlarmActiv()				((dio_flags & (1U << 3)) != 0U)

#define EntranceDoorSensorEnabled()		(dio_flags |= (1U << 4))
#define EntranceDoorSensorDisabled()	(dio_flags &= (~ (1U << 4)))
#define IsEntranceDoorSensorActiv()		((dio_flags & (1U << 4)) != 0U)

#define HandmaidCall_On()				(dio_flags |= (1U << 5))
#define HandmaidCall_Off()				(dio_flags &= (~ (1U << 5)))
#define IsHandmaidCallActiv()			((dio_flags & (1U << 5)) != 0U)

#define DoorlockRemoteOpenSet()         (dio_flags |= (1U << 6))
#define DoorlockRemoteOpenReset()       (dio_flags &= (~ (1U << 6)))
#define IsDoorlockRemoteOpenActiv()     ((dio_flags & (1U << 6)) != 0U)

#define WaterFloodSensorEnabled()		(dio_flags |= (1U << 7))
#define WaterFloodSensorDisabled()      (dio_flags &= (~ (1U << 7)))
#define IsWaterFloodSensorActivated()   ((dio_flags & (1U << 7)) != 0U)

#define DIN_ForcingEnabled()            (dio_flags |= (1U << 8))
#define DIN_ForcingDisabled()           (dio_flags &= (~ (1U << 8)))
#define IsDIN_ForcingActivated()        ((dio_flags & (1U << 8)) != 0U)
/***********************************************************************
**	 D I G I T A L		 I N P U T		0 ~	7		S T A T E S		
***********************************************************************/
#define IsIndorCardReaderActiv()        ((din_0_7 & (1U << 0)) != 0U)
#define IsSosAlarmSwitchActiv()         ((din_0_7 & (1U << 1)) == 0U)
#define IsSosResetSwitchActiv()         ((din_0_7 & (1U << 2)) != 0U)
#define IsHandmaidRequested()           ((din_0_7 & (1U << 3)) == 0U)
#define IsMinibarSensorActiv()          ((din_0_7 & (1U << 4)) == 0U)
#define IsBalconyDoorSensorActiv()      ((din_0_7 & (1U << 5)) == 0U)
#define IsDoNotDisturbSwitchActiv() 	((din_0_7 & (1U << 6)) == 0U)
#define IsWaterFloodSensorActiv()       ((din_0_7 & (1U << 7)) != 0U)
/***********************************************************************
**	 C A P A C I T I V E		S E N S O R 		S T A T E S		
***********************************************************************/
#define IsHandmaidSwitchActiv()			((din_cap_sen & (1U << 0)) != 0U)
#define IsDoorBellSwitchActiv()			((din_cap_sen & (1U << 2)) != 0U)
#define CAP1293_SensorPresent()			(din_cap_sen |=(1U << 7))
#define CAP1293_SensorNotPresent()		(din_cap_sen &=(~(1U << 7)))
#define IsCAP1293_Present()				((din_cap_sen & (1U << 7)) != 0U)
/***********************************************************************
**	 D I G I T A L		O U T P U T		0 ~	7		C O N T R O L	
***********************************************************************/
#define PowerContactor_On()             (dout_0_7 &= (~(1U << 0)))
#define PowerContactor_Off()            (dout_0_7 |= (1U << 0))
#define IsPowerContactorActiv()			((dout_0_7 & (1U << 0)) == 0U)
#define DoNotDisturbModul_On()        	(dout_0_7 |= (1U << 1))
#define DoNotDisturbModul_Off()   		(dout_0_7 &= (~(1U << 1)))
#define IsDoNotDisturbModulActiv()		((dout_0_7 &  (1U << 1)) != 0U)
#define WellcomeLight_On()              (dout_0_7 |= (1U << 2))
#define WellcomeLight_Off()             (dout_0_7 &= (~(1U << 2)))
#define IsWellcomeLightActiv()          ((dout_0_7 &  (1U << 2)) != 0U)
#define DoorBell_On()              		(dout_0_7 |= (1U << 3))
#define DoorBell_Off()             		(dout_0_7 &= (~(1U << 3)))
#define IsDoorBellActiv()              	((dout_0_7 &  (1U << 3)) != 0U)
#define HVAC_Contactor_On()             (dout_0_7 |= (1U << 4))
#define HVAC_Contactor_Off()            (dout_0_7 &= (~(1U << 4)))
#define IsHVAC_ContactorActiv()         ((dout_0_7 &  (1U << 4)) != 0U)
#define HVAC_Thermostat_On()			(dout_0_7 |= (1U << 5))
#define HVAC_Thermostat_Off()			(dout_0_7 &= (~(1U << 5)))
#define IsHVAC_ThermorstatActiv()		((dout_0_7 &  (1U << 5)) != 0U)
#define DoorLockCoil_On()               (dout_0_7 |= (1U << 6))
#define DoorLockCoil_Off()              (dout_0_7 &= (~(1U << 6)))
#define IsDoorLockCoilActiv()			((dout_0_7 &  (1U << 6)) != 0U)
#define Buzzer_On()                     (dout_0_7 |= (1U << 7))
#define Buzzer_Off()                    (dout_0_7 &= (~(1U << 7)))
#define IsBuzzerActiv()					((dout_0_7 &  (1U << 7)) != 0U)


/* Exported Function   -------------------------------------------------------*/
void DIO_Init(void);
void DIO_Service(void);
void DIO_SetOuput(void);


#endif

/******************************   END OF FILE  **********************************/
