/**
 ******************************************************************************
 * File Name          : thermostat.h
 * Date               : 22/02/2018 07:03:00
 * Description        : temperature controller header
 ******************************************************************************
 */
 
 
 /* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TEMP_CTRL_H__
#define __TEMP_CTRL_H__                              301018		// date version


/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx.h"


/* Exported Type  ------------------------------------------------------------*/
/* Exported Define  ----------------------------------------------------------*/
#define FAN_OFF							            0U
#define FAN_CONST_SPEED_LOW                         72U
#define FAN_CONST_SPEED_MIDDLE                      66U
#define FAN_CONST_SPEED_HIGH                        60U   
#define FANCOIL_RPM_MEASURE_TIME                    1234U
#define FANCOIL_NTC_MEASURING_DELAY_TIME            600000U // 10 min. (x60 sec x 1000 ms)      
#define FANCOIL_FAN_MIN_ON_TIME                     560U    // 0,5s between two or on/off fan speed switching
#define FANCOIL_VALVE_MIN_ON_TIME                   5678U	// cooling valve state swith 5,5s min. time	
#define FANCOIL_FREEZING_PROTECTION_SETPOINT        10U     // 10,0*C set point when freezing protection activated
#define FANCOIL_NTC_UPDATE_TIME                     2345U   // 2.3 second fancoil ntc temperature update rate
#define THERMOSTAT_POWER_ON_DELAY_TIME              3456U   // 2,5 sec. power on startup time
#define FAN_BLDC_MOTOR_CONTROL_LOOP_TIME            100     // 100ms speed loop time for bldc motor fan
#define FAN_BLDC_MOTOR_CONTROL_TRESHOLD             3       // o,3 degree controller treshold

/* Exported Variable   -------------------------------------------------------*/
extern __IO uint8_t fan_speed;
extern __IO uint32_t thermostat_flags;
extern __IO uint16_t room_temperature;
extern __IO uint16_t fancoil_temperature;
extern __IO uint16_t fancoil_ntc_temperature;
extern __IO uint16_t ambient_ntc_temperature;
extern __IO uint16_t thermostat_min_setpoint_temperature;
extern __IO uint16_t thermostat_max_setpoint_temperature;
extern __IO uint16_t thermostat_min_heating_temperature;
extern __IO uint16_t thermostat_max_cooling_temperature;
extern __IO uint16_t thermostat_error_duraton;
extern __IO int16_t thermostat_ambient_ntc_offset;
extern __IO int16_t thermostat_fancoil_ntc_offset;
extern __IO uint8_t fancoil_temperature_limit_mode;
extern __IO uint8_t thermostat_operation_mode;              //  thermostat operation off/cooling/heating/remote
extern __IO uint8_t fan_speed_treshold;                     //  +/-0,3°C treshold for fan speed change 
extern __IO uint8_t fan_low_speed_band;                     //  set point +/- 0,8°C low speed fan zone
extern __IO uint8_t fan_middle_speed_band;                  //  set point +/- 1,8°C middle speed fan zone
extern __IO uint8_t fan_control_type;
extern __IO uint8_t freezing_protection_temperature;
extern __IO uint8_t fan_night_quiet_mode_start_hour;
extern uint8_t temperature_setpoint;


/* Exported Macro ------------------------------------------------------------*/
#define THERMOSTAT_ControllerOn()                       (thermostat_flags |= (1U << 0))
#define THERMOSTAT_ControllerOff()                      (thermostat_flags &= (~ (1U << 0)))
#define IsTHERMOSTAT_ControllerActiv()                  ((thermostat_flags & (1U << 0)) != 0U)

#define AMBIENT_NTC_SensorConnected()                   (thermostat_flags |= (1U << 1))
#define AMBIENT_NTC_SensorNotConnected()                (thermostat_flags &= (~ (1U << 1)))
#define IsAMBIENT_NTC_SensorConnected()                 ((thermostat_flags & (1U << 1)) != 0U)

#define FANCOIL_NTC_SensorConnected()                   (thermostat_flags |= (1U << 2))
#define FANCOIL_NTC_SensorNotConnected()                (thermostat_flags &= (~ (1U << 2)))
#define IsFANCOIL_NTC_SensorConnected()                 ((thermostat_flags & (1U << 2)) != 0U)

