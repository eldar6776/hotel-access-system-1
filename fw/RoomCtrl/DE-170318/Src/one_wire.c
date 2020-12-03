/**
 ******************************************************************************
 * File Name          : onewire.c
 * Date               : 17/11/2016 00:59:00
 * Description        : one wire communication modul
 ******************************************************************************
 *
 *
 ******************************************************************************
 */


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "eeprom.h"
#include "common.h"
#include "dio.h"
#include "rc522.h"
#include "one_wire.h"
#include "rs485.h"
#include "display.h"
#include "signal.h"


/* Imported Type  ------------------------------------------------------------*/
/* Imported Variable  --------------------------------------------------------*/
/* Imported Function  --------------------------------------------------------*/
/* Private Type --------------------------------------------------------------*/
TempSensorTypeDef ds18b20_1;


/* Private Define ------------------------------------------------------------*/
/** ============================================================================*/
/**			D S 1 8 B 2 0       S E N S O R         C O N S T A N T E           */
/** ============================================================================*/
#define ONEWIRE_RESET				    1U
#define ONEWIRE_DATA				    2U
#define ONEWIRE_TIMEOUT                 800U
#define ONEWIRE_SEARCH_ROM			    ((uint8_t)0xf0U)
#define ONEWIRE_READ_ROM			    ((uint8_t)0x33U)
#define ONEWIRE_MATCH_ROM			    ((uint8_t)0x55U)
#define ONEWIRE_SKIP_ROM			    ((uint8_t)0xccU)
#define ONEWIRE_ALARM_SEARCH		    ((uint8_t)0xecU)
#define ONEWIRE_CONVERT				    ((uint8_t)0x44U)
#define ONEWIRE_WRITE_SCRATCHPAD	    ((uint8_t)0x4eU)
#define ONEWIRE_READ_SCRATCHPAD		    ((uint8_t)0xbeU)
#define ONEWIRE_COPY_SCRATCHPAD		    ((uint8_t)0x48U)
#define ONEWIRE_RACALL_E2			    ((uint8_t)0xb8U)
#define ONEWIRE_READ_POWER_SUPPLY	    ((uint8_t)0xb4U)

#define ONEWIRE_GET_STATE               ((uint8_t)0xa0U)
#define ONEWIRE_SET_BUTTON_STATE        ((uint8_t)0xa1U)
#define ONEWIRE_SET_DATE_TIME           ((uint8_t)0xa2U)
#define ONEWIRE_SET_THERMOSTAT_SP       ((uint8_t)0xa3U)
#define ONEWIRE_SET_DISPLAY_IMAGE       ((uint8_t)0xa4U)
#define ONEWIRE_CLEAR_ERROR             ((uint8_t)0xa5U)


/* Private Variable ----------------------------------------------------------*/
uint8_t thermostat_id;
uint8_t thermostat_addresse[9];
uint8_t thermostat_setpoint[9];
uint8_t thermostat_display_image_id[9];
uint8_t thermostat_display_image_time[9];
uint8_t thermostat_buzzer_repeat_time[9];
uint8_t thermostat_buzzer_sound_mode[9];
uint8_t thermostat_fancoil_temperature[9];
uint8_t thermostat_ambient_temperature[9];
uint8_t temperature_setpoint;
uint8_t temperature_difference;
uint8_t onewire_device_connected;
uint8_t onewire_thermostat_address;
uint16_t temperature_measured;
uint32_t onewire_flags;


/* Macros     ----------------------------------------------------------------*/
/* Private prototypes    -----------------------------------------------------*/
static uint8_t ONEWIRE_Reset(void);
static uint8_t ONEWIRE_ReceiveBit(void);
static void ONEWIRE_SendByte(uint8_t data);
static void ONEWIRE_SetUsart(uint32_t setup);
static void ONEWIRE_SendBit(uint8_t send_bit);
static uint8_t ONEWIRE_Search(TempSensorTypeDef* ds18b20, uint8_t* sensor_cnt);


/* Program code   ------------------------------------------------------------*/
void ONEWIRE_Init(void)
{
    onewire_device_connected = 0U;
    
    for(uint32_t t = 0U; t < 9U; t++)
    {
        if((thermostat_addresse[t] > 0U) && (thermostat_addresse[t] < 10U))
        {
            ++onewire_device_connected;
        }
    }
    
    if(onewire_device_connected > 0U) ONEWIRE_ThermostatConnected();
    else ONEWIRE_ScanBus();
}


