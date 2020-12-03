/**
 ******************************************************************************
 * File Name          : one_wire.c
 * Date               : 17/11/2016 00:59:00
 * Description        : one wire communication modul
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
TempSensorTypeDef ds18b20_1;
//TempSensorTypeDef ds18b20_2;
//TempSensorTypeDef ds18b20_3;
//TempSensorTypeDef ds18b20_4;
//TempSensorTypeDef ds18b20_5;
//TempSensorTypeDef ds18b20_6;
//TempSensorTypeDef ds18b20_7;
//TempSensorTypeDef ds18b20_8;


/* Private Define ------------------------------------------------------------*/
#define ONEWIRE_TIMEOUT				        800U
#define ONEWIRE_SEARCH_ROM			        ((uint8_t)0xf0U)
#define ONEWIRE_READ_ROM			        ((uint8_t)0x33U)
#define ONEWIRE_MATCH_ROM			        ((uint8_t)0x55U)
#define ONEWIRE_SKIP_ROM			        ((uint8_t)0xCCU)
#define ONEWIRE_ALARM_SEARCH		        ((uint8_t)0xECU)
#define ONEWIRE_CONVERT				        ((uint8_t)0x44U)
#define ONEWIRE_WRITE_SCRATCHPAD	        ((uint8_t)0x4EU)
#define ONEWIRE_READ_SCRATCHPAD		        ((uint8_t)0xBEU)
#define ONEWIRE_COPY_SCRATCHPAD		        ((uint8_t)0x48U)
#define ONEWIRE_RACALL_E2			        ((uint8_t)0xB8U)
#define ONEWIRE_READ_POWER_SUPPLY	        ((uint8_t)0xB4U)

#define ONEWIRE_GET_STATE                   ((uint8_t)0xA0U)    
#define ONEWIRE_SET_BUTTON_STATE            ((uint8_t)0xA1U)
#define ONEWIRE_SET_DATE_TIME               ((uint8_t)0xA2U)
#define ONEWIRE_SET_THERMOSTAT_SP           ((uint8_t)0xA3U)
#define ONEWIRE_SET_DISPLAY_IMAGE           ((uint8_t)0xA4U)
#define ONEWIRE_CLEAR_THERMOSTAT_ERROR      ((uint8_t)0xA5U)


/* Private Variable ----------------------------------------------------------*/
__IO uint32_t onewire_flags;
__IO uint8_t onewire_interface_address;

static uint8_t onewire_buffer[ONEWIRE_BUF_SIZE];
uint8_t onewire_last_family_discrepancy;
static uint8_t onewire_last_discrepancy;
static uint8_t onewire_last_device_flag;
uint8_t onewire_sensor_number;


/* Private Macro -------------------------------------------------------------*/
/* Private Function Prototype ------------------------------------------------*/
static uint8_t ONEWIRE_Reset(void);
uint8_t ONEWIRE_ReadByte(void);
static void ONEWIRE_SendByte(uint8_t data);
static void ONEWIRE_SendBit(uint8_t send_bit);
static uint8_t ONEWIRE_ReceiveBit(void);
void ONEWIRE_Send(uint8_t *command, uint8_t lenght);
static void ONEWIRE_Receive(uint8_t *data, uint8_t lenght);
uint8_t ONEWIRE_ReadROM(uint8_t *ow_address);
static uint8_t ONEWIRE_CrcCheck(uint8_t *ow_address, uint8_t lenght);
static void ONEWIRE_Pack (uint8_t cmd, uint8_t buffer[8]);
static uint8_t ONEWIRE_Unpack (uint8_t buffer[8]);
uint16_t ONEWIRE_ScratchpadToTemperature(uint16_t scratchpad);
void ONEWIRE_ResetSearch(void);
uint8_t ONEWIRE_Search(TempSensorTypeDef* ds18b20, uint8_t* sensor_cnt);
static void ONEWIRE_SetUsart(uint32_t setup);
void ONEWIRE_Select(uint8_t* addr);


