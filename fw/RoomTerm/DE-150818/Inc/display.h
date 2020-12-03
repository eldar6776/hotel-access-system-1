/**
 ******************************************************************************
 * File Name          : display.h
 * Date               : 10.3.2018
 * Description        : GUI Display Module Header
 ******************************************************************************
 *
 *
 ******************************************************************************
 */
 
 
 /* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DISPLAY_H__
#define __DISPLAY_H__                            RT200119    // firmware version


/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx.h"
#include "GUI.h"


/* Exported Define -----------------------------------------------------------*/
#define DISPLAY_BRIGHTNESS_HIGH                     500U
#define DISPLAY_BRIGHTNESS_LOW                      50U


/* Exported types ------------------------------------------------------------*/
typedef enum	// display button states
{
	RELEASED    = 0U,
	PRESSED     = 1U,
    BUTTON_SHIT = 2U
	
}BUTTON_StateTypeDef;


#define GUI_ID_BUTTON_Dnd   			                        0x801
#define GUI_ID_BUTTON_Sos   			                        0x802
#define GUI_ID_BUTTON_Maid   			                        0x803
#define GUI_ID_BUTTON_Increase                                  0x804
#define GUI_ID_BUTTON_Decrease                                  0x805
#define GUI_ID_BUTTON_Ok   			                            0x806
#define GUI_ID_BUTTON_DoorOpen                                  0x807
#define GUI_ID_BUTTON_Next                                      0x808

#define GUI_ID_SPINBOX_FancoillNtcOffset                        0x820
#define GUI_ID_SPINBOX_AmbientNtcOffset                         0x821
#define GUI_ID_SPINBOX_ErrorDurationTime                        0x822
#define GUI_ID_SPINBOX_CoolingMaxTemperature                    0x823
#define GUI_ID_SPINBOX_HeatingMinTemperature                    0x824
#define GUI_ID_SPINBOX_OneWireInterfaceAddress                  0x825
#define GUI_ID_SPINBOX_Rs485InterfaceAddress                    0x826
#define GUI_ID_SPINBOX_FancoilFanSpeedTreshold                  0x827
#define GUI_ID_SPINBOX_FancoilFanLowSpeedBand                   0x828
#define GUI_ID_SPINBOX_FancoilFanMiddleSpeedBand                0x829
#define GUI_ID_SPINBOX_FreezingProtectionTriggerTemperature     0x82a
#define GUI_ID_SPINBOX_FreezingProtectionSetpointTemperature    0x82b
#define GUI_ID_SPINBOX_ThermostatMaxSetpointTemperature         0x82c
#define GUI_ID_SPINBOX_ThermostatMinSetpointTemperature         0x82d
#define GUI_ID_SPINBOX_FancoilNightQuietModeStart               0x82e
#define GUI_ID_SPINBOX_FreezingProtectionTemperature            0x82f

#define GUI_ID_CHECK_AmbientNtcErrorReport                      0x840
#define GUI_ID_CHECK_FancoilNtcErrorReport                      0x841
#define GUI_ID_CHECK_FancoilCoolingErrorReport                  0x842
#define GUI_ID_CHECK_FancoilHeatingErrorReport                  0x843
#define GUI_ID_CHECK_AmbientNtcErrorShutdown                    0x844
#define GUI_ID_CHECK_FancoilNtcErrorShutdown                    0x845
#define GUI_ID_CHECK_FancoilCoolingErrorShutdown                0x846
#define GUI_ID_CHECK_FancoilHeatingErrorShutdown                0x847
#define GUI_ID_CHECK_FancoilErrorAutorestart                    0x848
#define GUI_ID_CHECK_FancoilValveShutdownOnSetpoint             0x849
#define GUI_ID_CHECK_FancoilFilterDirtyMonitor                  0x84a
#define GUI_ID_CHECK_FancoilFilterDirtyReport                   0x84b
#define GUI_ID_CHECK_FancoilFilterDirtyShutdown                 0x84c    
#define GUI_ID_CHECK_FreezingProtection                         0x84d
#define GUI_ID_CHECK_FreezingProtectionReport                   0x84e
#define GUI_ID_CHECK_FancoilRpmSensor                           0x84f
#define GUI_ID_CHECK_FancoilRpmSensorErrorReport                0x850
#define GUI_ID_CHECK_FancoilRpmSensorErrorShutdown              0x851
#define GUI_ID_CHECK_FancoilNightQuietMode                      0x852

#define GUI_ID_RADIO_TemperatureLimitType                       0x860
#define GUI_ID_RADIO_ThermostatModeSelect                       0x861
#define GUI_ID_RADIO_FancoilFanControlType                      0x862

#define GUI_ID_EDIT_TimeSet                                     0x870
#define GUI_ID_EDIT_DateSet                                     0x871

#define GUI_ID_ICONVIEW_DemoMenu                                0x880

/* Exported variables  -------------------------------------------------------*/
extern __IO uint32_t display_flags;
extern __IO uint8_t display_image_id, display_image_time;


/* Exported macros     -------------------------------------------------------*/
#define DISPLAY_UpdateSet()                         (display_flags |= (1U << 0))
#define DISPLAY_UpdateReset()                       (display_flags &= (~ (1U << 0)))
#define IsDISPLAY_UpdateActiv()                     ((display_flags & (1U << 0)) != 0U)