#define FANCOIL_CoolingValveSet()                       (thermostat_flags |= (1U << 3))
#define FANCOIL_CoolingValveReset()                     (thermostat_flags &= (~ (1U << 3)))
#define IsFANCOIL_CoolingValveActiv()                   ((thermostat_flags & (1U << 3)) != 0U)

#define FANCOIL_NTC_SensorErrorSet()                    (thermostat_flags |= (1U << 4))
#define FANCOIL_NTC_SensorErrorReset()                  (thermostat_flags &= (~ (1U << 4)))
#define IsFANCOIL_NTC_SensorErrorActiv()                ((thermostat_flags & (1U << 4)) != 0U)

#define FANCOIL_HeatingTempErrorSet()                   (thermostat_flags |= (1U << 5))
#define FANCOIL_HeatingTempErrorReset()                 (thermostat_flags &= (~ (1U << 5)))
#define IsFANCOIL_HeatingTempErrorActiv()               ((thermostat_flags & (1U << 5)) != 0U)

#define FANCOIL_CoolingTempErrorSet()                   (thermostat_flags |= (1U << 6))
#define FANCOIL_CoolingTempErrorReset()                 (thermostat_flags &= (~ (1U << 6)))
#define IsFANCOIL_CoolingTempErrorActiv()               ((thermostat_flags & (1U << 6)) != 0U)

#define FANCOIL_FreezingAlarmSet()                      (thermostat_flags |= (1U << 7))
#define FANCOIL_FreezingAlarmReset()                    (thermostat_flags &= (~ (1U << 7)))
#define IsFANCOIL_FreezingAlarmActiv()                  ((thermostat_flags & (1U << 7)) != 0U)

#define AMBIENT_NTC_SensorErrorSet()                    (thermostat_flags |= (1U << 8))
#define AMBIENT_NTC_SensorErrorReset()                  (thermostat_flags &= (~ (1U << 8)))
#define IsAMBIENT_NTC_SensorErrorActiv()                ((thermostat_flags & (1U << 8)) != 0U)

#define THERMOSTAT_OnecycleHeatingSet()                 (thermostat_flags |= (1U << 9))
#define THERMOSTAT_OnecycleHeatingReset()               (thermostat_flags &= (~ (1U << 9)))
#define IsTHERMOSTAT_OnecycleHeatingActiv()             ((thermostat_flags & (1U << 9)) != 0U)

#define FANCOIL_NightQuietModeSet()                     (thermostat_flags |= (1U << 10)) 
#define FANCOIL_NightQuietModeReset()                   (thermostat_flags &= (~ (1U << 10)))
#define IsFANCOIL_NightQuietModeActiv()                 ((thermostat_flags & (1U << 10)) != 0U)

#define FANCOIL_RpmSensorErrorShutdownSet()             (thermostat_flags |= (1U << 11)) 
#define FANCOIL_RpmSensorErrorShutdownReset()           (thermostat_flags &= (~ (1U << 11)))
#define IsFANCOIL_RpmSensorErrorShutdownActiv()         ((thermostat_flags & (1U << 11)) != 0U)

#define FANCOIL_RpmSensorErrorReportSet()               (thermostat_flags |= (1U << 12)) 
#define FANCOIL_RpmSensorErrorReportReset()             (thermostat_flags &= (~ (1U << 12)))
#define IsFANCOIL_RpmSensorErrorReportActiv()           ((thermostat_flags & (1U << 12)) != 0U)

#define FANCOIL_RpmSensorSet()                          (thermostat_flags |= (1U << 13)) 
#define FANCOIL_RpmSensorReset()                        (thermostat_flags &= (~ (1U << 13)))
#define IsFANCOIL_RpmSensorActiv()                      ((thermostat_flags & (1U << 13)) != 0U)

#define FANCOIL_FreezingProtectionReportSet()           (thermostat_flags |= (1U << 14)) 
#define FANCOIL_FreezingProtectionReportReset()         (thermostat_flags &= (~ (1U << 14)))
#define IsFANCOIL_FreezingProtectionReportActiv()       ((thermostat_flags & (1U << 14)) != 0U)

#define FANCOIL_FreezingProtectionSet()                 (thermostat_flags |= (1U << 15)) 
#define FANCOIL_FreezingProtectionReset()               (thermostat_flags &= (~ (1U << 15)))
#define IsFANCOIL_FreezingProtectionActiv()             ((thermostat_flags & (1U << 15)) != 0U)