void ONEWIRE_Service(void)
{
    RTC_TimeTypeDef time_ow;
    RTC_DateTypeDef date_ow;
    HAL_StatusTypeDef status;
    static uint32_t ow_init = 0U;
    static uint32_t ow_pcnt = 0U;
    static uint32_t ow_cmd = 0U;
    static uint32_t ow_err = 0U;
    static uint32_t ow_tmr;
    static uint32_t ow_tim;
    uint32_t tmp_tmr;
	uint8_t ow_buff[96];

    if(ow_init == 0U)
    {
        ow_tmr = HAL_GetTick();
        ow_tim = ONEWIRE_POWER_ON_DELAY;
        ++ow_init;
        return;
    }

    if((HAL_GetTick() - ow_tmr) < ow_tim) return;
    else if(IsONEWIRE_ThermostatConnected())    // use onewire bus as usart half duplex
	{
        /** ============================================================================*/
		/**			    S E T           O N E W I R E           C O M M A N D			*/
		/** ============================================================================*/
        if(IsONEWIRE_TimeUpdateActiv() && !IsONEWIRE_UpdateLockActiv())
        {
            ONEWIRE_UpdateLockSet();
            ONEWIRE_TimeUpdateReset();
            ow_cmd = ONEWIRE_SET_DATE_TIME;
            ow_pcnt = 0U;
        }
        else if(IsONEWIRE_ButtonUpdateActiv())
        {
            ONEWIRE_ButtonUpdateReset();
            ow_cmd = ONEWIRE_SET_BUTTON_STATE;
            ow_pcnt = 0U;
        }
        else if(IsONEWIRE_SetpointUpdateActiv())
        {
            ONEWIRE_SetpointUpdateReset();
            ow_cmd = ONEWIRE_SET_THERMOSTAT_SP;
            ow_pcnt = (thermostat_id - 1U);
        }
        else if(IsONEWIRE_DisplayUpdateActiv())
        {
            ONEWIRE_DisplayUpdateReset();
            ow_cmd = ONEWIRE_SET_DISPLAY_IMAGE;
            ow_pcnt = 0U;

            if(IsONEWIRE_UpdateSelectetdActiv())
            {
                ow_pcnt = (thermostat_id - 1U);
            }
        }
        else if(IsONEWIRE_ClearErrorActiv())
        {
            ONEWIRE_ClearErrorReset();
            ow_cmd = ONEWIRE_CLEAR_ERROR;
            ow_pcnt = 0U;
        }
        else if(!IsONEWIRE_UpdateLockActiv() && (ow_cmd != ONEWIRE_GET_STATE))
        {
            ow_cmd = ONEWIRE_GET_STATE;
            ow_pcnt = 0U;
        }
        /** ============================================================================*/
		/**     S E N D     A N D   R E C E I V E   O N E W I R E       P A C K E T	    */
		/** ============================================================================*/
        ClearBuffer(ow_buff, sizeof(ow_buff));
        huart2.Instance->DR = 0x0100U + thermostat_addresse[ow_pcnt];
        tmp_tmr = HAL_GetTick();
        status = HAL_OK;

        while((HAL_UART_GetState(&huart2) != HAL_UART_STATE_READY) && (status == HAL_OK))
        {
            if ((HAL_GetTick() - tmp_tmr) >= ONEWIRE_TRANSFER_TIMEOUT)
            {
                status = HAL_TIMEOUT;
            }
        }

        HAL_Delay(2);
        ow_buff[0] = ow_cmd;
        ow_buff[1] = 0U;

        switch(ow_cmd)
        {
            /** ==========================================================================*/
            /**			S E T		N E W		B U T T O N 		S T A T E			  */
            /** ==========================================================================*/
            case ONEWIRE_SET_BUTTON_STATE:
            {
                if(IsDonNotDisturbActiv())      ow_buff[2] = 1U;
                if(IsSosAlarmActiv())           ow_buff[4] = 1U;
                if(IsHandmaidCallActiv())       ow_buff[6] = 1U;
                if(IsHVAC_ContactorActiv())     ow_buff[8] = 1U;
                if(IsBalconyDoorSensorActiv())  ow_buff[10] = 1U;
                if(IsPowerContactorActiv())     ow_buff[12] = 1U;
                if(IsDoorBellActiv())           ow_buff[14] = 1U;
                break;
            }
            /** ==========================================================================*/
            /**			S E T		N E W		D A T E 	& 		T I M E				  */
            /** ==========================================================================*/
            case ONEWIRE_SET_DATE_TIME:
            {
                HAL_RTC_GetTime(&hrtc, &time_ow, RTC_FORMAT_BCD);
                HAL_RTC_GetDate(&hrtc, &date_ow, RTC_FORMAT_BCD);
                ow_buff[2] = date_ow.Date;
                ow_buff[4] = date_ow.WeekDay;
                ow_buff[6] = date_ow.Month;
                ow_buff[8] = date_ow.Year;
                ow_buff[10] = time_ow.Hours;
                ow_buff[12] = time_ow.Minutes;
                ow_buff[14] = time_ow.Seconds;
                break;
            }
            /** ==========================================================================*/
            /**		    C L E A R       T H E R M O S T A T     E R R O R                 */
            /** ==========================================================================*/
            case ONEWIRE_CLEAR_ERROR:
            {
                break;
            }
            /** ==========================================================================*/
            /**			S E T		T H E R M O S T A T 	P A R A M E T E R S			  */
            /** ==========================================================================*/
            case ONEWIRE_SET_THERMOSTAT_SP:
            {
                ow_buff[2] = thermostat_setpoint[ow_pcnt];
                break;
            }
            /** ==========================================================================*/
            /**		S E T		N E W		D I S P L A Y    M E S S A G E		          */
            /** ==========================================================================*/
            case ONEWIRE_SET_DISPLAY_IMAGE:
            {
                ow_buff[2] = thermostat_display_image_id[ow_pcnt];
                ow_buff[4] = thermostat_display_image_time[ow_pcnt];
                ow_buff[6] = thermostat_buzzer_sound_mode[ow_pcnt];
                ow_buff[8] = thermostat_buzzer_repeat_time[ow_pcnt];
                break;
            }
            /** ==========================================================================*/
            /**			    G E T		T H E R M O S T A T     S T A T E			      */
            /** ==========================================================================*/
            case ONEWIRE_GET_STATE:
            {
                if(IsONEWIRE_UpdateLockActiv()) break;
                ow_buff[16] = CalcCRC(ow_buff, 16U);
                status += HAL_UART_Transmit(&huart2, ow_buff, 9U, ONEWIRE_TRANSFER_TIMEOUT);
                __HAL_UART_FLUSH_DRREGISTER(&huart2);
                status += HAL_UART_Receive(&huart2, ow_buff, 9U, ONEWIRE_TRANSFER_TIMEOUT);
                if(ow_buff[16] != CalcCRC(ow_buff, 16U))
                {
                    status = HAL_ERROR;
                    break;
                }
                /** ==========================================================================*/
                /**			    G E T		N E W		B U T T O N 		S T A T E		  */
                /** ==========================================================================*/
                if((ow_buff[0] & (1U << 0)) != 0U)
                {
                    //
                    // DND BUTTON STATE
                    //
                    if(((ow_buff[0] & (1U << 1)) != 0U) && !IsDonNotDisturbActiv())
                    {
                        DoNotDisturb_On();
                        ONEWIRE_ButtonUpdateSet();
                        DISPLAY_DoNotDisturbImage();
                    }
                    else if(((ow_buff[0] & (1U << 1)) == 0U) && IsDonNotDisturbActiv())
                    {
                        DoNotDisturb_Off();
                        ONEWIRE_ButtonUpdateSet();
                        DISPLAY_DoNotDisturbImageDelete();
                    }
                    //
                    // SOS BUTTON STATE
                    //
                    if(((ow_buff[0] & (1U << 2)) != 0U) && !IsSosAlarmActiv())
                    {
                        SosAlarm_On();
                        DISPLAY_SosAlarmImage();
                        ONEWIRE_ButtonUpdateSet();
                    }
                    else if(((ow_buff[0] & (1U << 2)) == 0U) && IsSosAlarmActiv())
                    {
                        SosAlarm_Off();
                        SignalBuzzer = BUZZ_OFF;
                        ONEWIRE_ButtonUpdateSet();
                        DISPLAY_SosAlarmImageDelete();
                    }
                    //
                    // HM CALL BUTTON STATE
                    //
                    if(((ow_buff[0] & (1U << 3)) != 0U) && !IsHandmaidCallActiv())
                    {
                        HandmaidCall_On();
                        LED_HandmaidGreen_On();
                        ONEWIRE_ButtonUpdateSet();
                    }
                    else if(((ow_buff[0] & (1U << 3)) == 0U) && IsHandmaidCallActiv())
                    {
                        HandmaidCall_Off();
                        LED_HandmaidGreen_Off();
                        ONEWIRE_ButtonUpdateSet();
                    }
                    //
                    // OPEN DOOR BUTTON STATE
                    //
                    if(((ow_buff[0] & (1U << 4)) != 0U) && !IsDoorlockRemoteOpenActiv())
                    {
                        DoorlockRemoteOpenSet();;
                        DoorLockCoil_On();
                        DISPLAY_CardValidImage();
                        SignalBuzzer = BUZZ_CARD_VALID;
                        RC522_ExtendDoorlockTimeSet();

                        if(IsONEWIRE_ThermostatDisplayDoorBellActive() != 0xFFU)
                        {
                            ONEWIRE_ThermostatDisplayUserInterface();
                            ONEWIRE_DisplayUpdateSet();
                        }
                    }
                    //
                    // OK BUTTON STATE
                    //
                    if((ow_buff[0] & (1U << 5)) != 0U)
                    {
                        if(IsONEWIRE_ThermostatDisplayDoorBellActive() != 0xFFU)
                        {
                            ONEWIRE_ThermostatDisplayUserInterface();
                            ONEWIRE_DisplayUpdateSet();
                        }
                    }
                }
                /** ==========================================================================*/
                /**			G E T		T H E R M O S T A T 	P A R A M E T E R S			  */
                /** ==========================================================================*/
                if((ow_buff[2] & (1U << 6)) != 0U)
                {
                    thermostat_setpoint[ow_pcnt]            = ow_buff[4];
                    thermostat_ambient_temperature[ow_pcnt] = ow_buff[6];
                    thermostat_fancoil_temperature[ow_pcnt] = ow_buff[8];
                    if(ow_pcnt == 0U) temperature_measured  = ow_buff[6] * 10U;
                }
                /** ==========================================================================*/
                /**		    G E T		    D I S P L A Y       I M A G E       I D           */
                /** ==========================================================================*/
                if((ow_buff[2] & (1U << 7)) != 0U)
                {
                    thermostat_display_image_id[ow_pcnt] = ow_buff[10];
                }
                break;
            }
        }

        if((ow_cmd > ONEWIRE_GET_STATE) && (ow_cmd <= ONEWIRE_CLEAR_ERROR))
        {
            ow_buff[16] = CalcCRC(ow_buff, 16U);
            status += HAL_UART_Transmit(&huart2, ow_buff, 9U, ONEWIRE_TRANSFER_TIMEOUT);
        }

        if(status == HAL_OK)
        {
            ow_err = 0U;
            ++ow_pcnt;
        }
        else
        {
            ++ow_err;

            if(ow_err >= 5U)
            {
                ow_err = 0U;
                ++ow_pcnt;
            }
        }

        if(IsONEWIRE_UpdateSelectetdActiv())
        {
            if(ow_pcnt != (thermostat_id - 1U))
            {
                ONEWIRE_UpdateSelectetdReset();
                ONEWIRE_UpdateLockReset();
            }
        }

        if(ow_pcnt >= onewire_device_connected)
        {
            ow_pcnt = 0U;

            if(!IsONEWIRE_ButtonUpdateActiv() && !IsONEWIRE_SetpointUpdateActiv() &&
                !IsONEWIRE_DisplayUpdateActiv() && !IsONEWIRE_ClearErrorActiv())
            {
                ONEWIRE_UpdateLockReset();
            }

            if(IsDoorlockRemoteOpenActiv())
            {
                DoorlockRemoteOpenReset();
            }
        }

        ow_tim = ONEWIRE_UPDATE_TIME;
        ow_tmr = HAL_GetTick();
    }
}