/* Program Code  -------------------------------------------------------------*/
void ONEWIRE_Init(void)
{
    ONEWIRE_ListenAddressSet();
    ONEWIRE_SetUsart(ONEWIRE_9600);
    __HAL_UART_FLUSH_DRREGISTER(&huart2);
    HAL_UART_Receive_IT(&huart2, onewire_buffer, 1U);
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
    if(huart->Instance == USART2)
	{
        
    }
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    if(huart->Instance == USART2)
	{
        /** ==========================================================================*/
        /**     O N E W I R E       I N T E R F A C E           A D D R E S S E D     */
        /** ==========================================================================*/
        if(IsONEWIRE_ListenAddressActiv())
        {
            ONEWIRE_ListenAddressReset();
            ClearBuffer(onewire_buffer, 24U);
            __HAL_UART_FLUSH_DRREGISTER(&huart2);
            HAL_UART_Receive_IT(&huart2, onewire_buffer, 9U);
            return;
        }
        else if((onewire_buffer[1] == 0U)  && (onewire_buffer[16] == CalcCRC(onewire_buffer, 16U)))
        {
            /** ==========================================================================*/
            /**     O N E W I R E     C O M M A N D     P A C K E T     R E C E I V E D   */
            /** ==========================================================================*/
            switch(onewire_buffer[0])
            {
                /** ==========================================================================*/
                /**			S E T		N E W		B U T T O N 		S T A T E			  */
                /** ==========================================================================*/
                case ONEWIRE_SET_BUTTON_STATE:
                {
                    // BUTTON DND STATE
                    if((onewire_buffer[2] == 1U) && !IsBUTTON_DndActiv())
                    {
                        BUTTON_DndActivSet();
                        BUTTON_CallMaidActivReset();
                        ONEWIRE_ButtonReadyForDrawingSet();
                    }
                    else if((onewire_buffer[2] == 0U) && IsBUTTON_DndActiv())
                    {
                        BUTTON_DndActivReset();
                        ONEWIRE_ButtonReadyForDrawingSet();
                    }
                    // BUTTON SOS STATE
                    if((onewire_buffer[4] == 1U) && !IsBUTTON_SosResetActiv())
                    {
                        BUTTON_SosActivSet();
                        ONEWIRE_ButtonReadyForDrawingSet();
                    }
                    else if((onewire_buffer[4] == 0U) && IsBUTTON_SosResetActiv())
                    {
                        BUTTON_SosActivReset();
                        ONEWIRE_ButtonReadyForDrawingSet();
                    }
                    // BUTTON_CALMAID_STATE
                    if((onewire_buffer[6] == 1U)  && !IsBUTTON_CallMaidActiv()) 
                    {
                        BUTTON_DndActivReset();
                        BUTTON_CallMaidActivSet();
                        ONEWIRE_ButtonReadyForDrawingSet();
                    }
                    else if((onewire_buffer[6] == 0U) && IsBUTTON_CallMaidActiv())
                    {
                        BUTTON_CallMaidActivReset();
                        ONEWIRE_ButtonReadyForDrawingSet();
                    }
                    // ROOM CONTROLLER STATUS
                    if(onewire_buffer[8] == 0U) ONEWIRE_HVAC_ContactorOff();
                    else ONEWIRE_HVAC_ContactorOn();
                    
                    if(onewire_buffer[10] == 0U) ONEWIRE_WindowSwitchOpen();
                    else ONEWIRE_WindowSwitchClosed();
                    
                    if(onewire_buffer[12] == 0U) ONEWIRE_PowerContactorOff();
                    else ONEWIRE_PowerContactorOn();
                    
                    if(onewire_buffer[14] == 0U) ONEWIRE_DoorBellOff();
                    else ONEWIRE_DoorBellOn();
                    
                    break;
                }
                /** ==========================================================================*/
                /**			S E T		N E W		D A T E 	& 		T I M E				  */
                /** ==========================================================================*/
                case ONEWIRE_SET_DATE_TIME:
                {    
                    date.Date = onewire_buffer[2];
                    date.WeekDay = onewire_buffer[4];
                    date.Month = onewire_buffer[6];
                    date.Year = onewire_buffer[8];
                    time.Hours = onewire_buffer[10];
                    time.Minutes = onewire_buffer[12];
                    time.Seconds = onewire_buffer[14];
                    HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BCD);
                    HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BCD);
                    HAL_RTC_WaitForSynchro(&hrtc);
                    break;
                }
                /** ==========================================================================*/
                /**     C L E A R		T H E R M O S T A T 	E R R O R       E V E N T     */
                /** ==========================================================================*/
                case ONEWIRE_CLEAR_THERMOSTAT_ERROR:
                {
                    FANCOIL_RPM_SensorErrorReset();
                    FANCOIL_NTC_SensorErrorReset();
                    FANCOIL_HeatingTempErrorReset();
                    FANCOIL_CoolingTempErrorReset();
                    FANCOIL_FreezingAlarmReset();
                    AMBIENT_NTC_SensorErrorReset();
                    break;
                }
                /** ==========================================================================*/
                /**			S E T		T H E R M O S T A T 	P A R A M E T E R S			  */
                /** ==========================================================================*/                
                case ONEWIRE_SET_THERMOSTAT_SP:
                {
                    if (IsSYSTEM_StartupActiv()) break;
                        
                    temperature_setpoint = onewire_buffer[2];
                    BSP_EEPROM_WriteBuffer(&temperature_setpoint, EE_THERMOSTAT_SET_POINT, 1U);
                    BSP_EEPROM_WaitEepromStandbyState();
                    
                    if(IsTemperatureRegulatorOn()) 
                    {
                        THERMOSTAT_ControllerOn();
                        ONEWIRE_ForceThermostatOnSet();
                    }
                    else 
                    {
                        THERMOSTAT_ControllerOff();
                        ONEWIRE_ForceThermostatOffSet();
                    }
                    
                    onewire_buffer[0] = (thermostat_flags >> 24U);
                    onewire_buffer[1] = (thermostat_flags >> 16U);
                    onewire_buffer[2] = (thermostat_flags >> 8U);
                    onewire_buffer[3] = (thermostat_flags & 0xffU);
                    BSP_EEPROM_WriteBuffer(onewire_buffer, EE_THERMOSTAT_FLAGS, 4U);
                    BSP_EEPROM_WaitEepromStandbyState();
                    DISPLAY_SetpointUpdateSet();
                    break;
                }
                /** ==========================================================================*/
                /**		    S E T		N E W		D I S P L A Y    M E S S A G E            */
                /** ==========================================================================*/
                case ONEWIRE_SET_DISPLAY_IMAGE:             
                {
                    if(IsONEWIRE_UpdateDisplayImageActiv() || IsSYSTEM_StartupActiv()) break;
                    else if(display_image_id != onewire_buffer[2])
                    {
                        display_image_id = onewire_buffer[2];
                        display_image_time = onewire_buffer[4];
                        buzzer_signal_id = onewire_buffer[6];
                        buzzer_signal_time = onewire_buffer[8];
                        DISPLAY_UpdateSet();                    
                    }
                    break;
                }
                /** ==========================================================================*/
                /**		        G E T	    	T H E R M O S T A T      S T A T E            */
                /** ==========================================================================*/
                case ONEWIRE_GET_STATE:
                {
                    ClearBuffer(onewire_buffer, 24U);
                    /** ==========================================================================*/
                    /**			S E N D	        B U T T O N 		S T A T E                     */
                    /** ==========================================================================*/
                    if(IsONEWIRE_SendNewButtonStateActiv())
                    {
                        (onewire_buffer[0] |= (1U << 0)); 
                        if (IsBUTTON_DndActiv())        (onewire_buffer[0] |= (1U << 1));
                        if (IsBUTTON_SosResetActiv())   (onewire_buffer[0] |= (1U << 2));
                        if (IsBUTTON_CallMaidActiv())   (onewire_buffer[0] |= (1U << 3));
                        if (IsBUTTON_OpenDoorActiv())   (onewire_buffer[0] |= (1U << 4)), BUTTON_OpenDoorReset();
                        if (IsBUTTON_OkActiv())         (onewire_buffer[0] |= (1U << 5)), BUTTON_OkActivReset();
                        ONEWIRE_SendNewButtonStateReset();
                    }
                    /** ==========================================================================*/
                    /**			S E N D		T H E R M O S T A T 	E R R O R       E V E N T     */
                    /** ==========================================================================*/
                    if(IsFANCOIL_RPM_SensorErrorActiv())    (onewire_buffer[2] |= (1U << 0)), FANCOIL_RPM_SensorErrorReset();
                    if(IsFANCOIL_NTC_SensorErrorActiv())    (onewire_buffer[2] |= (1U << 1)), FANCOIL_NTC_SensorErrorReset();
                    if(IsFANCOIL_HeatingTempErrorActiv())   (onewire_buffer[2] |= (1U << 2)), FANCOIL_HeatingTempErrorReset();
                    if(IsFANCOIL_CoolingTempErrorActiv())   (onewire_buffer[2] |= (1U << 3)), FANCOIL_CoolingTempErrorReset();
                    if(IsFANCOIL_FreezingAlarmActiv())      (onewire_buffer[2] |= (1U << 4)), FANCOIL_FreezingAlarmReset();
                    if(IsAMBIENT_NTC_SensorErrorActiv())    (onewire_buffer[2] |= (1U << 5)), AMBIENT_NTC_SensorErrorReset();
                    /** ==========================================================================*/
                    /**			S E T		T H E R M O S T A T 	P A R A M E T E R S			  */
                    /** ==========================================================================*/
                    if(IsONEWIRE_SendThermostatVariableActiv())
                    {
                        (onewire_buffer[2] |= (1U << 6));
                        onewire_buffer[4] = temperature_setpoint;
                        onewire_buffer[6] = (room_temperature / 10U); 
                        onewire_buffer[8] = (fancoil_temperature / 10U);
                        ONEWIRE_SendThermostatVariableReset();
                    }
                    /** ==========================================================================*/
                    /**		S E T		A C T I V		D I S P L A Y    M E S S A G E		      */
                    /** ==========================================================================*/
                    if(IsONEWIRE_UpdateDisplayImageActiv())
                    {
                        (onewire_buffer[2] |= (1U << 7));
                        onewire_buffer[10] = display_image_id;
                        ONEWIRE_UpdateDisplayImageReset();
                    }
                    HAL_Delay(2);
                    onewire_buffer[16] = CalcCRC(onewire_buffer, 16U);
                    HAL_UART_Transmit(&huart2, onewire_buffer, 9U, ONEWIRE_TRANSFER_TIMEOUT);
                    break;
                }
            }            
        }
        
        ONEWIRE_Init();
	}
}


