/**
 ******************************************************************************
 * File Name          : logger.c
 * Date               : 28/02/2016 23:16:19
 * Description        : data logger software modul
 ******************************************************************************
 *
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "logger.h"


/* Defines    ----------------------------------------------------------------*/
/* Types  --------------------------------------------------------------------*/
/* Variables  ----------------------------------------------------------------*/
uint8_t logger_pcnt;
uint16_t logger_list_count;
uint16_t logger_next_log_id;
uint16_t logger_next_log_address;
uint16_t logger_tmp_log_address;
uint32_t logger_timer;
uint32_t logger_flags;

LOGGER_EventTypeDef LogEvent;
LOGGER_StatusTypeDef LOGGER_Status;

uint8_t  aEepromBuffer[EE_BUFFER_SIZE];
uint8_t *p_spi_ee_buff;

extern I2C_HandleTypeDef hi2c1;
extern RTC_HandleTypeDef hrtc;
extern RTC_DateTypeDef date;
extern RTC_TimeTypeDef time;

/* Macros     ----------------------------------------------------------------*/
/* Private prototypes    -----------------------------------------------------*/
/* Program code   ------------------------------------------------------------*/
void LOGGER_Init(void)
{
	LogEvent.log_event = NULL;
	LogEvent.log_type = NULL;
	LogEvent.log_group = NULL;
	LogEvent.log_card_id[0] = NULL;
	LogEvent.log_card_id[1] = NULL;
	LogEvent.log_card_id[2] = NULL;
	LogEvent.log_card_id[3] = NULL;
	LogEvent.log_card_id[4] = NULL;
	LogEvent.log_time_stamp[0] = NULL;
	LogEvent.log_time_stamp[1] = NULL;
	LogEvent.log_time_stamp[2] = NULL;
	LogEvent.log_time_stamp[3] = NULL;
	LogEvent.log_time_stamp[4] = NULL;
	LogEvent.log_time_stamp[5] = NULL;
	
	logger_pcnt = 0;
	logger_list_count = 0;
	logger_next_log_id = 1;
	logger_next_log_address = EE_LOG_LIST_START_ADDRESS;
	logger_tmp_log_address = 0;
	LOGGER_Status = LOGGER_OK;

	aEepromBuffer[0] = logger_next_log_address >> 8;
	aEepromBuffer[1] = logger_next_log_address;
	if (HAL_I2C_Master_Transmit(&hi2c1, I2C_EE_WRITE, aEepromBuffer, 2, I2C_EE_TIMEOUT) != HAL_OK) _Error_Handler(__FILE__, __LINE__);
	if (HAL_I2C_Master_Receive(&hi2c1, I2C_EE_READ, aEepromBuffer, LOG_SIZE, I2C_EE_TIMEOUT) != HAL_OK) _Error_Handler(__FILE__, __LINE__);
	
	while(logger_next_log_address <= (EE_LOG_LIST_END_ADDRESS - LOG_SIZE))
	{	
		if((aEepromBuffer[0] == NULL) && (aEepromBuffer[1] == NULL)) break;

		logger_next_log_address += LOG_SIZE;
		++logger_next_log_id;
		++logger_list_count;
		aEepromBuffer[0] = logger_next_log_address >> 8;
		aEepromBuffer[1] = logger_next_log_address;
		if (HAL_I2C_Master_Transmit(&hi2c1, I2C_EE_WRITE, aEepromBuffer, 2, I2C_EE_TIMEOUT) != HAL_OK) _Error_Handler(__FILE__, __LINE__);
		if (HAL_I2C_Master_Receive(&hi2c1, I2C_EE_READ, aEepromBuffer, LOG_SIZE, I2C_EE_TIMEOUT) != HAL_OK) _Error_Handler(__FILE__, __LINE__);
	}
	
}

LOGGER_StatusTypeDef LOGGER_Write(void)
{
	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BCD);
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BCD);
	ClearBuffer(aEepromBuffer, LOG_SIZE);
	
	aEepromBuffer[2] = (logger_next_log_id >> 8);
	aEepromBuffer[3] = (logger_next_log_id & 0xff);
	aEepromBuffer[4] = LogEvent.log_event;
	aEepromBuffer[5] = LogEvent.log_type;
	aEepromBuffer[6] = LogEvent.log_group;
	aEepromBuffer[7] = LogEvent.log_card_id[0];
	aEepromBuffer[8] = LogEvent.log_card_id[1];
	aEepromBuffer[9] = LogEvent.log_card_id[2];
	aEepromBuffer[10] = LogEvent.log_card_id[3];
	aEepromBuffer[11] = LogEvent.log_card_id[4];
	aEepromBuffer[12] = date.Date;
	aEepromBuffer[13] = date.Month;
	aEepromBuffer[14] = date.Year;
	aEepromBuffer[15] = time.Hours;
	aEepromBuffer[16] = time.Minutes;
	aEepromBuffer[17] = time.Seconds;
	
	if(logger_next_log_address > (EE_LOG_LIST_END_ADDRESS - LOG_SIZE))
	{
		return (LOGGER_FULL);
	}
	
	I2C_EE_WriteEnable();
	aEepromBuffer[0] = logger_next_log_address >> 8;
	aEepromBuffer[1] = logger_next_log_address;
	if (HAL_I2C_Master_Transmit(&hi2c1, I2C_EE_WRITE, aEepromBuffer, 18, I2C_EE_TIMEOUT) != HAL_OK) _Error_Handler(__FILE__, __LINE__);
	if (HAL_I2C_IsDeviceReady(&hi2c1, I2C_EE_READ, I2C_EE_TRIALS, I2C_EE_WRITE_DELAY) != HAL_OK) _Error_Handler(__FILE__, __LINE__);
	I2C_EE_WriteDisable();	
	++logger_list_count;
	++logger_next_log_id;
	logger_next_log_address += LOG_SIZE;
	ClearBuffer(aEepromBuffer, LOG_SIZE);
	return (LOGGER_OK);		
}

