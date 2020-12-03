/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : MIFARE Reader
  ******************************************************************************
  */
  
  
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "rc522.h"
#include "signal.h"
//#include "logger.h"
//#include "nrf24l01.h"


/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
IWDG_HandleTypeDef hiwdg;
SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
UART_HandleTypeDef huart1;


/* Private variables ---------------------------------------------------------*/
//uint8_t TxAddress[] = {RADIO_BRIDGE_ADDRESS};
//uint8_t MyAddress[] = {DOOR_1_ADDRESS};

	
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_IWDG_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);


/* Program code    ----------------------------------------------------------*/
int main(void)
{
//	uint8_t cnt;
//	static uint8_t temp_sec;
//	RTC_TimeTypeDef sTime;
//	RTC_DateTypeDef sDate;
//	NRF24L01_Transmit_Status_t txrxStatus;

	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_I2C1_Init();
	MX_IWDG_Init();
	MX_SPI1_Init();
	MX_SPI2_Init();
	RC522_Init();
	SIGNAL_Init();
//	LOGGER_Init();
    HAL_IWDG_Refresh(&hiwdg);
//	NRF24L01_Init(DOOR_1_CHANEL, 32);
//	NRF24L01_SetRF(NRF24L01_DataRate_250k, NRF24L01_OutputPower_0dBm);
//	NRF24L01_SetMyAddress(MyAddress);
//	NRF24L01_SetTxAddress(TxAddress);

	while (1)
	{
		RC522_Service();
		SIGNAL_Service();

//		if(!IsRADIO_ModulPresent())
//		{
//			HAL_Delay(5);
//		}
//		else if (NRF24L01_DataReady()) 
//		{
//			ClearBuffer(radio_rx_buffer, RADIO_BUFFER_SIZE);
//			NRF24L01_GetData(radio_rx_buffer);
//			
//			if ((radio_rx_buffer[0] == '1') && \
//				(radio_rx_buffer[1] == 'O') && \
//				(radio_rx_buffer[2] == 'P') && \
//				(radio_rx_buffer[3] == 'E') && \
//				(radio_rx_buffer[4] == 'N'))
//			{
//				LED_GreenOn();
//				LED_RedOn();
//				SignalBuzzer = BUZZ_CARD_VALID;
//				DOORLOCK_StartTimer(OPEN_TIME);
//				RC522_StartTimer(RC522_CARD_VALID_EVENT_TIME);		
//				LogEvent.log_event = PC_COMMAND;
//				LogEvent.log_card_id[0] = 0;
//				LogEvent.log_card_id[1] = 0;
//				LogEvent.log_card_id[2] = 0;
//				LogEvent.log_card_id[3] = 0;
//				LogEvent.log_card_id[4] = 0;		
//				LOGGER_Write();
//				
//				ClearBuffer(radio_tx_buffer, RADIO_BUFFER_SIZE);				
//				radio_tx_buffer[0] =  0;
//				radio_tx_buffer[1] =  0;
//				radio_tx_buffer[2] =  0;
//				radio_tx_buffer[3] =  0;
//				radio_tx_buffer[4] =  0;
//				radio_tx_buffer[5] = date.Date;
//				radio_tx_buffer[6] = date.Month;
//				radio_tx_buffer[7] = date.Year;
//				radio_tx_buffer[8] = time.Hours;
//				radio_tx_buffer[9] = time.Minutes;
//				radio_tx_buffer[10] = time.Seconds;
//				radio_tx_buffer[11] = LogEvent.log_event;
//				radio_tx_buffer[12] = ReaderId[4];
//				RADIO_TxBufferReady();
//			}
//			else if ((radio_rx_buffer[0] == '1') && \
//					 (radio_rx_buffer[1] == 'G') && \
//					 (radio_rx_buffer[2] == 'E') && \
//					 (radio_rx_buffer[3] == 'T') && \
//					 (radio_rx_buffer[4] == '#'))
//			{
//				
//				while(logger_list_count > 0)
//				{
//					LOGGER_Read(0x00);
//					
//					ClearBuffer(radio_tx_buffer, RADIO_BUFFER_SIZE);
//					
//					for(cnt = 0; cnt < LOG_SIZE; cnt++)
//					{
//						radio_tx_buffer[cnt] = aEepromBuffer[cnt];
//					}
//					
//					NRF24L01_Transmit(radio_tx_buffer);
//			
//					do
//					{ 
//						txrxStatus = NRF24L01_GetTransmissionStatus();				
//					} 
//					while (txrxStatus == NRF24L01_Transmit_Status_Sending);
//					
//					NRF24L01_PowerUpRx();
//					LOGGER_Delete(0x00);
//					HAL_Delay(50);
//					HAL_IWDG_Refresh(&hiwdg);
//				}
//			}
//			else if ((radio_rx_buffer[0] == '1') && \
//					 (radio_rx_buffer[1] == 'S') && \
//					 (radio_rx_buffer[2] == 'E') && \
//					 (radio_rx_buffer[3] == 'T') && \
//					 (radio_rx_buffer[4] == '$'))	
//			{//0x31 0x53 0x45 0x54 0x24 0x15 0x22 0x00 0x02 0x02 0x27 0x18
//				sTime.Hours = radio_rx_buffer[5];
//				sTime.Minutes = radio_rx_buffer[6];
//				sTime.Seconds = radio_rx_buffer[7];

//				if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
//				{
//					_Error_Handler(__FILE__, __LINE__);
//				}

//				sDate.WeekDay = radio_rx_buffer[8];
//				sDate.Month = radio_rx_buffer[9];
//				sDate.Date = radio_rx_buffer[10];
//				sDate.Year = radio_rx_buffer[11];

//				if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
//				{
//					_Error_Handler(__FILE__, __LINE__);
//				}
//				
//			}
//			
//			NRF24L01_PowerUpRx();						
//		}
//		else if(IsRADIO_TxBufferReady())
//		{
//			NRF24L01_Transmit(radio_tx_buffer);
//			
//			do
//			{ 
//				txrxStatus = NRF24L01_GetTransmissionStatus();				
//			} 
//			while (txrxStatus == NRF24L01_Transmit_Status_Sending);
//			
//			NRF24L01_PowerUpRx();			
//			RADIO_TxBufferNotReady();
//		}
//		
//		HAL_Delay(50);		
//		HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BCD);
//		
//		if(temp_sec != time.Seconds)
//		{
//			temp_sec = time.Seconds;
//			
//			/***********************************************************************
//			**
//			**		B A C K U P   R T C   D A T E	
//			**
//			***********************************************************************/
//			HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BCD);
//			HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, date.Date);
//			HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, date.Month);
//			HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, date.WeekDay);
//			HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, date.Year);
//		}

		HAL_IWDG_Refresh(&hiwdg);
	}
}