void ONEWIRE_ScanBus(void)
{
    uint8_t owire_buff[96];
    uint32_t owire_error = 0U;
    uint32_t owire_pcnt = ONEWIRE_THERMOSTAT_FIRST_ADDRESS;
    uint32_t owire_tmr;
    uint32_t owire_stat;

    onewire_device_connected = 0U;
    ClearBuffer(thermostat_addresse, 9);
    
	if(ONEWIRE_Search(&ds18b20_1, &onewire_device_connected))
	{
		ONEWIRE_DalasSensorConnected();
	}
	else
	{
		ONEWIRE_DalasSensorNotConnected();
		ONEWIRE_SetUsart(ONEWIRE_9600);

        do
        {
#ifndef	USE_DEBUGGER
            HAL_IWDG_Refresh(&hiwdg);
#endif
            ClearBuffer(owire_buff, sizeof(owire_buff));
            huart2.Instance->DR = 0x0100U + owire_pcnt;
            owire_tmr = HAL_GetTick();
            owire_stat = 0U;

            while((HAL_UART_GetState(&huart2) != HAL_UART_STATE_READY) && (owire_stat == 0U))
            {
                if ((HAL_GetTick() - owire_tmr) >= ONEWIRE_TRANSFER_TIMEOUT)
                {
                    owire_stat = 1U;
                }
            }

            HAL_Delay(2);
            owire_buff[0] = ONEWIRE_GET_STATE;
            owire_buff[1] = 0U;
            owire_buff[16] = CalcCRC(owire_buff, 16U);
            owire_stat += HAL_UART_Transmit(&huart2, owire_buff, 9U, ONEWIRE_TRANSFER_TIMEOUT);
            __HAL_UART_FLUSH_DRREGISTER(&huart2);
            owire_stat += HAL_UART_Receive(&huart2, owire_buff, 9U, ONEWIRE_TRANSFER_TIMEOUT);

            if((owire_stat == 0U) && (owire_buff[16] == CalcCRC(owire_buff, 16U)))
            {
                thermostat_addresse[onewire_device_connected] = owire_pcnt;
                ONEWIRE_ThermostatConnected();
                HAL_Delay(ONEWIRE_UPDATE_TIME);
                ++onewire_device_connected;
                owire_error = 0U;
                ++owire_pcnt;
            }
            else
            {
                ++owire_error;

                if(owire_error >= 5U)
                {
                    owire_error = 0U;
                    ++owire_pcnt;
                }
            }
        }
        while(owire_pcnt <= ONEWIRE_THERMOSTAT_LAST_ADDRESS);
	}    
}