#define FANCOIL_FilterDirtyShutdownSet()                (thermostat_flags |= (1U << 16)) 
#define FANCOIL_FilterDirtyShutdownReset()              (thermostat_flags &= (~ (1U << 16)))
#define IsFANCOIL_FilterDirtyShutdownActiv()            ((thermostat_flags & (1U << 16)) != 0U)

#define FANCOIL_FilterDirtyReportSet()                  (thermostat_flags |= (1U << 17)) 
#define FANCOIL_FilterDirtyReportReset()                (thermostat_flags &= (~ (1U << 17)))
#define IsFANCOIL_FilterDirtyReportActiv()              ((thermostat_flags & (1U << 17)) != 0U)

#define FANCOIL_FilterDirtyMonitorSet()                 (thermostat_flags |= (1U << 18)) 
#define FANCOIL_FilterDirtyMonitorReset()               (thermostat_flags &= (~ (1U << 18)))
#define IsFANCOIL_FilterDirtyMonitorActiv()             ((thermostat_flags & (1U << 18)) != 0U)

#define FANCOIL_ValveFollowFanSet()                     (thermostat_flags |= (1U << 19)) 
#define FANCOIL_ValveFollowFanReset()                   (thermostat_flags &= (~ (1U << 19)))
#define IsFANCOIL_ValveFollowFanActiv()                 ((thermostat_flags & (1U << 19)) != 0U)

#define FANCOIL_ErrorAutorestartSet()                   (thermostat_flags |= (1U << 20)) 
#define FANCOIL_ErrorAutorestartReset()                 (thermostat_flags &= (~ (1U << 20)))
#define IsFANCOIL_ErrorAutorestartActiv()               ((thermostat_flags & (1U << 20)) != 0U)

#define FANCOIL_CoolingErrorReportSet()                 (thermostat_flags |= (1U << 21)) 
#define FANCOIL_CoolingErrorReportReset()               (thermostat_flags &= (~ (1U << 21)))
#define IsFANCOIL_CoolingErrorReportActiv()             ((thermostat_flags & (1U << 21)) != 0U)

#define FANCOIL_HeatingErrorReportSet()                 (thermostat_flags |= (1U << 22)) 
#define FANCOIL_HeatingErrorReportReset()               (thermostat_flags &= (~ (1U << 22)))
#define IsFANCOIL_HeatingErrorReportActiv()             ((thermostat_flags & (1U << 22)) != 0U)

#define FANCOIL_NtcErrorReportSet()                     (thermostat_flags |= (1U << 23)) 
#define FANCOIL_NtcErrorReportReset()                   (thermostat_flags &= (~ (1U << 23)))
#define IsFANCOIL_NtcErrorReportActiv()                 ((thermostat_flags & (1U << 23)) != 0U)

#define THERMOSTAT_AmbientNtcErrorReportSet()           (thermostat_flags |= (1U << 24)) 
#define THERMOSTAT_AmbientNtcErrorReportReset()         (thermostat_flags &= (~ (1U << 24)))
#define IsTHERMOSTAT_AmbientNtcErrorReportActiv()       ((thermostat_flags & (1U << 24)) != 0U)

#define FANCOIL_CoolingErrorShutdownSet()               (thermostat_flags |= (1U << 25)) 
#define FANCOIL_CoolingErrorShutdownReset()             (thermostat_flags &= (~ (1U << 25)))
#define IsFANCOIL_CoolingErrorShutdownActiv()           ((thermostat_flags & (1U << 25)) != 0U)

#define FANCOIL_HeatingErrorShutdownSet()               (thermostat_flags |= (1U << 26)) 
#define FANCOIL_HeatingErrorShutdownReset()             (thermostat_flags &= (~ (1U << 26)))
#define IsFANCOIL_HeatingErrorShutdownActiv()           ((thermostat_flags & (1U << 26)) != 0U)

#define FANCOIL_FancoilNtcErrorShutdownSet()            (thermostat_flags |= (1U << 27)) 
#define FANCOIL_FancoilNtcErrorShutdownReset()          (thermostat_flags &= (~ (1U << 27)))
#define IsFANCOIL_FancoilNtcErrorShutdownActiv()        ((thermostat_flags & (1U << 27)) != 0U)

