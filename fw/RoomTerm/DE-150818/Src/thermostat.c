/**
 ******************************************************************************
 * File Name          : thermostat.c
 * Date               : 22/02/2018 06:59:00
 * Description        : temperature control processing modul 
 ******************************************************************************
 *
 *
 ******************************************************************************
 */
 
 
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "common.h"
#include "display.h"
#include "one_wire.h"
#include "thermostat.h"
#include "stm32746g.h"
#include "stm32746g_ts.h"
#include "stm32746g_qspi.h"
#include "stm32746g_sdram.h"
#include "stm32746g_eeprom.h"


/* Imported Type  ------------------------------------------------------------*/
/* Imported Variable  --------------------------------------------------------*/
/* Imported Function  --------------------------------------------------------*/
/* Private Type --------------------------------------------------------------*/
/* Private Define ------------------------------------------------------------*/
/* Private Variable ----------------------------------------------------------*/
__IO uint8_t fan_speed;
__IO uint32_t thermostat_flags;
__IO uint16_t room_temperature;
__IO uint16_t fancoil_temperature;
__IO uint16_t ambient_ntc_temperature;
__IO uint16_t fancoil_ntc_temperature;
__IO int16_t thermostat_ambient_ntc_offset;
__IO int16_t thermostat_fancoil_ntc_offset;
__IO uint16_t thermostat_min_heating_temperature;
__IO uint16_t thermostat_max_cooling_temperature;
__IO uint16_t thermostat_min_setpoint_temperature;
__IO uint16_t thermostat_max_setpoint_temperature;
__IO uint16_t thermostat_error_duraton;
__IO uint8_t fancoil_temperature_limit_mode;
__IO uint8_t thermostat_operation_mode;
__IO uint8_t fan_speed_treshold;
__IO uint8_t fan_low_speed_band;
__IO uint8_t fan_middle_speed_band;
__IO uint8_t fan_control_type;
__IO uint8_t freezing_protection_temperature;
__IO uint8_t fan_night_quiet_mode_start_hour;

uint8_t temperature_setpoint;


/* Private Macro -------------------------------------------------------------*/
/* Private Function Prototype ------------------------------------------------*/
/* Program Code  -------------------------------------------------------------*/
void THERMOSTAT_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	  
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2|GPIO_PIN_4, GPIO_PIN_RESET);
	
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    thermostat_flags = thermostat_flags_memo;
}