void ONEWIRE_ThermostatDisplayDoorBell(void)
{
    for(uint32_t cnt = 0U; cnt < onewire_device_connected; cnt++)
    {
        thermostat_display_image_id[cnt] = 1U;
        thermostat_display_image_time[cnt] = 1U;
        thermostat_buzzer_sound_mode[cnt] = 2U;
        thermostat_buzzer_repeat_time[cnt] = 2U;
    }
}


void ONEWIRE_ThermostatDisplayUserInterface(void)
{
    for(uint32_t cnt = 0U; cnt < onewire_device_connected; cnt++)
    {
        thermostat_display_image_id[cnt] = 0U;
        thermostat_display_image_time[cnt] = 0U;
        thermostat_buzzer_sound_mode[cnt] = 0U;
        thermostat_buzzer_repeat_time[cnt] = 0U;
    }
}


void ONEWIRE_ThermostatDisplayConditionError(void)
{
    for(uint32_t cnt = 0U; cnt < onewire_device_connected; cnt++)
    {
        thermostat_display_image_id[cnt] = 101U;
        thermostat_display_image_time[cnt] = 1U;
        thermostat_buzzer_sound_mode[cnt] = 0U;
        thermostat_buzzer_repeat_time[cnt] = 0U;
    }
}


uint8_t IsONEWIRE_ThermostatDisplayDoorBellActive(void)
{
    for(uint32_t cnt = 0U; cnt < onewire_device_connected; cnt++)
    {
        if(thermostat_display_image_id[cnt] == 1U)
        {
            return cnt;
        }
    }

    return 0xFFU;
}