void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart)
{
	if(huart->Instance == USART2)
	{
        __HAL_UART_CLEAR_PEFLAG(&huart2);
        __HAL_UART_CLEAR_FEFLAG(&huart2);
        __HAL_UART_CLEAR_NEFLAG(&huart2);
        __HAL_UART_CLEAR_IDLEFLAG(&huart2);
        __HAL_UART_CLEAR_OREFLAG(&huart2);
        __HAL_UART_FLUSH_DRREGISTER(&huart2);
        huart->ErrorCode = HAL_UART_ERROR_NONE;
        ONEWIRE_Init();
	}
}

static uint8_t ONEWIRE_Reset(void)
{
	ONEWIRE_SetUsart(ONEWIRE_9600);
	onewire_buffer[0] = 0xf0U;
	HAL_UART_Transmit(&huart2, onewire_buffer, 1, ONEWIRE_TIMEOUT);
	HAL_UART_Receive(&huart2, onewire_buffer, 1, ONEWIRE_TIMEOUT);
	ONEWIRE_SetUsart(ONEWIRE_115200);
	if((onewire_buffer[0] != 0xf0U) && (onewire_buffer[0] != 0x00U) && (onewire_buffer[0] != 0xffU)) return (1U);
	else return(0U);	
}


static void ONEWIRE_SetUsart(uint32_t setup)
{
	huart2.Instance        		= USART2;
	huart2.Init.BaudRate		= setup;
	huart2.Init.WordLength 		= UART_WORDLENGTH_9B;
	huart2.Init.StopBits   		= UART_STOPBITS_1;
	huart2.Init.Parity     		= UART_PARITY_NONE;
	huart2.Init.HwFlowCtl  		= UART_HWCONTROL_NONE;
	huart2.Init.Mode       		= UART_MODE_TX_RX;
	huart2.Init.OverSampling	= UART_OVERSAMPLING_16;
    huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    
    if (HAL_MultiProcessor_Init(&huart2, onewire_interface_address, UART_WAKEUPMETHOD_ADDRESSMARK) != HAL_OK) Error_Handler(ONEWIRE_FUNC, USART_DRIVER);
    HAL_MultiProcessor_EnableMuteMode(&huart2);
    HAL_MultiProcessor_EnterMuteMode(&huart2);
}


