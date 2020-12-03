/**
 ******************************************************************************
 * File Name          : main.c
 * Date               : 9.3.2018.
 * Description        : Hotel Room Controller Program Code
 ******************************************************************************
 *
 *	sys_status
 *	0 -> 1 = log added to list
 *	1 -> 1 = log list full
 *	2 -> 1 = file transfer successful
 *	3 -> 1 = file transfer fail
 *	4 -> 1 = sos alarm activ
 *	5 -> 1 =
 *	6 -> 1 =
 *	7 -> 1 =
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


/* Private Define  ----------------------------------------------------------*/
/* Constants ----------------------------------------------------------------*/
const char sys_info_hw[] = {"HW: DE290817"};
const char sys_info_fw[] = {"FW: RC221018"};
const uint32_t baud_rate[] ={ 2400U,   4800U,   9600U,  19200U,  38400U,
                             57600U, 115200U, 230400U, 460800U, 921600U};

/* Imported -----------------------------------------------------------------*/
extern void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim);
                             
                             
/* Variables ----------------------------------------------------------------*/
static uint32_t reset_source;


/* Private defines    --------------------------------------------------------*/
CRC_HandleTypeDef hcrc;
I2C_HandleTypeDef hi2c1;
#ifndef	USE_DEBUGGER
IWDG_HandleTypeDef hiwdg;
#endif
SPI_HandleTypeDef hspi2;
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
RTC_HandleTypeDef hrtc;


/* Private function prototypes -----------------------------------------------*/
static void RestartSource(void);
static void SystemClock_Config(void);
static void RAM_Init(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
#ifndef	USE_DEBUGGER
static void MX_IWDG_Init(void);
#endif
static void MX_RTC_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_CRC_Init(void);


/* Program Code --------------------------------------------------------------*/
int main(void)
{
	RestartSource();
    HAL_Init();
    SystemClock_Config();
#ifndef	USE_DEBUGGER
	MX_IWDG_Init();
#endif
	MX_RTC_Init();
	MX_CRC_Init();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_SPI2_Init();
    MX_TIM1_Init();
    MX_TIM2_Init();
    MX_TIM3_Init();
    HAL_Delay(123); // wait cap sensor 
    LOGGER_Init();
    RAM_Init();
	DIO_Init();
    RC522_Init();
	MX_USART1_UART_Init();
    MX_USART2_UART_Init();
    ONEWIRE_Init();
    RS485_Init();
    DISPLAY_Init();

    while (1)
    {
        DIO_Service();
        RC522_Service();
        DISPLAY_Service();
        SIGNAL_Service();
		ONEWIRE_Service();
#ifndef	USE_DEBUGGER
		HAL_IWDG_Refresh(&hiwdg);
#endif
    }
}


static void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	/**Initializes the CPU, AHB and APB busses clocks
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;

	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) Error_Handler(MAIN_FUNC, RTC_DRIVER);

	/**Initializes the CPU, AHB and APB busses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)Error_Handler(MAIN_FUNC, RTC_DRIVER);

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_ADC;
	PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)Error_Handler(MAIN_FUNC, RTC_DRIVER);

	/**Configure the Systick interrupt time
	*/
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	/**Configure the Systick
	*/
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}


static void MX_CRC_Init(void)
{
	hcrc.Instance = CRC;
	HAL_CRC_Init(&hcrc);
}


static void MX_I2C1_Init(void)
{
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 400000U;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0U;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0U;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if(HAL_I2C_DeInit(&hi2c1) != HAL_OK) Error_Handler(MAIN_FUNC, I2C_DRIVER);
	if(HAL_I2C_Init(&hi2c1) != HAL_OK) Error_Handler(MAIN_FUNC, I2C_DRIVER);
}


#ifndef	USE_DEBUGGER
static void MX_IWDG_Init(void)
{
    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
    hiwdg.Init.Reload = 4095U;
    HAL_IWDG_Init(&hiwdg);
}
#endif

static void MX_RTC_Init(void)
{
    RTC_TimeTypeDef time_set;
    RTC_DateTypeDef date_set;

    hrtc.Instance = RTC;
    hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
    hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
    HAL_RTC_Init(&hrtc);


	if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) == 0xA5A5U)
	{
		date_set.Date = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2);
		date_set.Month = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR3);
		date_set.WeekDay = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR4);
		date_set.Year = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR5);
	}
    else
    {
        time_set.Hours = 0x12U;
        time_set.Minutes = 0U;
        time_set.Seconds = 0U;
        date_set.Date = 0x01U;
        date_set.Month = 0x01U;
        date_set.WeekDay = 0x01U;
        date_set.Year = 0x18U;
        HAL_RTC_SetTime(&hrtc, &time_set, RTC_FORMAT_BCD);
    }

    HAL_RTC_SetDate(&hrtc, &date_set, RTC_FORMAT_BCD);
    HAL_RTC_WaitForSynchro(&hrtc);
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0xA5A5U);
}