static void ONEWIRE_SetUsart(uint32_t setup)
{
	huart2.Instance = USART2;
	huart2.Init.BaudRate = setup;
	huart2.Init.WordLength = UART_WORDLENGTH_9B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_DeInit(&huart2) != HAL_OK) Error_Handler();
    if (HAL_UART_Init(&huart2) != HAL_OK) Error_Handler();
}



static uint8_t ONEWIRE_Search(TempSensorTypeDef* ds18b20, uint8_t* sensor_cnt)
{
	static uint8_t init_cnt = 0U;
	uint8_t last_zero, rom_byte_number, search_result;
	uint8_t id_bit, cmp_id_bit, id_bit_number;
	uint8_t rom_byte_mask, search_direction;
    static uint8_t onewire_last_discrepancy = 0U;
    static uint8_t onewire_last_device_flag = 0U;

	id_bit_number = 1U;
	last_zero = 0U;
	rom_byte_number = 0U;
	rom_byte_mask = 1U;

	if (onewire_last_device_flag == 0U)
	{
		if (ONEWIRE_Reset() == 0U)
		{
			onewire_last_discrepancy = 0U;
			onewire_last_device_flag = 0U;
			return (0U);
		}

		ONEWIRE_SendByte(ONEWIRE_SEARCH_ROM);

		do{
			id_bit = ONEWIRE_ReceiveBit();
			cmp_id_bit = ONEWIRE_ReceiveBit();

			if ((id_bit == 1U) && (cmp_id_bit == 1U)) break;
			else
			{
				if (id_bit != cmp_id_bit) search_direction = id_bit;  // bit write value for search
				else
				{
					if (id_bit_number < onewire_last_discrepancy)
					{
						search_direction = ((ds18b20->rom_code[rom_byte_number] & rom_byte_mask) > 0U);
					}
					else search_direction = (id_bit_number == onewire_last_discrepancy);

					if (search_direction == 0U)
					{
						last_zero = id_bit_number;
					}
				}

				if (search_direction == 1) ds18b20->rom_code[rom_byte_number] |= rom_byte_mask;
				else ds18b20->rom_code[rom_byte_number] &= ~rom_byte_mask;

				ONEWIRE_SendBit(search_direction);
				id_bit_number++;
				rom_byte_mask <<= 1U;

				if (rom_byte_mask == 0U)
				{
					rom_byte_number++;
					rom_byte_mask = 1U;
				}
			}
		} while(rom_byte_number < 8U);

		if (!(id_bit_number < 65U))
		{
			search_result = 1U;
			onewire_last_discrepancy = last_zero;
			if (onewire_last_discrepancy == 0U) onewire_last_device_flag = 1U;
		}
	}

	if ((search_result == 0U) || (ds18b20->rom_code[0] == 0U))
	{
		onewire_last_discrepancy = 0U;
		onewire_last_device_flag = 0U;
		return (0U);
	}
	else
	{
		init_cnt++;
		*sensor_cnt = init_cnt;
		ds18b20->sensor_id = init_cnt;
		return (init_cnt);
	}
}