#define THERMOSTAT_AmbientNtcErrorShutdownSet()         (thermostat_flags |= (1U << 28)) 
#define THERMOSTAT_AmbientNtcErrorShutdownReset()       (thermostat_flags &= (~ (1U << 28)))
#define IsTHERMOSTAT_AmbientNtcErrorShutdownActiv()     ((thermostat_flags & (1U << 28)) != 0U)

#define TemperatureRegulatorOn()					    (temperature_setpoint |= (1U << 7))
#define TemperatureRegulatorOff()					    (temperature_setpoint &= (~ (1U << 7)))
#define IsTemperatureRegulatorOn()					    ((temperature_setpoint & (1U << 7)) != 0U)
#define TemperatureRegulatorSetHeating()                (temperature_setpoint |= (1U << 6))
#define IsTemperatureRegulatorHeating()				    ((temperature_setpoint & (1U << 6)) != 0U)
#define IsTemperatureRegulatorCooling()				    ((temperature_setpoint & (1U << 6)) == 0U)
#define TemperatureRegulatorSetCooling()                (temperature_setpoint &= (~ (1U << 6)))

#define IsTHERMOSTAT_OperationModeOff()                 (thermostat_operation_mode == 0U)
#define IsTHERMOSTAT_OperationModeCooling()             (thermostat_operation_mode == 1U)
#define IsTHERMOSTAT_OperationModeHeating()             (thermostat_operation_mode == 2U)
#define IsTHERMOSTAT_OperationModeRemote()              (thermostat_operation_mode == 3U)

#define IsFANCOIL_FanMotorOnOffControl()                (fan_control_type == 0U)
#define IsFANCOIL_FanMotor3SpeedControl()               (fan_control_type == 1U)
#define IsFANCOIL_FanMotorTriacControl()                (fan_control_type == 2U)
#define IsFANCOIL_FanMotorBldcControl()                 (fan_control_type == 3U)

#define IsFANCOIL_WaterLimitTemperatureValue()          (fancoil_temperature_limit_mode == 0U)
#define IsFANCOIL_WaterLimitSetpointDifference()        (fancoil_temperature_limit_mode == 1U)
/** ============================================================================*/
/**   F A N C O I L   C O N T O L   W I T H   4   D I G I T A L   O U T P U T   */
/** ============================================================================*/
#define FANCOIL_FanLowSpeedOn()				(HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET))
#define FANCOIL_FanLowSpeedOff()			(HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET))
#define IsFANCOIL_FanLowSpeedOn()           (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_10) == GPIO_PIN_SET)
#define FANCOIL_FanMiddleSpeedOn()			(HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET))
#define FANCOIL_FanMiddleSpeedOff()			(HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET))
#define IsFANCOIL_FanMiddleSpeedOn()        (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11) == GPIO_PIN_SET)
#define FANCOIL_FanHighSpeedOn()			(HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET))
#define FANCOIL_FanHighSpeedOff()			(HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET))	
#define IsFANCOIL_FanHighSpeedOn()          (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_12) == GPIO_PIN_SET)
#define FANCOIL_FanOff()					(HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_RESET))
#define FANCOIL_CoolingValveOn()            (HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET))
#define FANCOIL_CoolingValveOff()           (HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET))
#define IsFANCOIL_CoolingValveOn()          (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2) == GPIO_PIN_SET)
/** ============================================================================*/
/**   F A N C O I L   C O N T O L   W I T H   T R I A C   A N D		D O U T     */
/** ============================================================================*/
#define FANCOIL_SetFanOff()					(FAN_SetSpeed(FAN_OFF))
#define FANCOIL_SetFanLowSpeed()			(FAN_SetSpeed(FAN_CONST_SPEED_LOW))
#define FANCOIL_SetFanMiddleSpeed()			(FAN_SetSpeed(FAN_CONST_SPEED_MIDDLE))
#define FANCOIL_SetFanHighSpeed()			(FAN_SetSpeed(FAN_CONST_SPEED_HIGH))


/* Exported Function  ------------------------------------------------------- */
void THERMOSTAT_Init(void);
void THERMOSTAT_Service(void);


#endif
/******************************   END OF FILE  ********************************/