static void MX_SPI2_Init(void)
{
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
    hspi2.Init.CRCPolynomial = 10U;
    if(HAL_SPI_Init(&hspi2) != HAL_OK) Error_Handler(MAIN_FUNC, SPI_DRIVER);
}


void MX_TIM1_Init(void)
{
#define REQUIRED_FREQUENCY_1  2770U        // pwm output frequency
#define TIMER_INPUT_CLOCK_1   72000000U    // timer input clock in HZ
#define PWM_RESOLUTION_1      100U          // counter period - number of pwm setps
#define COUNTER_FREQUENCY_1   (REQUIRED_FREQUENCY_1 * PWM_RESOLUTION_1)
#define TIMER_PRESCALER_1     ((TIMER_INPUT_CLOCK_1 / COUNTER_FREQUENCY_1) -1U)
    

    TIM_ClockConfigTypeDef sClockSourceConfig;
    TIM_MasterConfigTypeDef sMasterConfig;
    TIM_OC_InitTypeDef sConfigOC;

    htim1.Instance = TIM1;
    htim1.Init.Prescaler = TIMER_PRESCALER_1;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = PWM_RESOLUTION_1;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0U;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)Error_Handler(MAIN_FUNC, TMR_DRIVER);
    
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK) Error_Handler(MAIN_FUNC, TMR_DRIVER);

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)Error_Handler(MAIN_FUNC, TMR_DRIVER);

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0U;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)Error_Handler(MAIN_FUNC, TMR_DRIVER);


    HAL_TIM_MspPostInit(&htim1);
}


static void MX_TIM2_Init(void)
{
#define REQUIRED_FREQUENCY_2  1000U        // pwm output frequency
#define TIMER_INPUT_CLOCK_2   72000000U    // timer input clock in HZ
#define PWM_RESOLUTION_2      1000U        // counter period - number of pwm setps
#define COUNTER_FREQUENCY_2   (REQUIRED_FREQUENCY_2 * PWM_RESOLUTION_2)
#define TIMER_PRESCALER_2     ((TIMER_INPUT_CLOCK_2 / COUNTER_FREQUENCY_2) -1U)
    
	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_OC_InitTypeDef sConfigOC;

	htim2.Instance = TIM2;
	htim2.Init.Prescaler = TIMER_PRESCALER_2;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = PWM_RESOLUTION_2;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	if (HAL_TIM_PWM_Init(&htim2) != HAL_OK) Error_Handler(MAIN_FUNC, TMR_DRIVER);
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK) Error_Handler(MAIN_FUNC, TMR_DRIVER);
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 100U;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) Error_Handler(MAIN_FUNC, TMR_DRIVER);
    
	HAL_TIM_MspPostInit(&htim2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
}


void MX_TIM3_Init(void)
{
#define REQUIRED_FREQUENCY_3  1000U        // pwm output frequency
#define TIMER_INPUT_CLOCK_3   72000000U    // timer input clock in HZ
#define PWM_RESOLUTION_3      100U        // counter period - number of pwm setps
#define COUNTER_FREQUENCY_3   (REQUIRED_FREQUENCY_3 * PWM_RESOLUTION_3)
#define TIMER_PRESCALER_3     ((TIMER_INPUT_CLOCK_3 / COUNTER_FREQUENCY_3) -1U)
    
	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_OC_InitTypeDef sConfigOC;

	htim3.Instance = TIM3;
	htim3.Init.Prescaler = TIMER_PRESCALER_3;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = PWM_RESOLUTION_3;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	if (HAL_TIM_PWM_Init(&htim3) != HAL_OK) Error_Handler(MAIN_FUNC, TMR_DRIVER);
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK) Error_Handler(MAIN_FUNC, TMR_DRIVER);
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0U;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) Error_Handler(MAIN_FUNC, TMR_DRIVER);
    
	HAL_TIM_MspPostInit(&htim3);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}


static void MX_USART1_UART_Init(void)
{
    huart1.Instance = USART1;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    huart1.Init.BaudRate = baud_rate[rs485_interface_baudrate - '0'];
    if(HAL_UART_Init(&huart1) != HAL_OK) Error_Handler(MAIN_FUNC, USART_DRIVER);
}