static uint8_t ONEWIRE_Reset(void)
{
    uint8_t onew_buff[2];

	ONEWIRE_SetUsart(ONEWIRE_9600);
	onew_buff[0] = 0xF0U;
    onew_buff[1] = 0U;
	if (HAL_UART_Transmit(&huart2, onew_buff, 1U, ONEWIRE_TIMEOUT) != HAL_OK) Error_Handler();
	if (HAL_UART_Receive(&huart2, &onew_buff[1], 1U, ONEWIRE_TIMEOUT) != HAL_OK) Error_Handler();
	ONEWIRE_SetUsart(ONEWIRE_115200);

	if((onew_buff[1] != 0xF0U) && (onew_buff[1] != 0U) && (onew_buff[1] != 0xFFU)) return (1U);
	else return(0U);
}


static void ONEWIRE_SendByte(uint8_t data)
{
	uint32_t i;

	for(i = 0U; i < 8U; i++)
	{
		ONEWIRE_SendBit(data & (1U << i));
	}
}


static uint8_t ONEWIRE_ReceiveBit(void)
{
	uint8_t onew_buff[2];

	onew_buff[0] = 0xFFU;
    onew_buff[1] = 0U;
	if(HAL_UART_Transmit(&huart2, onew_buff, 1U, ONEWIRE_TIMEOUT) != HAL_OK) Error_Handler();
	if(HAL_UART_Receive(&huart2, &onew_buff[1], 1U, ONEWIRE_TIMEOUT) != HAL_OK) Error_Handler();

	if(onew_buff[0] == onew_buff[1]) return(1U);
	else return(0U);
}


static void ONEWIRE_SendBit(uint8_t send_bit)
{
	uint8_t onew_buff[2];

	if(send_bit == 0U)  onew_buff[0] = 0U;
	else onew_buff[0] = 0xFFU;
    onew_buff[1] = 0U;
	if(HAL_UART_Transmit(&huart2, onew_buff, 1U, ONEWIRE_TIMEOUT) != HAL_OK) Error_Handler();
	if(HAL_UART_Receive(&huart2, &onew_buff[1], 1U, ONEWIRE_TIMEOUT) != HAL_OK) Error_Handler();
}


/******************************   END OF FILE  **********************************/

