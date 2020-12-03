/**
 ******************************************************************************
 * File Name          : signaling.c
 * Date               : 28/02/2016 23:16:19
 * Description        : audio visual signaling software modul
 ******************************************************************************
 *
 *
 ******************************************************************************
 */
 
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "eeprom.h"
#include "common.h"
#include "dio_interface.h"
#include "logger.h"
#include "rc522.h"
#include "one_wire.h"
#include "rs485.h"
#include "display.h"
#include "signal.h"


/* Defines    ----------------------------------------------------------------*/
/* Types  --------------------------------------------------------------------*/
SIG_BUZZER_TypeDef	SignalBuzzer = BUZZ_OFF;
ROOM_StatusTypeDef ROOM_Status = ROOM_READY;
ROOM_StatusTypeDef ROOM_OldStatus = ROOM_IDLE;


/* Macros     ----------------------------------------------------------------*/
/* Variables  ----------------------------------------------------------------*/
__IO uint32_t signal_flags;
uint8_t aRoomPowerExpiryDateTime[6];


/* Private prototypes    -----------------------------------------------------*/
static void SaveRoomStatus(void);


/* Program code   ------------------------------------------------------------*/
void SIGNAL_Service(void)
{
    RTC_TimeTypeDef time_sig;
    RTC_DateTypeDef date_sig;
	static uint32_t signal_pcnt = 0U;
	static uint32_t temp_minute = 0U;
    static uint32_t signal_timer = 0U;
    static uint32_t signal_time = 0U;
    static uint32_t room_status_timer = 0U;
    static uint32_t room_status_time = 0U;
	uint8_t br_tmp_cnt, br_tmp_period;
	uint32_t room_time, real_time;
    uint8_t buff[8];
	
	if(eComState == COM_PACKET_RECEIVED) RS485_Service();
	/****************************************************/
	/*	R O O M   S T A T U S   P R O C E S S I N G		*/
	/****************************************************/
    HAL_RTC_GetTime(&hrtc, &time_sig, RTC_FORMAT_BCD);	
    
	if(temp_minute != time_sig.Minutes)
	{
		temp_minute = time_sig.Minutes;
		HAL_RTC_GetDate(&hrtc, &date_sig, RTC_FORMAT_BCD);
		room_time = ((aRoomPowerExpiryDateTime[2] << 16U) +  (aRoomPowerExpiryDateTime[1] << 8U) + aRoomPowerExpiryDateTime[0]);
		real_time = ((date_sig.Year << 16U) + (date_sig.Month << 8U) + date_sig.Date);

		if((room_time > real_time)  \
			&& (time_sig.Hours == ROOM_CLEANING_TIME )  \
			&& (time_sig.Minutes == 0U) \
			&& IsSIGNAL_ExpiryTimeFromGuestCardSet() \
			&& (ROOM_Status == ROOM_BUSY))

		{	
			ROOM_Status = ROOM_CLEANING;
			
			buff[0] = EE_BEDDING_REPL_PERIOD_ADDRESS >> 8U;
			buff[1] = EE_BEDDING_REPL_PERIOD_ADDRESS & 0xFFU;
			if (HAL_I2C_Master_Transmit(&hi2c1, I2C_EE_WRITE, buff, 2U, I2C_EE_TIMEOUT) != HAL_OK) Error_Handler(SIGNAL_FUNC, I2C_DRIVER);
			if (HAL_I2C_Master_Receive(&hi2c1, I2C_EE_READ, &br_tmp_period, 1U, I2C_EE_TIMEOUT) != HAL_OK) Error_Handler(SIGNAL_FUNC, I2C_DRIVER);
			
			if(br_tmp_period != 0U)
			{
				buff[0] = EE_BEDDING_REPL_COUNTER_ADDRESS >> 8U;
				buff[1] = EE_BEDDING_REPL_COUNTER_ADDRESS & 0xFFU;
				if (HAL_I2C_Master_Transmit(&hi2c1, I2C_EE_WRITE, buff, 2U, I2C_EE_TIMEOUT) != HAL_OK) Error_Handler(SIGNAL_FUNC, I2C_DRIVER);
				if (HAL_I2C_Master_Receive(&hi2c1, I2C_EE_READ, &br_tmp_cnt, 1U, I2C_EE_TIMEOUT) != HAL_OK) Error_Handler(SIGNAL_FUNC, I2C_DRIVER);
				br_tmp_cnt++;
				
				if(br_tmp_cnt >= br_tmp_period)
				{
					ROOM_Status = ROOM_BEDDING_REPLACEMENT;
					buff[2] = 0U;
				}
				else
				{
					buff[2] = br_tmp_cnt;
				}
			}
		}
		else if((room_time == real_time) 	\
				&& (time_sig.Hours == ROOM_CLEANING_TIME) \
				&& (time_sig.Minutes == 0U) \
				&& IsSIGNAL_ExpiryTimeFromGuestCardSet() \
			 	&& ((ROOM_Status == ROOM_BUSY) \
					|| (ROOM_Status == ROOM_CLEANING) \
					|| (ROOM_Status == ROOM_BEDDING_REPLACEMENT)))
		{
			buff[0] = EE_BEDDING_REPL_COUNTER_ADDRESS >> 8U;
			buff[1] = EE_BEDDING_REPL_COUNTER_ADDRESS & 0xFFU;
			buff[2] = 0U;
			if (HAL_I2C_Master_Transmit(&hi2c1, I2C_EE_WRITE, buff, 3U, I2C_EE_TIMEOUT) != HAL_OK) Error_Handler(SIGNAL_FUNC, I2C_DRIVER);
			if (HAL_I2C_IsDeviceReady(&hi2c1, I2C_EE_READ, I2C_EE_TRIALS, I2C_EE_WRITE_DELAY) != HAL_OK) Error_Handler(SIGNAL_FUNC, I2C_DRIVER);
			ROOM_Status = ROOM_GENERAL_CLEANING;
			SIGNAL_ExpiryTimeFromGuestCardReset();
		}		
	}
	
	/****************************************************/
	/*		R O O M		S T A T U S		S I G N A L		*/
	/****************************************************/
	if(ROOM_OldStatus != ROOM_Status)
	{
        if((HAL_GetTick() - room_status_timer) >= room_status_time)
        {
            switch (ROOM_Status)
            {
                case ROOM_IDLE:
                {
                    buff[0] = EE_BEDDING_REPL_COUNTER_ADDRESS >> 8U;
                    buff[1] = EE_BEDDING_REPL_COUNTER_ADDRESS & 0xFFU;
                    buff[2] = 0U;
                    if (HAL_I2C_Master_Transmit(&hi2c1, I2C_EE_WRITE, buff, 3U, I2C_EE_TIMEOUT) != HAL_OK) Error_Handler(SIGNAL_FUNC, I2C_DRIVER);
                    if (HAL_I2C_IsDeviceReady(&hi2c1, I2C_EE_READ, I2C_EE_TRIALS, I2C_EE_WRITE_DELAY) != HAL_OK) Error_Handler(SIGNAL_FUNC, I2C_DRIVER);
                    buff[0] = EE_ROOM_POWER_TIMEOUT >> 8U;
                    buff[1] = EE_ROOM_POWER_TIMEOUT & 0xFFU;
                    buff[2] = 0U;
                    buff[3] = 0U;
                    buff[4] = 0U;
                    buff[5] = 0U;
                    buff[6] = 0U;
                    if (HAL_I2C_Master_Transmit(&hi2c1, I2C_EE_WRITE, buff, 7U, I2C_EE_TIMEOUT) != HAL_OK) Error_Handler(SIGNAL_FUNC, I2C_DRIVER);
                    if (HAL_I2C_IsDeviceReady(&hi2c1, I2C_EE_READ, I2C_EE_TRIALS, I2C_EE_WRITE_DELAY) != HAL_OK) Error_Handler(SIGNAL_FUNC, I2C_DRIVER);
                    DISPLAY_CleanUpImageDelete();
                    DISPLAY_SosAlarmImageDelete();
                    DISPLAY_DoNotDisturbImageDelete();				
                    DISPLAY_GeneralCleanUpImageDelete();
                    DISPLAY_BeddingReplacementImageDelete();
                    DISPLAY_MinibarUsedImageDelete();
                    DISPLAY_FireExitImageDelete();
                    DISPLAY_FireAlarmImageDelete();
                    SIGNAL_ExpiryTimeFromGuestCardReset();	
                    DoNotDisturbModul_Off();
                    PowerContactor_Off();
                    HVAC_Contactor_Off();
                    HVAC_Thermostat_Off();
                    SaveRoomStatus();
                    break;
                }	

                case ROOM_READY:
                {
                    DISPLAY_CleanUpImageDelete();
                    DISPLAY_SosAlarmImageDelete();
                    DISPLAY_DoNotDisturbImageDelete();				
                    DISPLAY_GeneralCleanUpImageDelete();
                    DISPLAY_BeddingReplacementImageDelete();
                    DISPLAY_MinibarUsedImageDelete();
                    DISPLAY_FireExitImageDelete();
                    DISPLAY_FireAlarmImageDelete();
                    DISPLAY_RoomNumberImage();
                    DoNotDisturbModul_On();
                    SaveRoomStatus();
                    break;
                }
                
                
                case ROOM_BUSY:
                {
                    DISPLAY_CleanUpImageDelete();
                    DISPLAY_SosAlarmImageDelete();
                    DISPLAY_GeneralCleanUpImageDelete();
                    DISPLAY_BeddingReplacementImageDelete();
                    DISPLAY_MinibarUsedImageDelete();
                    DISPLAY_FireExitImageDelete();
                    DISPLAY_FireAlarmImageDelete();
                    DISPLAY_RoomNumberImage();
                    DoNotDisturbModul_On();
                    SaveRoomStatus();
                    break;
                }
                
                
                case ROOM_CLEANING:
                {
                    DISPLAY_BeddingReplacementImageDelete();
                    DISPLAY_GeneralCleanUpImageDelete();
                    DISPLAY_CleanUpImage();
                    DoNotDisturbModul_On();
                    SaveRoomStatus();
                    break;
                }
                
                
                case ROOM_BEDDING_REPLACEMENT:
                {
                    DISPLAY_GeneralCleanUpImageDelete();
                    DISPLAY_BeddingReplacementImage();
                    DISPLAY_CleanUpImageDelete();
                    DoNotDisturbModul_On();
                    SaveRoomStatus();
                    break;
                }
                
                
                case ROOM_GENERAL_CLEANING:
                {
                    DISPLAY_BeddingReplacementImageDelete();
                    DISPLAY_GeneralCleanUpImage();
                    DISPLAY_CleanUpImageDelete();
                    DoNotDisturbModul_On();
                    SaveRoomStatus();
                    break;
                }
                
                
                case ROOM_OUT_OF_ORDER:
                {
                    DISPLAY_BeddingReplacementImageDelete();
                    DISPLAY_GeneralCleanUpImageDelete();
                    DISPLAY_MinibarUsedImageDelete();
                    DISPLAY_RoomOutOfServiceImage();
                    DISPLAY_CleanUpImageDelete();
                    DoNotDisturbModul_Off();
                    SaveRoomStatus();				
                    break;
                }
                
                
                case ROOM_LATE_CHECKOUT:
                {
                    DoNotDisturbModul_On();
                    DISPLAY_CleanUpImageDelete();
                    DISPLAY_GeneralCleanUpImageDelete();
                    DISPLAY_BeddingReplacementImageDelete();
                    SaveRoomStatus();
                    break;
                }
                
                
                case ROOM_HANDMAID_IN:
                {
                    DoNotDisturbModul_Off();
                    break;
                }
                
                
                case ROOM_FORCING_DND:
                {	
                    break;
                }
                
                
                case ROOM_FIRE_EXIT:
                {
                    DISPLAY_FireExitImage();
                    SignalBuzzer = BUZZ_OFF;
                    SaveRoomStatus();
                    break;
                }
                
                
                case ROOM_FIRE_ALARM:
                {
                    DISPLAY_FireAlarmImage();
                    SignalBuzzer = BUZZ_OFF;
                    SaveRoomStatus();
                    break;
                }
            }
            
            if(ROOM_Status == ROOM_FORCING_DND) 
            {
                if(IsDonNotDisturbActiv())
                {
                    DoNotDisturb_Off();
                    DISPLAY_DoNotDisturbImageDelete();
                    LogEvent.log_event = DO_NOT_DISTURB_SWITCH_OFF;
                    LOGGER_Write();
                    ONEWIRE_ButtonUpdateSet();
                }
                
                if (IsHandmaidCallActiv())
                {
                    HandmaidCall_Off();
                    LED_HandmaidGreen_Off();
                    LogEvent.log_event = HANDMAID_SWITCH_OFF;
                    LOGGER_Write();
                    ONEWIRE_ButtonUpdateSet();
                }
                
                DoNotDisturbModul_Off();
                ROOM_Status = ROOM_OldStatus;
                ROOM_OldStatus = ROOM_FORCING_DND;
                room_status_timer = HAL_GetTick();
                room_status_time = ROOM_STATUS_TOGGLE_TIME;
            }
            else
            {
                ROOM_OldStatus = ROOM_Status;
                room_status_timer = 0U;
                room_status_time = 0U;
            }            
        }
	}
	/************************************************/
	/*		B U Z Z E R			S I G N A L			*/
	/************************************************/
    if((HAL_GetTick() - signal_timer) >= signal_time)
    {
        signal_timer = HAL_GetTick();
        
        switch(SignalBuzzer)
        {
            case BUZZ_OFF:
            {
                Buzzer_Off();
                signal_timer = 0U;
                signal_time = 0U;
                signal_pcnt = 0U;
                break;
            } 
            
            
            case BUZZ_CARD_INVALID:     //  3 short buzzer bips
            {       
                if(!IsBuzzerActiv()) Buzzer_On();
                else Buzzer_Off();
                signal_time = 50U;
                ++signal_pcnt;
                if(signal_pcnt >= 5U)  SignalBuzzer = BUZZ_OFF;
                break;
            } 
            
            
            case BUZZ_CARD_VALID:       //  2 short buzzer bips
            {
                Buzzer_On();
                signal_time = 500U;
                SignalBuzzer = BUZZ_OFF;
                break;
            } 
            
            
            case BUZZ_DOOR_BELL:        //  1 short buzzer bips
            {    
                Buzzer_On();
                signal_time = 50U;
                SignalBuzzer = BUZZ_OFF;
                break;
            } 
            
            
            case BUZZ_HANDMAID_FINISH:
            {    
                Buzzer_On();
                signal_time = 700U;
                SignalBuzzer = BUZZ_OFF; 
                break;
            } 
            
            
            case BUZZ_SOS_ALARM:
            {  
                SignalBuzzer = BUZZ_OFF; 
                break;
            } 
            
            
            case BUZZ_FIRE_ALARM:
            { 
                if((ROOM_Status != ROOM_FIRE_ALARM) && (ROOM_Status != ROOM_FIRE_EXIT))
                {
                    SignalBuzzer = BUZZ_OFF;
                }
                else
                {
                    if(!IsBuzzerActiv()) 
                    {
                        Buzzer_On();
                        signal_time = 50U;
                    }
                    else 
                    {
                        Buzzer_Off();
                        signal_time = 600U;
                    }
                }
                break;
            }  

            default:
            {
                SignalBuzzer = BUZZ_OFF; 
                break;
            }
        }        
    }
}


static void SaveRoomStatus(void)
{
    uint8_t buff[3];
    
    buff[0] = EE_ROOM_STATUS_ADDRESS >> 8U;
    buff[1] = EE_ROOM_STATUS_ADDRESS & 0xFFU;
    buff[2] = ROOM_Status;
    if (HAL_I2C_Master_Transmit(&hi2c1, I2C_EE_WRITE, buff, 3U, I2C_EE_TIMEOUT) != HAL_OK) Error_Handler(RC522_FUNC,  I2C_DRIVER);
    if (HAL_I2C_IsDeviceReady(&hi2c1, I2C_EE_READ, I2C_EE_TRIALS, I2C_EE_WRITE_DELAY) != HAL_OK) Error_Handler(RC522_FUNC,  I2C_DRIVER);
}
/******************************   END OF FILE  **********************************/