static void MX_USART2_UART_Init(void)
{
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 9600U;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.WordLength = UART_WORDLENGTH_9B;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if(HAL_UART_Init(&huart2) != HAL_OK) Error_Handler(MAIN_FUNC, USART_DRIVER);
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
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_7|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_15, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_SET);

	/*Configure GPIO pin : PC13 */
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : PA4 PA5 PA6 PA7 PA8 PA15 */
	GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_7|GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : PA11 PA12 */
	GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : PB0 PB1 PB2 PB3
						   PB4 PB5 PB6 PB7 */
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
						  |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pins : PB10 PB11 PB12 */
	GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}


void HAL_SYSTICK_Callback(void)
{
    if (rs485_timer)
	{
        --rs485_timer;

		if(rs485_timer == 0U)
        {
            RS485_Init();
        }
	}
}


static void RAM_Init(void)
{
    RTC_TimeTypeDef time_m;
    RTC_DateTypeDef date_m;
	uint32_t room_time, rtc_time;
	uint8_t ee_buf[256];

    if(SPI_FLASH_ReleasePowerDown() == 0) Error_Handler(MAIN_FUNC, SPI_DRIVER);

    ee_buf[0] = 0x00U;
    ee_buf[1] = 0x00U;
    if (HAL_I2C_Master_Transmit(&hi2c1, I2C_EE_WRITE, ee_buf, 2, I2C_EE_TIMEOUT) != HAL_OK) Error_Handler(MAIN_FUNC, I2C_DRIVER);
    if (HAL_I2C_Master_Receive(&hi2c1, I2C_EE_READ, ee_buf, 256, I2C_EE_TIMEOUT) != HAL_OK) Error_Handler(MAIN_FUNC, I2C_DRIVER);
    
    ROOM_Status     = (ROOM_StatusTypeDef) ee_buf[EE_ROOM_STATUS_ADDRESS];  // room status address
    fw_update_status            = ee_buf[EE_FW_UPDATE_STATUS];              // firmware update status
    temperature_setpoint        = ee_buf[EE_ROOM_TEMPERATURE_SETPOINT];     // room setpoint temperature in degree of Celsious
    temperature_difference      = ee_buf[EE_ROOM_TEMPERATURE_DIFFERENCE];   // room tempreature on / off difference
    rs485_interface_baudrate    = ee_buf[EE_RS485_BAUDRATE];                // rs485 interface baudrate
    lcd_brightness              = (ee_buf[EE_LCD_BRIGHTNESS] << 8U);        // lcd display backlight LED 
    lcd_brightness              += ee_buf[EE_LCD_BRIGHTNESS + 1U];          // lcd display backlight LED 
    display_status              = ee_buf[EE_DISPLAY_STATUS_ADDRESS];        // display status flags
    buzzer_volume               = ee_buf[EE_BUZZER_VOLUME_ADDRESS];	        // buzzer volume address
    doorlock_force              = ee_buf[EE_DOORLOCK_FORCE_ADDRESS];	    // doorlock force address     
    
    for(uint32_t t = 0U; t < 2U; t++)
    {
        rs485_interface_address[t]  = ee_buf[EE_RS485_INTERFACE_ADDRESS + t];   // rs485 device address
        rs485_group_address[t]      = ee_buf[EE_RS485_GROUP_ADDRESS + t];       // rs485 group broadcast address
        rs485_broadcast_address[t]  = ee_buf[EE_RS485_BROADCAST_ADDRESS + t];   // rs485 broadcast address msb
        system_id[t]                = ee_buf[EE_SYSTEM_ID_ADDRESS + t];         // system id (system unique number)
    }   
    
    for(uint32_t t = 0U; t < 5U; t++)
    {
        aRoomPowerExpiryDateTime[t] = ee_buf[EE_ROOM_POWER_TIMEOUT + t];    // room power expiry date time
    }

    for(uint32_t t = 0U; t < 6U; t++)
    {
        mifare_key_a[t] = ee_buf[EE_MIFARE_KEY_A + t];  // mifare access authentication key A 
        mifare_key_b[t] = ee_buf[EE_MIFARE_KEY_B + t];  // mifare access authentication key B 
    }
 
    for(uint32_t t = 0U; t < 8U; t++)
    {
        permitted_add[t][0] = ee_buf[EE_PERMITTED_ADDRESS_1 + (t * 2U)];        // additional permited address 1
        permitted_add[t][1] = ee_buf[EE_PERMITTED_ADDRESS_1 + 1U + (t * 2U)];   // additional permited address 1
    }    

    for(uint32_t t = 0U; t < 9U; t++)
    {
        thermostat_addresse[t]  = ee_buf[EE_ONEWIRE_ADDRESS_1 + t]; // onewire Lux thermostat address 1 or DS18B20 id 1
    }
    
    
	HAL_RTC_GetTime(&hrtc, &time_m, RTC_FORMAT_BCD);
	HAL_RTC_GetDate(&hrtc, &date_m, RTC_FORMAT_BCD);

	room_time = ((aRoomPowerExpiryDateTime[2] << 24U) +  (aRoomPowerExpiryDateTime[1] << 16U) + (aRoomPowerExpiryDateTime[0] << 8U) + (aRoomPowerExpiryDateTime[3]));
	rtc_time = ((date_m.Year << 24U) + (date_m.Month << 16U) + (date_m.Date << 8) + time_m.Hours);

	if((room_time > rtc_time) || ((room_time == rtc_time) && (aRoomPowerExpiryDateTime[4] >= time_m.Minutes))) SIGNAL_ExpiryTimeFromGuestCardSet();
	else SIGNAL_ExpiryTimeFromGuestCardReset();

	if(!IS_09(rs485_interface_baudrate)) rs485_interface_baudrate = RS485_BAUDRATE_115200;
 
    if (fw_update_status == BOOTLOADER_STATUS_UPDATE_SUCCESS)
    {
        SYS_UpdateSuccessSet();
        DISPLAY_FirmwareUpdated();
        LogEvent.log_event = FIRMWARE_UPDATED;
    }
    else if(fw_update_status == BOOTLOADER_STATUS_UPDATE_FAIL)
    {
        SYS_UpdateFailSet();
        DISPLAY_FirmwareUpdateFail();
        LogEvent.log_event = FIRMWARE_UPDATE_FAIL;
    }

    if (fw_update_status != 0U)
    {
        LOGGER_Write();
        fw_update_status = 0U;
        ee_buf[0] = EE_FW_UPDATE_STATUS >> 8U;
        ee_buf[1] = EE_FW_UPDATE_STATUS & 0xFFU;
        ee_buf[2] = fw_update_status;
        if (HAL_I2C_Master_Transmit(&hi2c1, I2C_EE_WRITE, ee_buf, 3U, I2C_EE_TIMEOUT) != HAL_OK) Error_Handler(MAIN_FUNC, I2C_DRIVER);
        if (HAL_I2C_IsDeviceReady(&hi2c1, I2C_EE_READ, I2C_EE_TRIALS, I2C_EE_WRITE_DELAY) != HAL_OK) Error_Handler(MAIN_FUNC, I2C_DRIVER);
    }

	if(reset_source != 0U)
	{
        LogEvent.log_event = reset_source;  // where  PIN_RESET is ((uint8_t)0xd0)
		LOGGER_Write();
	}
}