void THERMOSTAT_Service(void)
{
    uint8_t ee_buff[4];
    static uint32_t temp_sp = 0U;
    static uint32_t fan_pcnt = 0U;
    static uint32_t old_fan_speed = 0U;
    static uint32_t fan_calc_speed = 0U;
    static uint32_t thermostat_time = 0U;
    static uint32_t thermostat_timer = 0U;
    static uint32_t fancoil_ntc_timer = 0U;
    static uint32_t fancoil_fan_timer = 0U;
    static uint32_t fancoil_valve_timer = 0U;
    static uint32_t fancoil_fluid_error_time = 0U;
    static uint32_t fancoil_fluid_error_timer = 0U;
    static uint32_t bldc_fan_speed_control_timer = 0U;
    /** ============================================================================*/
	/**		C H E C K       T I M E R    A N D      S Y S T E M     F L A G S	    */
	/** ============================================================================*/
    if((HAL_GetTick() - thermostat_timer) < thermostat_time) return;
    /** ============================================================================*/
	/**		    R U N       O N C E     R P M       S E N S O R     C H E C K	    */
	/** ============================================================================*/
    if(IsSYSTEM_StartupActiv())
    {
        if(IsFANCOIL_FanMotorBldcControl())
        {
            return;
        }
        
        if(fan_pcnt == 0U)
        {
            if(IsFANCOIL_FanMotorTriacControl()) FANCOIL_SetFanLowSpeed();
            thermostat_timer = HAL_GetTick();
            thermostat_time = FANCOIL_RPM_MEASURE_TIME;
            ++fan_pcnt;
        }
        else if(fan_pcnt == 1U)
        {
            thermostat_timer = HAL_GetTick();
            thermostat_time = FANCOIL_RPM_MEASURE_TIME;
            ++fan_pcnt;
        }
        else if(fan_pcnt == 2U)
        {
            /***********************************************************************************************************
            *   run fancoil fan motor shortly with triac driver to sense hall rpm sensor if available, 
            *   this will define functions used to controll hardware interface and type of temperature controller
            *   rpm sensor will be also used for cleaning request when motor start to slow down due to dust collect
            *   if sensor not available,than tree speed motor is used for fancoil fan, and controller is simpler without
            *   precise timings for triac triggering releasing from runtime code two interrupt request services
            ************************************************************************************************************/ 
            if (fan_rpm_pulse > 10U) FANCOIL_RPM_SensorConnected();
            else FANCOIL_RPM_SensorNotConnected();
            thermostat_time = THERMOSTAT_POWER_ON_DELAY_TIME;
            thermostat_timer = HAL_GetTick();
            if (IsFANCOIL_FanMotorTriacControl()) FANCOIL_SetFanOff();
            ++fan_pcnt;
        }
        
        return;
    }    
    /** ============================================================================*/
	/**		U P D A T E 	R O O M     A M B I E N T     T E M P E R A T U R E     */
	/** ============================================================================*/
    if(IsONEWIRE_SensorConnected())         
    {
        if(room_temperature != ds18b20_1.temperature)
        {
            room_temperature = ds18b20_1.temperature;
            ONEWIRE_SendThermostatVariableSet();
        }
    }
	else if(IsAMBIENT_NTC_SensorConnected())
    {
        if(room_temperature != ambient_ntc_temperature + thermostat_ambient_ntc_offset)
        {
            room_temperature = ambient_ntc_temperature + thermostat_ambient_ntc_offset;
            ONEWIRE_SendThermostatVariableSet();
            DISPLAY_AmbientTemperatureUpdateSet();
        }
    }
    else
    {
        room_temperature = 0U;
        return;
    }
    /** ============================================================================*/
    /**     U P D A T E     F A N C O I L     F L U I D     T E M P E R A T U R E   */
    /** ============================================================================*/
    if(IsFANCOIL_NTC_SensorConnected())
    {
        if((HAL_GetTick() - fancoil_ntc_timer) >= FANCOIL_NTC_UPDATE_TIME) 
        {
            fancoil_ntc_timer = HAL_GetTick();
            fancoil_fluid_error_time = (thermostat_error_duraton * 60000U);
            
            if(fancoil_temperature != fancoil_ntc_temperature + thermostat_fancoil_ntc_offset)
            {
                fancoil_temperature = fancoil_ntc_temperature + thermostat_fancoil_ntc_offset;
                ONEWIRE_SendThermostatVariableSet();
                DISPLAY_FancoilTemperatureUpdateSet();
            }
            /** ============================================================================*/
            /**     F A N C O I L       F R E E Z I N G         P R O T E C T I O N         */
            /** ============================================================================*/
            if(IsFANCOIL_FreezingProtectionActiv() && !IsTHERMOSTAT_OperationModeOff())
            {
                if(!IsTHERMOSTAT_OnecycleHeatingActiv())
                {
                    if((fancoil_temperature < freezing_protection_temperature))
                    {
                        if(IsFANCOIL_FreezingProtectionReportActiv()) 
                        {
                            FANCOIL_FreezingAlarmSet();
                        }
                        
                        TemperatureRegulatorOff();
                        THERMOSTAT_OnecycleHeatingSet();
                        
                        if      (IsFANCOIL_FanMotor3SpeedControl()) FANCOIL_FanHighSpeedOn();
                        else if (IsFANCOIL_FanMotorTriacControl())  FANCOIL_SetFanHighSpeed();
                        else if (IsFANCOIL_FanMotorBldcControl())   __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, fan_middle_speed_band);
                        
                        FANCOIL_CoolingValveOn();
                        fancoil_fan_timer = HAL_GetTick();
                        fancoil_valve_timer = HAL_GetTick();
                        return;
                    }                       
                }
                else if(IsTHERMOSTAT_OnecycleHeatingActiv())
                {
                    if((fancoil_temperature >= FANCOIL_FREEZING_PROTECTION_SETPOINT))
                    {
                        if(IsTHERMOSTAT_ControllerActiv())
                        {
                            TemperatureRegulatorOn();
                        }
                        
                        THERMOSTAT_OnecycleHeatingReset();
                        old_fan_speed = 1U;
                        fan_speed = 0U;
                    }
                    
                    fancoil_fluid_error_timer = HAL_GetTick();
                    return;
                }
            }
            /** ============================================================================*/
            /**         C H E C K    F L U I D     T E M P E R A T U R E     L I M I T S    */
            /** ============================================================================*/
            if(IsTemperatureRegulatorOn())
            {
                if(IsFANCOIL_WaterLimitTemperatureValue())
                {
                    if(IsTemperatureRegulatorHeating())
                    {
                        if(fancoil_temperature < thermostat_min_heating_temperature)
                        {
                            if((HAL_GetTick() - fancoil_fluid_error_timer) >= fancoil_fluid_error_time)
                            { 
                                if(IsFANCOIL_HeatingErrorReportActiv()) 
                                {
                                    FANCOIL_HeatingTempErrorSet();
                                }
                                
                                if(IsFANCOIL_HeatingErrorShutdownActiv()) 
                                {
                                    TemperatureRegulatorOff();
                                } 
                                
                                fancoil_fluid_error_timer = HAL_GetTick();
                            }
                        }
                        else
                        {
                            fancoil_fluid_error_timer = HAL_GetTick();
                        }
                    }
                    else if(IsTemperatureRegulatorCooling()) 
                    {
                        if(fancoil_temperature > thermostat_max_cooling_temperature)
                        {
                            if((HAL_GetTick() - fancoil_fluid_error_timer) >= fancoil_fluid_error_time)
                            {
                                fancoil_fluid_error_timer = HAL_GetTick();
                                
                                if(IsFANCOIL_CoolingErrorReportActiv()) 
                                {
                                    FANCOIL_CoolingTempErrorSet();
                                }
                                
                                if(IsFANCOIL_CoolingErrorShutdownActiv()) 
                                {
                                    TemperatureRegulatorOff();
                                }
                            }
                        }
                        else
                        {
                            fancoil_fluid_error_timer = HAL_GetTick();
                        }
                    }                
                }
                else if(IsFANCOIL_WaterLimitSetpointDifference())
                {
                    if(IsTemperatureRegulatorHeating())
                    {
                        if(fancoil_temperature < (((temperature_setpoint & 0x3fU) * 10U)  - thermostat_min_heating_temperature))
                        {
                            if((HAL_GetTick() - fancoil_fluid_error_timer) >= fancoil_fluid_error_time)
                            { 
                                if(IsFANCOIL_HeatingErrorReportActiv()) 
                                {
                                    FANCOIL_HeatingTempErrorSet();
                                }
                                
                                if(IsFANCOIL_HeatingErrorShutdownActiv()) 
                                {
                                    TemperatureRegulatorOff();
                                } 
                                
                                fancoil_fluid_error_timer = HAL_GetTick();
                            }
                        }
                        else
                        {
                            fancoil_fluid_error_timer = HAL_GetTick();
                        }                        
                    }
                    else if(IsTemperatureRegulatorCooling()) 
                    {
                        if(fancoil_temperature > (((temperature_setpoint & 0x3fU) * 10U)  + thermostat_max_cooling_temperature))
                        {
                            if((HAL_GetTick() - fancoil_fluid_error_timer) >= fancoil_fluid_error_time)
                            {
                                if(IsFANCOIL_CoolingErrorReportActiv()) 
                                {
                                    FANCOIL_CoolingTempErrorSet();
                                }
                                
                                if(IsFANCOIL_CoolingErrorShutdownActiv()) 
                                {
                                    TemperatureRegulatorOff();
                                }
                                
                                fancoil_fluid_error_timer = HAL_GetTick();
                            }
                        }
                        else
                        {
                            fancoil_fluid_error_timer = HAL_GetTick();
                        }
                    }    
                }               
            }
            /** ============================================================================*/
            /**             T H E R M O S T A T         A U T O R E S T A R T               */
            /** ============================================================================*/
            else if(IsFANCOIL_ErrorAutorestartActiv())
            {
                if((HAL_GetTick() - fancoil_fluid_error_timer) >= fancoil_fluid_error_time)
                {
                    if(IsTHERMOSTAT_ControllerActiv())
                    {
                        if(IsFANCOIL_HeatingErrorShutdownActiv()
                           || IsFANCOIL_CoolingErrorShutdownActiv()
                           || IsFANCOIL_FancoilNtcErrorShutdownActiv()
                           || IsTHERMOSTAT_AmbientNtcErrorShutdownActiv())
                        {
                           TemperatureRegulatorOn(); 
                        }
                    }
                    
                    fancoil_fluid_error_timer = HAL_GetTick();
                }
            }
        }
    }
    else 
    {
        fancoil_temperature = 0U;
    }
    /** ============================================================================*/
	/**		    C H E C K       O P E R A T I N G       C O N D I T I O N S   	    */
	/** ============================================================================*/
    if(IsTHERMOSTAT_OperationModeOff())
    {
        if(IsTemperatureRegulatorOn())
        {
            TemperatureRegulatorOff();
            BSP_EEPROM_WriteBuffer(&temperature_setpoint, EE_THERMOSTAT_SET_POINT, 1U);
            BSP_EEPROM_WaitEepromStandbyState();
            THERMOSTAT_ControllerOff();
            ee_buff[0] = (thermostat_flags >> 24U);
            ee_buff[1] = (thermostat_flags >> 16U);
            ee_buff[2] = (thermostat_flags >> 8U);
            ee_buff[3] = (thermostat_flags & 0xFFU);
            BSP_EEPROM_WriteBuffer(ee_buff, EE_THERMOSTAT_FLAGS, 4U);
            BSP_EEPROM_WaitEepromStandbyState();
        }
    }
    else if(IsTHERMOSTAT_OperationModeCooling())
    {
        if(!IsTemperatureRegulatorOn() || !IsTemperatureRegulatorCooling())
        {
            TemperatureRegulatorOn();
            TemperatureRegulatorSetCooling();
            BSP_EEPROM_WriteBuffer(&temperature_setpoint, EE_THERMOSTAT_SET_POINT, 1U);
            BSP_EEPROM_WaitEepromStandbyState();
            THERMOSTAT_ControllerOff();
            ee_buff[0] = (thermostat_flags >> 24U);
            ee_buff[1] = (thermostat_flags >> 16U);
            ee_buff[2] = (thermostat_flags >> 8U);
            ee_buff[3] = (thermostat_flags & 0xFFU);
            BSP_EEPROM_WriteBuffer(ee_buff, EE_THERMOSTAT_FLAGS, 4U);
            BSP_EEPROM_WaitEepromStandbyState();
            DISPLAY_UserInterfaceSet();
            DISPLAY_UpdateSet();
        }
    }
    else if(IsTHERMOSTAT_OperationModeHeating())
    {
        if(!IsTemperatureRegulatorOn() || !IsTemperatureRegulatorHeating())
        {
            TemperatureRegulatorOn();
            TemperatureRegulatorSetHeating();
            BSP_EEPROM_WriteBuffer(&temperature_setpoint, EE_THERMOSTAT_SET_POINT, 1U);
            BSP_EEPROM_WaitEepromStandbyState();
            THERMOSTAT_ControllerOff();
            ee_buff[0] = (thermostat_flags >> 24U);
            ee_buff[1] = (thermostat_flags >> 16U);
            ee_buff[2] = (thermostat_flags >> 8U);
            ee_buff[3] = (thermostat_flags & 0xFFU);
            BSP_EEPROM_WriteBuffer(ee_buff, EE_THERMOSTAT_FLAGS, 4U);
            BSP_EEPROM_WaitEepromStandbyState();
            DISPLAY_UserInterfaceSet();
            DISPLAY_UpdateSet();
        }
    }
    else if(IsTHERMOSTAT_OperationModeRemote())
    {
        if(!IsTHERMOSTAT_WindowSwitchClosed() && !IsTHERMOSTAT_WindowSwitchActiv()) 
        {
            TemperatureRegulatorOff();
            THERMOSTAT_WindowSwitchActivSet();
            ONEWIRE_UpdateDisplayImageSet();
            DISPLAY_ConditionErrorSet();
            DISPLAY_UpdateSet();
        }
        else if((!IsTHERMOSTAT_WindowSwitchClosed() && IsTHERMOSTAT_WindowSwitchActiv()) ||
                 (IsTHERMOSTAT_WindowSwitchClosed() && !IsTHERMOSTAT_WindowSwitchActiv()))
        {
            if(IsONEWIRE_ForceThermostatOnActiv())
            {
                ONEWIRE_ForceThermostatOnReset();
                ONEWIRE_UpdateDisplayImageSet();
                DISPLAY_UserInterfaceSet();
                DISPLAY_UpdateSet();
            }
            else if(IsONEWIRE_ForceThermostatOffActiv())
            {
                ONEWIRE_ForceThermostatOffReset();
                ONEWIRE_UpdateDisplayImageSet();
                DISPLAY_ConditionErrorSet();
                DISPLAY_UpdateSet();
            }
            else if(IsONEWIRE_HVAC_ContactorOn() && !IsONEWIRE_HVAC_ContactorActiv())
            {
                if(IsTHERMOSTAT_ControllerActiv())
                {
                    TemperatureRegulatorOn();
                }
                
                ONEWIRE_HVAC_ContactorActivSet();
                ONEWIRE_UpdateDisplayImageSet();
                DISPLAY_UserInterfaceSet();
                DISPLAY_UpdateSet();
            }
            else if(!IsONEWIRE_HVAC_ContactorOn() && IsONEWIRE_HVAC_ContactorActiv())
            {
                TemperatureRegulatorOff();
                ONEWIRE_HVAC_ContactorActivReset();
                ONEWIRE_UpdateDisplayImageSet();
                DISPLAY_ConditionErrorSet();
                DISPLAY_UpdateSet();
            }
        }
        else if(IsTHERMOSTAT_WindowSwitchClosed() && IsTHERMOSTAT_WindowSwitchActiv()) 
        {
            if(IsTHERMOSTAT_ControllerActiv())
            {
                TemperatureRegulatorOn();
            }
            
            THERMOSTAT_WindowSwitchActivReset();
            ONEWIRE_UpdateDisplayImageSet();
            DISPLAY_UserInterfaceSet();
            DISPLAY_UpdateSet();
        }        
    }
    /** ============================================================================*/
	/**				T E M P E R A T U R E		C O N T R O L L E R					*/
	/** ============================================================================*/
	if(!IsTemperatureRegulatorOn())
	{
        fan_pcnt = 0U;
		fan_speed = 0U;
        old_fan_speed = 0U;
        fan_calc_speed = 0U;
        
		if      (IsFANCOIL_FanMotor3SpeedControl()) FANCOIL_FanOff();
		else if (IsFANCOIL_FanMotorTriacControl())  FANCOIL_SetFanOff();
        else if (IsFANCOIL_FanMotorBldcControl())   __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, fan_speed);
        
        FANCOIL_CoolingValveOff();
        FANCOIL_CoolingValveReset();
        FANCOIL_HeatingTempErrorReset();
        FANCOIL_CoolingTempErrorReset();
        FANCOIL_FreezingAlarmReset();
        FANCOIL_RPM_SensorErrorReset();
        FANCOIL_NTC_SensorErrorReset();
        AMBIENT_NTC_SensorErrorReset();
	}
	else if(IsTemperatureRegulatorOn())  
	{
        temp_sp = ((temperature_setpoint & 0x3FU) * 10U);
        
        if(IsTemperatureRegulatorCooling())
        {
            if(IsFANCOIL_FanMotorBldcControl())
            {
                if((HAL_GetTick() - bldc_fan_speed_control_timer) >= FAN_BLDC_MOTOR_CONTROL_LOOP_TIME)
                {
                    bldc_fan_speed_control_timer = HAL_GetTick();

                    if ((fan_speed == 0U) && (room_temperature > (temp_sp + FAN_BLDC_MOTOR_CONTROL_TRESHOLD))) fan_speed = 1U;
                    else if (room_temperature <= temp_sp) fan_speed = 0U;
                    else if (fan_speed != 0U)
                    {
                        if(room_temperature > (temp_sp + fan_speed_treshold)) fan_calc_speed = fan_middle_speed_band;                        
                        else fan_calc_speed = (((room_temperature - temp_sp) * 100U) / fan_speed_treshold);
                        
                        if (fan_speed < fan_calc_speed) ++fan_speed;
                        else if (fan_speed > fan_calc_speed) --fan_speed;
                        
                        if(fan_speed > fan_middle_speed_band) fan_speed = fan_middle_speed_band;
                        else if (fan_speed < fan_low_speed_band) fan_speed = fan_low_speed_band;
                    }
                }
            }
            else
            {
                if      ((fan_speed == 0U) && (room_temperature > (temp_sp + fan_low_speed_band)))                                               fan_speed = 1U;
                else if ((fan_speed == 1U) && (room_temperature > (temp_sp + fan_middle_speed_band)))                                            fan_speed = 2U;
                else if ((fan_speed == 1U) && (room_temperature <= temp_sp))                                                                     fan_speed = 0U;
                else if ((fan_speed == 1U) && (room_temperature <=(temp_sp + fan_speed_treshold)))                             
                {
                    if(!IsFANCOIL_ValveFollowFanActiv())
                    {
                        FANCOIL_CoolingValveReset();
                    }
                }
                else if ((fan_speed == 2U) && (room_temperature > (temp_sp + fan_middle_speed_band + fan_low_speed_band)))                       fan_speed = 3U;
                else if ((fan_speed == 2U) && (room_temperature <=(temp_sp + fan_middle_speed_band - fan_speed_treshold)))                       fan_speed = 1U; 
                else if ((fan_speed == 3U) && (room_temperature <=(temp_sp + fan_middle_speed_band + fan_low_speed_band - fan_speed_treshold)))  fan_speed = 2U;                 
            }
        }
        else if(IsTemperatureRegulatorHeating())
        {
            if(IsFANCOIL_FanMotorBldcControl())
            {
                if((HAL_GetTick() - bldc_fan_speed_control_timer) >= FAN_BLDC_MOTOR_CONTROL_LOOP_TIME)
                {
                    bldc_fan_speed_control_timer = HAL_GetTick();

                    if ((fan_speed == 0U) && (room_temperature < (temp_sp - FAN_BLDC_MOTOR_CONTROL_TRESHOLD))) fan_speed = 1U;
                    else if (room_temperature >= temp_sp) fan_speed = 0U;
                    else if (fan_speed != 0U)
                    {
                        if(room_temperature < (temp_sp - fan_speed_treshold)) fan_calc_speed = fan_middle_speed_band;                        
                        else fan_calc_speed = (((temp_sp - room_temperature)* 100U) / fan_speed_treshold);
                        
                        if (fan_speed < fan_calc_speed) ++fan_speed;
                        else if (fan_speed > fan_calc_speed) --fan_speed;
                        
                        if(fan_speed > fan_middle_speed_band) fan_speed = fan_middle_speed_band;
                        else if (fan_speed < fan_low_speed_band) fan_speed = fan_low_speed_band;
                    }     
                }
            }
            else
            {
                if      ((fan_speed == 0U) && (room_temperature < (temp_sp - fan_low_speed_band)))                                               fan_speed = 1U;
                else if ((fan_speed == 1U) && (room_temperature < (temp_sp - fan_middle_speed_band)))                                            fan_speed = 2U;
                else if ((fan_speed == 1U) && (room_temperature >= temp_sp))                                                                     fan_speed = 0U;
                else if ((fan_speed == 2U) && (room_temperature < (temp_sp - fan_middle_speed_band - fan_low_speed_band)))                       fan_speed = 3U;
                else if ((fan_speed == 2U) && (room_temperature >=(temp_sp - fan_middle_speed_band + fan_speed_treshold)))                       fan_speed = 1U; 
                else if ((fan_speed == 3U) && (room_temperature >=(temp_sp - fan_middle_speed_band - fan_low_speed_band + fan_speed_treshold)))  fan_speed = 2U; 
            }        
        }
    }
    /** ============================================================================*/
	/**		S W I T C H		F A N		S P E E D		W I T H		D E L A Y		*/
	/** ============================================================================*/
	if (fan_speed != old_fan_speed) // don't let fan motor goes to smoke if we like to touch too much 
	{
        if((HAL_GetTick() - fancoil_fan_timer) >= FANCOIL_FAN_MIN_ON_TIME)
        {
            if (IsFANCOIL_FanMotorBldcControl())   __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, fan_speed);
            if(fan_pcnt > 1U)  fan_pcnt = 0U;
                
            if(fan_pcnt == 0U) 
            {
                if      (IsFANCOIL_FanMotor3SpeedControl()) FANCOIL_FanOff();
                else if (IsFANCOIL_FanMotorTriacControl())  FANCOIL_SetFanOff();
                
                if(old_fan_speed != 0U) fancoil_fan_timer = HAL_GetTick();
                ++fan_pcnt;
            }
            else if(fan_pcnt == 1U)
            {
                if(fan_speed == 1U) 
                {
                    if      (IsFANCOIL_FanMotor3SpeedControl()) FANCOIL_FanLowSpeedOn();
                    else if (IsFANCOIL_FanMotorTriacControl())  FANCOIL_SetFanLowSpeed();
                }
                else if(fan_speed == 2U) 
                {
                    if      (IsFANCOIL_FanMotor3SpeedControl()) FANCOIL_FanMiddleSpeedOn();
                    else if (IsFANCOIL_FanMotorTriacControl())  FANCOIL_SetFanMiddleSpeed();
                }
                else if(fan_speed == 3U) 
                {
                    if      (IsFANCOIL_FanMotor3SpeedControl()) FANCOIL_FanHighSpeedOn();
                    else if (IsFANCOIL_FanMotorTriacControl())  FANCOIL_SetFanHighSpeed();
                }
                
                if(fan_speed != 0U) 
                {
                    FANCOIL_CoolingValveSet();
                    fancoil_fan_timer = HAL_GetTick();
                }
                else 
                {
                    if(IsFANCOIL_ValveFollowFanActiv() || IsTemperatureRegulatorCooling())
                    {
                       FANCOIL_CoolingValveReset(); 
                    }
                }
                old_fan_speed = fan_speed;
                ++fan_pcnt;
            }            
        }
	}
    /** ============================================================================*/
	/**		S W I T C H		E L E C T R O V A L V E     W I T H		D E L A Y       */
	/** ============================================================================*/
    if((HAL_GetTick() - fancoil_valve_timer) >= FANCOIL_VALVE_MIN_ON_TIME)
    {
        fancoil_valve_timer = HAL_GetTick();
        
        if(IsFANCOIL_CoolingValveActiv() && !IsFANCOIL_CoolingValveOn())
        {
            FANCOIL_CoolingValveOn();
        }       
        else if(!IsFANCOIL_CoolingValveActiv() && IsFANCOIL_CoolingValveOn())
        {
            FANCOIL_CoolingValveOff();
        } 
    }
}


/******************************   RAZLAZ SIJELA  ********************************/