static void ONEWIRE_SendByte(uint8_t data)
{
	uint32_t i;
	
	for(i = 0U; i < 8U; i++, (data = data >> 1U))
	{
		ONEWIRE_SendBit(data & (1U << 0));
	}
}


uint8_t ONEWIRE_ReadByte(void)
{
	uint8_t rd_byte, i;
	
	for(i = 0U; i < 8U; i++)
	{
		rd_byte = (rd_byte >> 1U) + 0x80U * ONEWIRE_ReceiveBit();
	}
	
	return rd_byte;
}


static uint8_t ONEWIRE_ReceiveBit(void)
{
	uint8_t txd, rxd;
	txd = 0xffU;	
	HAL_UART_Transmit(&huart2, &txd, 1U, ONEWIRE_TIMEOUT);	
	HAL_UART_Receive(&huart2, &rxd, 1U, ONEWIRE_TIMEOUT);	
	if(rxd == txd) return(1U);
	else return(0U);
}


static void ONEWIRE_SendBit(uint8_t send_bit)
{
	uint8_t txb, rxb;	
	if(send_bit == 0U)  txb = 0x00U;
	else txb = 0xffU;	
	HAL_UART_Transmit(&huart2, &txb, 1U, ONEWIRE_TIMEOUT);
	HAL_UART_Receive(&huart2, &rxb, 1U, ONEWIRE_TIMEOUT);
}