void BootloaderExe(void)
{
    HAL_CRC_MspDeInit(&hcrc);
    HAL_I2C_MspDeInit(&hi2c1);
    HAL_RTC_MspDeInit(&hrtc);
    HAL_SPI_MspDeInit(&hspi2);
	HAL_TIM_PWM_MspDeInit(&htim2);
    HAL_UART_MspDeInit(&huart1);
    HAL_UART_MspDeInit(&huart2);
    HAL_DeInit();
    HAL_FLASH_OB_Launch();
}


static void RestartSource(void)
{
         if(__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST))   reset_source = LOW_POWER_RESET;
    else if(__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST))    reset_source = POWER_ON_RESET;
    else if(__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))    reset_source = SOFTWARE_RESET;
    else if(__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))   reset_source = IWDG_RESET;
    else if(__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST))    reset_source = PIN_RESET;
    else if(__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST))   reset_source = WWDG_RESET;
    else                                            reset_source = 0U;
	 __HAL_RCC_CLEAR_RESET_FLAGS();
}


void Error_Handler(uint8_t function, uint8_t driver)
{
    LogEvent.log_event = DRIVER_OR_FUNCTION_FAIL;
    LogEvent.log_group = function;
    LogEvent.log_type = driver;
    LOGGER_Write();
    BootloaderExe();
}


#ifdef USE_FULL_ASSERT

/**
 * @brief Reports the name of the source file and the source line number
 * where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
      ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */

}

#endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