#define BUTTON_DndActivSet()                        (display_flags |= (1U << 1)) 
#define BUTTON_DndActivReset()                      (display_flags &= (~ (1U << 1)))
#define IsBUTTON_DndActiv()                         ((display_flags & (1U << 1)) != 0U)

#define BUTTON_SosActivSet()                        (display_flags |= (1U << 2)) 
#define BUTTON_SosActivReset()                      (display_flags &= (~ (1U << 2)))
#define IsBUTTON_SosResetActiv()                    ((display_flags & (1U << 2)) != 0U)

#define BUTTON_CallMaidActivSet()                   (display_flags |= (1U << 3)) 
#define BUTTON_CallMaidActivReset()                 (display_flags &= (~ (1U << 3)))
#define IsBUTTON_CallMaidActiv()                    ((display_flags & (1U << 3)) != 0U)

#define DISPLAY_SetpointUpdateSet()                 (display_flags |= (1U << 4)) 
#define DISPLAY_SetpointUpdateReset()               (display_flags &= (~ (1U << 4)))
#define IsDISPLAY_SetpointUpdateActiv()             ((display_flags & (1U << 4)) != 0U)

#define BUTTON_OpenDoorSet()                        (display_flags |= (1U << 5)) 
#define BUTTON_OpenDoorReset()                      (display_flags &= (~ (1U << 5)))
#define IsBUTTON_OpenDoorActiv()                    ((display_flags & (1U << 5)) != 0U)

#define BUTTON_OkActivSet()                         (display_flags |= (1U << 6)) 
#define BUTTON_OkActivReset()                       (display_flags &= (~ (1U << 6)))
#define IsBUTTON_OkActiv()                          ((display_flags & (1U << 6)) != 0U)

#define DISPLAY_ScreensaverSet()                    (display_flags |= (1U << 7)) 
#define DISPLAY_ScreensaverReset()                  (display_flags &= (~ (1U << 7)))
#define IsDISPLAY_ScreensaverActive()               ((display_flags & (1U << 7)) != 0U)

#define BUTTON_StateChangedSet()                    (display_flags |= (1U << 8)) 
#define BUTTON_StateChangedReset()                  (display_flags &= (~ (1U << 8)))
#define IsBUTTON_StateChangedActiv()                ((display_flags & (1U << 8)) != 0U)

#define DISPLAY_AmbientTemperatureUpdateSet()       (display_flags |= (1U << 9)) 
#define DISPLAY_AmbientTemperatureUpdateReset()     (display_flags &= (~ (1U << 9)))
#define IsDISPLAY_AmbientTemperatureUpdated()       ((display_flags & (1U << 9)) != 0U)

#define DISPLAY_FancoilTemperatureUpdateSet()       (display_flags |= (1U << 10)) 
#define DISPLAY_FancoilTemperatureUpdateReset()     (display_flags &= (~ (1U << 10)))
#define IsDISPLAY_FancoilTemperatureUpdated()       ((display_flags & (1U << 10)) != 0U)

#define DISPLAY_AmbientNtcSensorStateSet()          (display_flags |= (1U << 11)) 
#define DISPLAY_AmbientNtcSensorStateReset()        (display_flags &= (~ (1U << 11)))
#define IsDISPLAY_AmbientNtcSensorStateUpdated()    ((display_flags & (1U << 11)) != 0U)

#define DISPLAY_FancoilNtcSensorStateSet()          (display_flags |= (1U << 12)) 
#define DISPLAY_FancoilNtcSensorStateReset()        (display_flags &= (~ (1U << 12)))
#define IsDISPLAY_FancoilNtcSensorStateUpdated()    ((display_flags & (1U << 12)) != 0U)

#define DISPLAY_DoorBellSet()                       (display_flags |= (1U << 13))
#define DISPLAY_DoorBellReset()                     (display_flags &= (~ (1U << 13)))
#define IsDISPLAY_DoorBellActiv()                   ((display_flags & (1U << 13)) != 0U)

#define DISPLAY_ConditionErrorSet()                 (display_flags |= (1U << 14))
#define DISPLAY_ConditionErrorReset()               (display_flags &= (~ (1U << 14)))
#define IsDISPLAY_ConditionErrorActiv()             ((display_flags & (1U << 14)) != 0U)

#define DISPLAY_UserInterfaceSet()                  (display_flags |= (1U << 15))
#define DISPLAY_UserInterfaceReset()                (display_flags &= (~ (1U << 15)))
#define IsDISPLAY_UserInterfaceActiv()              ((display_flags & (1U << 15)) != 0U)

#define DISPLAY_UserInformationSet()                (display_flags |= (1U << 16))
#define DISPLAY_UserInformationReset()              (display_flags &= (~ (1U << 16)))
#define IsDISPLAY_UserInformationActiv()            ((display_flags & (1U << 16)) != 0U)


/* Exported functions  -------------------------------------------------------*/
void DISPLAY_Init(void);
void DISPLAY_Service(void);


#endif
/******************************   END OF FILE  **********************************/