void ONEWIRE_Send(uint8_t *command, uint8_t lenght)
{
	uint32_t i;
	
	uint32_t one_wire_lenght = lenght * 8U;
	
	for (i = 0U;  i < lenght; i++) 
	{
		ONEWIRE_Pack(command[i], &(onewire_buffer[i * 8U]));
	}
	
	HAL_UART_Transmit(&huart2, onewire_buffer, one_wire_lenght, ONEWIRE_TIMEOUT);
}


static void ONEWIRE_Receive(uint8_t *data, uint8_t lenght)
{
	uint32_t i;
	uint32_t onewire_lenght = lenght * 8U;
	uint8_t tx_byte = 0xffU;
	
	for(i = 0U; i < onewire_lenght; i++)
	{
		HAL_UART_Transmit(&huart2, &tx_byte, 1U, ONEWIRE_TIMEOUT);
		HAL_UART_Receive(&huart2, &onewire_buffer[i], 1U, ONEWIRE_TIMEOUT);
	}
	
	for(i = 0U; i < lenght; i++)
	{
		data[i] = ONEWIRE_Unpack(&(onewire_buffer[i * 8U]));
	}
}


uint8_t ONEWIRE_ReadROM(uint8_t *ow_address)
{
	uint8_t crc;
	
	if(ONEWIRE_Reset() != 0U)
	{
		ONEWIRE_SendByte(ONEWIRE_READ_ROM);
		ONEWIRE_Receive(ow_address, 8U);
		crc = ONEWIRE_CrcCheck(ow_address, 7U);
		if((crc != ow_address[7U]) || (crc == 0U))return (1U);
		else return(0U);
	}
	else return (2U);
}