LOGGER_StatusTypeDef LOGGER_Read(uint16_t log_id)
{
	if(logger_list_count == 0)
	{
		return(LOGGER_EMPTY);
	}
	else if(log_id == 0x0000)
	{
		logger_tmp_log_address = logger_next_log_address - LOG_SIZE;
	}
	else if(log_id >= logger_next_log_id)
	{
		return(LOGGER_WRONG_ID);
	}
	else
	{
		logger_tmp_log_address = (logger_next_log_address - ((logger_next_log_id - log_id) * LOG_SIZE));
	}
	
	aEepromBuffer[0] = logger_tmp_log_address >> 8;
	aEepromBuffer[1] = logger_tmp_log_address;
	if (HAL_I2C_Master_Transmit(&hi2c1, I2C_EE_WRITE, aEepromBuffer, 2, I2C_EE_TIMEOUT) != HAL_OK) _Error_Handler(__FILE__, __LINE__);
	if (HAL_I2C_Master_Receive(&hi2c1, I2C_EE_READ, aEepromBuffer, LOG_SIZE, I2C_EE_TIMEOUT) != HAL_OK) _Error_Handler(__FILE__, __LINE__);	
	return(LOGGER_OK);
}

LOGGER_StatusTypeDef LOGGER_Delete(uint16_t log_id)
{
	if(logger_list_count == 0)
	{
		return(LOGGER_EMPTY);
	}
	else if(log_id == 0x0000)
	{
		ClearBuffer(aEepromBuffer, LOG_SIZE);
		logger_tmp_log_address = logger_next_log_address - LOG_SIZE;
		
		if(HAL_I2C_IsDeviceReady(&hi2c1, I2C_EE_READ, 1000, I2C_EE_TIMEOUT) == HAL_OK)
		{
			I2C_EE_WriteEnable();
			aEepromBuffer[0] = logger_tmp_log_address >> 8;
			aEepromBuffer[1] = logger_tmp_log_address;
			if (HAL_I2C_Master_Transmit(&hi2c1, I2C_EE_WRITE, aEepromBuffer, 18, I2C_EE_TIMEOUT) != HAL_OK) _Error_Handler(__FILE__, __LINE__);
			if (HAL_I2C_IsDeviceReady(&hi2c1, I2C_EE_READ, I2C_EE_TRIALS, I2C_EE_WRITE_DELAY) != HAL_OK) _Error_Handler(__FILE__, __LINE__);
			I2C_EE_WriteDisable();
			logger_next_log_address -= LOG_SIZE;
			--logger_list_count;
			return (LOGGER_OK);
		}
		else
		{
			return (LOGGER_ERROR);
		}
	}
	else if(log_id == 0xffff)
	{
		I2C_EE_WriteEnable();
		ClearBuffer(aEepromBuffer, LOG_SIZE);
		logger_tmp_log_address = EE_LOG_LIST_START_ADDRESS;
		
		while(logger_tmp_log_address <= (EE_LOG_LIST_END_ADDRESS - LOG_SIZE))
		{
			if(HAL_I2C_IsDeviceReady(&hi2c1, I2C_EE_READ, I2C_EE_TRIALS, I2C_EE_WRITE_DELAY) == HAL_OK)
			{
				aEepromBuffer[0] = logger_tmp_log_address >> 8;
				aEepromBuffer[1] = logger_tmp_log_address;
				if (HAL_I2C_Master_Transmit(&hi2c1, I2C_EE_WRITE, aEepromBuffer, 18, I2C_EE_TIMEOUT) != HAL_OK) _Error_Handler(__FILE__, __LINE__);
				if (HAL_I2C_IsDeviceReady(&hi2c1, I2C_EE_READ, I2C_EE_TRIALS, I2C_EE_WRITE_DELAY) != HAL_OK) _Error_Handler(__FILE__, __LINE__);
			}
			else
			{
				I2C_EE_WriteDisable();
				return (LOGGER_ERROR);
			}	
			
			logger_tmp_log_address += LOG_SIZE;			
		}
		
		I2C_EE_WriteDisable();
		logger_list_count = 0;
		logger_next_log_id = 1;
		logger_next_log_address = EE_LOG_LIST_START_ADDRESS;

		return(LOGGER_OK);
	}
	else if(log_id >= logger_next_log_id)
	{
		return(LOGGER_WRONG_ID);
	}
	else
	{
		return(LOGGER_WRONG_ID);
	}
}

/******************************   END OF FILE  **********************************/
