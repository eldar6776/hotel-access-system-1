/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : USB Radio Bridge
  ******************************************************************************
  */
  
  
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "signal.h"
#include "nrf24l01.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"


/* Private variables ---------------------------------------------------------*/
IWDG_HandleTypeDef hiwdg;
SPI_HandleTypeDef hspi2;



/* Private variables ---------------------------------------------------------*/
uint8_t Tx1Address[] = {DOOR_1_ADDRESS};
uint8_t Tx2Address[] = {DOOR_2_ADDRESS};
uint8_t Tx3Address[] = {DOOR_3_ADDRESS};
uint8_t Tx4Address[] = {DOOR_4_ADDRESS};
uint8_t Tx5Address[] = {DOOR_5_ADDRESS};
uint8_t MyAddress[] = {RADIO_BRIDGE_ADDRESS};


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
//static void MX_IWDG_Init(void);
static void MX_SPI2_Init(void);


/* Program code    ----------------------------------------------------------*/
int main(void)
{
	NRF24L01_Transmit_Status_t txrxStatus;
	
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();	
	MX_USB_DEVICE_Init();
//	MX_IWDG_Init();
	MX_SPI2_Init();
	SIGNAL_Init();
	
	/* Initialize NRF24L01+ on channel 15 and 32bytes of payload */
	/* By default 2Mbps data rate and 0dBm output power */
	/* NRF24L01 goes to RX mode by default */
	NRF24L01_Init(DOOR_1_CHANEL, 32);
	
	/* Set RF settings, Data rate to 2Mbps, Output power to -18dBm */
	NRF24L01_SetRF(NRF24L01_DataRate_250k, NRF24L01_OutputPower_0dBm);
	
	/* Set my address, 5 bytes */
	NRF24L01_SetMyAddress(MyAddress);
	
	/* Set TX address 1, 5 bytes */
	NRF24L01_SetTxAddress(Tx1Address);
	
	
	while (1)
	{		
		if(IsUSB_DataReady())
		{
			switch(radio_tx_buffer[0])
			{
				case '1':
					NRF24L01_SetChannel(DOOR_1_CHANEL);
					NRF24L01_SetTxAddress(Tx1Address);
					break;
				
				case '2':
					NRF24L01_SetChannel(DOOR_2_CHANEL);
					NRF24L01_SetTxAddress(Tx2Address);
					break;
				
				case '3':
					NRF24L01_SetChannel(DOOR_3_CHANEL);
					NRF24L01_SetTxAddress(Tx3Address);
					break;
				
				case '4':
					NRF24L01_SetChannel(DOOR_4_CHANEL);
					NRF24L01_SetTxAddress(Tx4Address);
					break;
				
				case '5':
					NRF24L01_SetChannel(DOOR_5_CHANEL);
					NRF24L01_SetTxAddress(Tx5Address);
					break;
				
				default:
					NRF24L01_SetChannel(DOOR_1_CHANEL);
					NRF24L01_SetTxAddress(Tx1Address);
					break;
			}
					
			NRF24L01_Transmit(radio_tx_buffer);
		
			do
			{ 
				txrxStatus = NRF24L01_GetTransmissionStatus();				
			} 
			while (txrxStatus == NRF24L01_Transmit_Status_Sending);
			
			NRF24L01_PowerUpRx();	
			ClearBuffer(radio_tx_buffer, RADIO_BUFFER_SIZE);
			usb_rx_data_size = 0;
			RADIO_TxBufferNotReady();
		}
		else if (NRF24L01_DataReady()) 
		{
			ClearBuffer(radio_rx_buffer, RADIO_BUFFER_SIZE);
			NRF24L01_GetData(radio_rx_buffer);
			NRF24L01_PowerUpRx();
			CDC_Transmit_FS(radio_rx_buffer, RADIO_BUFFER_SIZE);
		}
		
		HAL_Delay(50);
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
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE
							  |RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
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

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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




/* IWDG init function */
//static void MX_IWDG_Init(void)
//{

//	hiwdg.Instance = IWDG;
//	hiwdg.Init.Prescaler = IWDG_PRESCALER_4;
//	hiwdg.Init.Reload = 4095;
//	if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
//	{
//		_Error_Handler(__FILE__, __LINE__);
//	}

//}





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
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, BUZZER_Pin|LED_RED_Pin|LED_GREEN_Pin|MIFARE_CS_Pin|MIFARE_RST_Pin|USB_SEL_Pin, GPIO_PIN_RESET);

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

	/*Configure GPIO pin : PA15 */
	GPIO_InitStruct.Pin = GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
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