static uint8_t ONEWIRE_CrcCheck(uint8_t *ow_address, uint8_t lenght)
{
	uint8_t crc = 0U;
	uint8_t i, j;

	for (i = 0U; i < lenght; i++) 
	{
		uint8_t inbyte = ow_address[i];
		
		for (j = 0U; j < 8U; j++) 
		{
			uint8_t mix = (crc ^ inbyte) & 0x01U;
			crc >>= 1U;
			if (mix) 
			crc ^= 0x8CU;
			inbyte >>= 1U;
		}
	}
	
	return crc;
}


static void ONEWIRE_Pack(uint8_t command, uint8_t buffer[8])
{
	uint32_t i;
	
	for (i = 0U;  i < 8U; i++)
	{
		buffer[i] = (command & (1U << i)) ? 0xffU : 0x00U;
	}
}


static uint8_t ONEWIRE_Unpack (uint8_t buffer[8])
{
	uint32_t i;
	uint8_t res = 0U;

	for (i = 0U; i < 8U; i++) 
	{
		if (buffer[i] == 0xffU)
		{
			res |=  (1U << i);
		}
	}

	return res;
}


uint16_t ONEWIRE_ScratchpadToTemperature(uint16_t scratchpad) 
{
    uint16_t result;
	
	if((scratchpad & 0xfc00U) == 0xfc00U)
	{
		scratchpad = ~scratchpad + 1U;
		result = scratchpad >> 4U; 							// cijelobrojni dio temperature
		result *= 10U; 										// 22 -> 220
		result += (((scratchpad & 0x000fU) *625U) / 1000U);
		result |= 0x8000U; 									// add minus sign
	}
	else
	{
		result = scratchpad >> 4U; 							// cijelobrojni dio temperature
		result *= 10U; 										// 22 -> 220
		result += (((scratchpad & 0x000fU) *625U) / 1000U);	// add decimal part
	}
    
    return result;
}


void ONEWIRE_ResetSearch(void) 
{
	onewire_last_discrepancy = 0U;
	onewire_last_family_discrepancy = 0U;
	onewire_last_device_flag = 0U;
	onewire_sensor_number = 0U;
}


uint8_t ONEWIRE_Search(TempSensorTypeDef* ds18b20, uint8_t* sensor_cnt) 
{
	static uint8_t init_cnt = 0U;
	uint8_t last_zero, rom_byte_number, search_result;
	uint8_t id_bit, cmp_id_bit, id_bit_number;
	uint8_t rom_byte_mask, search_direction;

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
			onewire_last_family_discrepancy = 0U;
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
						if (last_zero < 9U)  onewire_last_family_discrepancy = last_zero;
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
		onewire_last_family_discrepancy = 0U;
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


void ONEWIRE_Select(uint8_t* addr) 
{
	uint8_t i;
	
	ONEWIRE_SendByte(ONEWIRE_MATCH_ROM);
	
	for (i = 0U; i < 8U; i++) 
	{
		ONEWIRE_SendByte(*(addr + i));
	}
}


/******************************   END OF FILE  **********************************/