void Delay(__IO uint32_t nCount)
{
	__IO uint32_t index = 0;
	
	for(index = nCount; index != 0; index--)
	{
	}
}


void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	/**Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
	PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
	
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
	/**Configure the Systick interrupt time 
	*/
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	/**Configure the Systick 
	*/
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}



/* I2C1 init function */
static void MX_I2C1_Init(void)
{

	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 100000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* IWDG init function */
static void MX_IWDG_Init(void)
{

	hiwdg.Instance = IWDG;
	hiwdg.Init.Prescaler = IWDG_PRESCALER_4;
	hiwdg.Init.Reload = 4095;
	if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

}


/* SPI1 init function */
static void MX_SPI1_Init(void)
{

	/* SPI1 parameter configuration*/
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* SPI2 init function */
static void MX_SPI2_Init(void)
{
	/* SPI2 parameter configuration*/
	hspi2.Instance = SPI2;
	hspi2.Init.Mode = SPI_MODE_MASTER;
	hspi2.Init.Direction = SPI_DIRECTION_2LINES;
	hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi2.Init.NSS = SPI_NSS_SOFT;
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi2.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi2) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

}


static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, BUZZER_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, LED_RED_Pin|LED_GREEN_Pin|MIFARE_CS_Pin|MIFARE_RST_Pin|USB_SEL_Pin, GPIO_PIN_SET);
	
	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, ENC28J60_RST_Pin|ENC28J60_CS_Pin|NRF24L01_CSN_Pin|EE_WP_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, NRF24L01_CE_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : BUZZER_Pin LED_RED_Pin LED_GREEN_Pin MIFARE_CS_Pin 
		   MIFARE_RST_Pin USB_SEL_Pin */
	GPIO_InitStruct.Pin = BUZZER_Pin|LED_RED_Pin|LED_GREEN_Pin|MIFARE_CS_Pin|MIFARE_RST_Pin|USB_SEL_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : ENC28J60_Pin ENC28J60_INT_Pin NRF24L01_IRQ_Pin */
	GPIO_InitStruct.Pin = ENC28J60_Pin|ENC28J60_INT_Pin|NRF24L01_IRQ_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pins : ENC28J60_RST_Pin ENC28J60_CS_Pin NRF24L01_CSN_Pin NRF24L01_CE_Pin 
		   EE_WP_Pin */
	GPIO_InitStruct.Pin = ENC28J60_RST_Pin|ENC28J60_CS_Pin|NRF24L01_CSN_Pin|NRF24L01_CE_Pin|EE_WP_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}



void _Error_Handler(char *file, int line)
{
	while(1) 
	{
	}
}


void ClearBuffer(unsigned char *buffer, unsigned int  size)
{
	uint16_t bcnt = 0;
	
	while(bcnt < size) buffer[bcnt++] = NULL;
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
