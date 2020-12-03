/**
 ******************************************************************************
 * File Name          : main.c
 * Date               : 10.3.2018.
 * Description        : Hotel Room Thermostat Program Code
 ******************************************************************************
 *
 *
 ******************************************************************************
 */


/* Includes ------------------------------------------------------------------*/
#include "WM.h"
#include "GUI.h"
#include <math.h>
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
ADC_HandleTypeDef hadc3;
UART_HandleTypeDef huart2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim9;
I2C_HandleTypeDef hi2c4;
I2C_HandleTypeDef hi2c3;
LTDC_HandleTypeDef hltdc;
DMA2D_HandleTypeDef hdma2d;
CRC_HandleTypeDef hcrc;
RTC_HandleTypeDef hrtc;
RTC_TimeTypeDef time;
RTC_DateTypeDef date;
#ifndef	USE_DEBUGGER
IWDG_HandleTypeDef hiwdg;
#endif


/* Private Define ------------------------------------------------------------*/
#define TOUCH_SCREEN_UPDATE_TIME			25U     // 5ms touch screen update period
#define TOUCH_SCREEN_LAYER                  1U      // touch screen layer event
#define AMBIENT_NTC_RREF                    10000U  // 10k NTC value of at 25 degrees
#define AMBIENT_NTC_B_VALUE                 3977U   // NTC beta parameter
#define AMBIENT_NTC_PULLUP                  10000U	// 10k pullup resistor
#define FANCOIL_NTC_RREF                    2000U  	// 2k fancoil NTC value of at 25 degrees
#define FANCOIL_NTC_B_VALUE                 3977U   // NTC beta parameter
#define FANCOIL_NTC_PULLUP                  2200U	// 2k2 pullup resistor
#define ADC_READOUT_PERIOD                  234U    // 89 ms ntc conversion rate
#define FAN_CONTROL_LOOP_PERIOD			    200U    // fan speed control loop 
#define BUZZER_CLICK_TIME                   20U     // single 50 ms tone when button pressed
#define BUZZER_SHORT_TIME                   100U    // 100 ms buzzer activ then repeat pause  
#define BUZZER_MIDDLE_TIME                  500U    // 500 ms buzzer activ then repeat pause
#define BUZZER_LONG_TIME                    1000U   // 1 s buzzer activ then repeat pause
#define TRIAC_ON_PULSE                      5U      // 500 us triac fire pulse duration
#define SYSTEM_STARTUP_TIME                 12345U  // 12,5s system startup config check


/* Private Variable ----------------------------------------------------------*/
__IO uint32_t sys_flags;
__IO uint32_t fan_rpm_pulse;

uint16_t rs485_interface_address;
uint32_t thermostat_flags_memo;
uint8_t buzzer_signal_time;
uint8_t buzzer_signal_id;

static uint32_t triac_timer;
static uint32_t triac_on_time;

/* Constants ----------------------------------------------------------------*/
const char sys_info_hw[] = {"HW: DE020717"};    // hardware version
const char sys_info_fw[] = {"FW: RT200119"};    // firmware version

const uint8_t RTC_Months[2][12] = {
	{31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U},	/* Not leap year */
	{31U, 29U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U}};	/* Leap year */

const uint32_t baud_rate[] ={ 2400U,   4800U,   9600U,  19200U,  38400U,
                             57600U, 115200U, 230400U, 460800U, 921600U};


/* Private Macro -------------------------------------------------------------*/
#define RTC_LEAP_YEAR(year)             ((((year) % 4U == 0U) && ((year) % 100U != 0U)) || ((year) % 400U == 0U))
#define RTC_DAYS_IN_YEAR(x)             RTC_LEAP_YEAR(x) ? 366U : 365U
#define RTC_BCD2BIN(x)                  ((((x) >> 4U) & 0x0FU) * 10U + ((x) & 0x0FU))
#define RTC_CHAR2NUM(x)                 ((x) - '0')
#define RTC_CHARISNUM(x)                ((x) >= '0' && (x) <= '9')


/* Private Function Prototype ------------------------------------------------*/
static void MPU_Config(void);
static void CACHE_Config(void);
static void SystemClock_Config(void);
#ifndef	USE_DEBUGGER
void MX_IWDG_Init(void);
#endif
static void MX_GPIO_Init(void);
static void MX_GPIO_DeInit(void);
static void MX_CRC_Init(void);
static void MX_CRC_DeInit(void);
static void MX_RTC_Init(void);
static void MX_RTC_DeInit(void);
static void MX_TIM3_Init(void);
static void MX_TIM3_DeInit(void);
static void MX_TIM9_Init(void);
static void MX_TIM9_DeInit(void);
static void MX_UART2_Init(void);
static void MX_UART2_DeInit(void);
static void MX_ADC3_Init(void);
static void MX_ADC3_DeInit(void);
static void ADC3_Read(void);
static void RAM_Init(void);
static void BUZZER_Service(void);
static void TOUCH_SCREEN_Service(void);
static void RESTART_Init(void);
static float AMBIENT_NTC_GetTemperature(uint16_t adc_value);
static float FANCOIL_NTC_GetTemperature(uint16_t adc_value);


/* Program Code  -------------------------------------------------------------*/
int main(void)
{
	MPU_Config();
	CACHE_Config();
	HAL_Init(); 
	SystemClock_Config();
#ifndef	USE_DEBUGGER
	MX_IWDG_Init();
#endif	
    MX_CRC_Init();
	MX_RTC_Init();
	MX_ADC3_Init();
	MX_UART2_Init();
	MX_TIM3_Init();
	MX_TIM9_Init();
	MX_GPIO_Init();
	MX_QSPI_Init();
    BSP_EEPROM_Init();
    TOUCH_SCREEN_Init(480, 272);
	SDRAM_Init();
    RAM_Init();
    THERMOSTAT_Init();
	DISPLAY_Init();
	ONEWIRE_Init();
    FAN_SetControlType(fan_control_type);
#ifndef	USE_DEBUGGER
	HAL_IWDG_Refresh(&hiwdg);
#endif

	while(1)
	{
        ADC3_Read();
		THERMOSTAT_Service();
        TOUCH_SCREEN_Service();
		DISPLAY_Service();
        BUZZER_Service();
#ifndef	USE_DEBUGGER
        HAL_IWDG_Refresh(&hiwdg);
#endif
	}
}


void RTC_GetDateTime(RTC_t* data, uint32_t format) 
{
	uint32_t unix;

	/* Get time */
	if (format == RTC_FORMAT_BIN) 
	{
		HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
	} 
	else 
	{
		HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BCD);
	}
	
	/* Format hours */
	data->hours = time.Hours;
	data->minutes = time.Minutes;
	data->seconds = time.Seconds;
	
	/* Get subseconds */
	data->subseconds = RTC->SSR;
	
	/* Get date */
	if (format == RTC_FORMAT_BIN) 
	{
		HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
	} 
	else 
	{
		HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BCD);
	}
	
	/* Format date */
	data->year = date.Year;
	data->month = date.Month;
	data->date = date.Date;
	data->day = date.WeekDay;
	
	/* Calculate unix offset */
	unix = RTC_GetUnixTimeStamp(data);
	data->unix = unix;
}


uint32_t RTC_GetUnixTimeStamp(RTC_t* data) 
{
	uint32_t days = 0U, seconds = 0U;
	uint16_t i;
	uint16_t year = (uint16_t) (data->year + 2000U);
	
	/* Year is below offset year */
	if (year < RTC_OFFSET_YEAR) 
	{
		return 0U;
	}
	
	/* Days in back years */
	for (i = RTC_OFFSET_YEAR; i < year; i++) 
	{
		days += RTC_DAYS_IN_YEAR(i);
	}
	
	/* Days in current year */
	for (i = 1U; i < data->month; i++) 
	{
		days += RTC_Months[RTC_LEAP_YEAR(year)][i - 1U];
	}
	
	/* Day starts with 1 */
	days += data->date - 1U;
	seconds = days * RTC_SECONDS_PER_DAY;
	seconds += data->hours * RTC_SECONDS_PER_HOUR;
	seconds += data->minutes * RTC_SECONDS_PER_MINUTE;
	seconds += data->seconds;
	
	/* seconds = days * 86400; */
	return seconds;
}


void RTC_GetDateTimeFromUnix(RTC_t* data, uint32_t unix) 
{
	uint16_t year;
	
	/* Store unix time to unix in struct */
	data->unix = unix;
	/* Get seconds from unix */
	data->seconds = unix % 60U;
	/* Go to minutes */
	unix /= 60U;
	/* Get minutes */
	data->minutes = unix % 60U;
	/* Go to hours */
	unix /= 60U;
	/* Get hours */
	data->hours = unix % 24U;
	/* Go to days */
	unix /= 24U;
	
	/* Get week day */
	/* Monday is day one */
	data->day = (unix + 3U) % 7U + 1U;

	/* Get year */
	year = 1970U;
	while (1U) 
	{
		if (RTC_LEAP_YEAR(year)) 
		{
			if (unix >= 366U) 
			{
				unix -= 366U;
			} 
			else 
			{
				break;
			}
		} 
		else if (unix >= 365U) 
		{
			unix -= 365U;
		} 
		else 
		{
			break;
		}
		year++;
	}
	
	/* Get year in xx format */
	data->year = (uint8_t) (year - 2000U);
	
	/* Get month */
	for (data->month = 0; data->month < 12U; data->month++) 
	{
		if (RTC_LEAP_YEAR(year)) 
		{
			if (unix >= (uint32_t)RTC_Months[1][data->month]) 
			{
				unix -= RTC_Months[1][data->month];
			}
			else 
			{
				break;
			}
		} 
		else if (unix >= (uint32_t)RTC_Months[0][data->month]) 
		{
			unix -= RTC_Months[0][data->month];
		} 
		else
		{
			break;
		}
	}
	
	/* Get month */
	/* Month starts with 1 */
	data->month++;
	/* Get date */
	/* Date starts with 1 */
	data->date = unix + 1U;
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	__HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_UPDATE);
   
	++triac_timer;
	if(triac_on_time == 0U) HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	else if(triac_timer < triac_on_time) HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
    else if(triac_timer > (triac_on_time + TRIAC_ON_PULSE)) HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	else HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);  
}


void Error_Handler(uint8_t function, uint8_t driver)
{
//    LogEvent.log_event = DRIVER_OR_FUNCTION_FAIL;
//    LogEvent.log_group = function;
//    LogEvent.log_type = driver;
//    LOGGER_Write();	
    RESTART_Init();
}


void FAN_SetControlType(uint8_t fan_ctrl_type)
{
    TIM_OC_InitTypeDef sConfigOC;
	GPIO_InitTypeDef GPIO_InitStruct;
    
    __HAL_RCC_TIM3_CLK_DISABLE();
    HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
    HAL_NVIC_DisableIRQ(TIM3_IRQn);
    HAL_TIM_Base_DeInit(&htim3);
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_8);
    
    if (fan_ctrl_type == 0U)        // simple On / Off thermostat control 
    {
    }
    else if (fan_ctrl_type == 1U)   // 3 speed fan motor connected to 3 triac controled output interface pcb
    {
    }
    else if (fan_ctrl_type == 2U)   // leading edge triac fan speed control with interface pcb 
    {
        GPIO_InitStruct.Pin = GPIO_PIN_8;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
        
        MX_TIM3_Init();
    }
    else if (fan_ctrl_type == 3U)   // BLDC fan motor speed control with interface pcb DE-151218
    {       
        __HAL_RCC_TIM3_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();
        
        htim3.Instance = TIM3;
        htim3.Init.Prescaler = 9999U;
        htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
        htim3.Init.Period = 100U;
        htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	
        if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
        {
            Error_Handler(MAIN_FUNC, TMR_DRIVER);
        }

        sConfigOC.OCMode = TIM_OCMODE_PWM1;
        sConfigOC.Pulse = fan_speed;
        sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
        sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
        
        if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
        {
            Error_Handler(MAIN_FUNC, TMR_DRIVER);
        }

        /**TIM3 GPIO Configuration    
        PC8     ------> TIM3_CH3 
        */
        GPIO_InitStruct.Pin = GPIO_PIN_8;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
        
        HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
    }
}


void FAN_SetSpeed(uint8_t fan_speed)
{
    HAL_TIM_Base_Stop_IT(&htim3);
    HAL_NVIC_DisableIRQ(TIM3_IRQn);
	HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13);
	__HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_UPDATE);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
    
    fan_rpm_pulse = 0U;
	triac_on_time = 0U;
	triac_timer = 0U;
    
	if(fan_speed != FAN_OFF)
	{
		triac_on_time = fan_speed;
		HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	}
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_13)
	{
        HAL_TIM_Base_Stop_IT(&htim3);
        HAL_NVIC_DisableIRQ(TIM3_IRQn);
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
        triac_timer = 0U;
        __HAL_TIM_SET_COUNTER(&htim3, 0U);
        
        if(!IsFANCOIL_RPM_SensorErrorActiv())
        {
            HAL_NVIC_EnableIRQ(TIM3_IRQn);
            HAL_TIM_Base_Start_IT(&htim3);            
        }
        else
        {
            HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
        }
	}
	else if(GPIO_Pin == GPIO_PIN_14)
	{
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_14);
		++fan_rpm_pulse;
	}
}


#ifndef	USE_DEBUGGER
void MX_IWDG_Init(void)
{
    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
    hiwdg.Init.Reload = 4095;
    HAL_IWDG_Init(&hiwdg);
}
#endif


static void RAM_Init(void)
{
    uint16_t bt = 16U;
    uint8_t ee_buff[16];
    
    BSP_EEPROM_ReadBuffer(ee_buff, EE_THERMOSTAT_MAX_COOLING_TEMP, &bt);
    thermostat_max_cooling_temperature = ((ee_buff[0] << 8U) + ee_buff[1]);
    thermostat_min_heating_temperature = ((ee_buff[2] << 8U) + ee_buff[3]);
    thermostat_error_duraton = (ee_buff[4] << 8U) + ee_buff[5];
    thermostat_ambient_ntc_offset = (ee_buff[6] << 8U) + ee_buff[7];
    thermostat_fancoil_ntc_offset = (ee_buff[8] << 8U) + ee_buff[9];
    fancoil_temperature_limit_mode = ee_buff[10];
    thermostat_flags_memo = (ee_buff[11] << 24U) + (ee_buff[12] << 16U) + (ee_buff[13] << 8U) + ee_buff[14];
    
    BSP_EEPROM_ReadBuffer(ee_buff, EE_THERMOSTAT_OPERATION_MODE, &bt);
    thermostat_operation_mode = ee_buff[0];
    thermostat_max_setpoint_temperature  = ee_buff[1];
    thermostat_min_setpoint_temperature  = ee_buff[2];
    fan_speed_treshold  = ee_buff[3];
    fan_low_speed_band  = ee_buff[4];
    fan_middle_speed_band  = ee_buff[5];
    onewire_interface_address  = ee_buff[6];
    rs485_interface_address = (ee_buff[7] << 8U) + ee_buff[8];
    fan_control_type  = ee_buff[9];
    freezing_protection_temperature = ee_buff[10];
    fan_night_quiet_mode_start_hour = ee_buff[11];
    temperature_setpoint = ee_buff[12];
    
    if((thermostat_max_cooling_temperature > 500U) || (thermostat_max_cooling_temperature < 100U)) thermostat_max_cooling_temperature = 500U;
    if((thermostat_min_heating_temperature > 500U) || (thermostat_min_heating_temperature < 100U)) thermostat_min_heating_temperature = 100U;
    if((thermostat_error_duraton > 240U) || (thermostat_error_duraton < 1U)) thermostat_error_duraton = 1U;
    if((thermostat_ambient_ntc_offset > 100) || (thermostat_ambient_ntc_offset < -100) )thermostat_ambient_ntc_offset = 0;
    if((thermostat_fancoil_ntc_offset > 100) || (thermostat_fancoil_ntc_offset < -100) )thermostat_fancoil_ntc_offset = 0;
    if(fancoil_temperature_limit_mode > 1U) fancoil_temperature_limit_mode = 0U;
    if(thermostat_operation_mode > 3U) thermostat_operation_mode = 3U;
    if((thermostat_max_setpoint_temperature > 40U) || (thermostat_max_setpoint_temperature < 10U)) thermostat_max_setpoint_temperature = 40U;
    if((thermostat_min_setpoint_temperature > 40U) || (thermostat_min_setpoint_temperature < 10U)) thermostat_min_setpoint_temperature = 15U;
    if(fan_speed_treshold > 100U) fan_speed_treshold = 100U;
    if(fan_low_speed_band > 100U) fan_low_speed_band = 100U;
    if(fan_middle_speed_band > 100U) fan_middle_speed_band = 100U;
    if((onewire_interface_address > 9U) || (onewire_interface_address < 1U)) onewire_interface_address = 1U;
    if((rs485_interface_address > 65000U) || (rs485_interface_address < 100U)) rs485_interface_address = 100U;
    if(fan_control_type > 2U) fan_control_type = 0U;
    if((freezing_protection_temperature > 80U) || (freezing_protection_temperature < 5U)) freezing_protection_temperature = 80U;
    if(fan_night_quiet_mode_start_hour > 23U) fan_night_quiet_mode_start_hour = 7U;
    if((temperature_setpoint & 0x3FU) > thermostat_max_setpoint_temperature) temperature_setpoint = (temperature_setpoint & 0xC0U) + thermostat_max_setpoint_temperature;
    if((temperature_setpoint & 0x3FU) < thermostat_min_setpoint_temperature) temperature_setpoint = (temperature_setpoint & 0xC0U) + thermostat_min_setpoint_temperature;
}

static void MPU_Config(void)
{
	MPU_Region_InitTypeDef MPU_InitStruct;

	/* Disable the MPU */
	HAL_MPU_Disable();

	/* Configure the MPU attributes as WT for SRAM */
	MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
	MPU_InitStruct.BaseAddress      = 0x20010000U;
	MPU_InitStruct.Size             = MPU_REGION_SIZE_256KB;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
	MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
	MPU_InitStruct.IsShareable      = MPU_ACCESS_SHAREABLE;
	MPU_InitStruct.Number           = MPU_REGION_NUMBER0;
	MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
	MPU_InitStruct.SubRegionDisable = 0U;
	MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	/* Configure the MPU attributes for Quad-SPI area to strongly ordered
	 This setting is essentially needed to avoid MCU blockings! 
	 See also STM Application Note AN4861 */
	MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
	MPU_InitStruct.Number           = MPU_REGION_NUMBER2;
	MPU_InitStruct.BaseAddress      = 0x90000000U;
	MPU_InitStruct.Size             = MPU_REGION_SIZE_256MB;
	MPU_InitStruct.SubRegionDisable = 0U;
	MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
	MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
	MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE;
	MPU_InitStruct.IsShareable      = MPU_ACCESS_SHAREABLE;
	MPU_InitStruct.IsCacheable      = MPU_ACCESS_NOT_CACHEABLE;
	MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	/* Configure the MPU attributes for the QSPI 64MB to normal memory Cacheable, must reflect the real memory size */
	MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
	MPU_InitStruct.Number           = MPU_REGION_NUMBER3;
	MPU_InitStruct.BaseAddress      = 0x90000000U;
	MPU_InitStruct.Size             = MPU_REGION_SIZE_16MB; // Set region size according to the QSPI memory size
	MPU_InitStruct.SubRegionDisable = 0U;
	MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE;
	MPU_InitStruct.IsShareable      = MPU_ACCESS_SHAREABLE;
	MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
	MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	/* Configure the MPU attributes for SDRAM_Banks area to strongly ordered
	 This setting is essentially needed to avoid MCU blockings!
	 See also STM Application Note AN4861 */
	MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
	MPU_InitStruct.Number           = MPU_REGION_NUMBER4;
	MPU_InitStruct.BaseAddress      = 0xC0000000U;
	MPU_InitStruct.Size             = MPU_REGION_SIZE_512MB;
	MPU_InitStruct.SubRegionDisable = 0U;
	MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
	MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
	MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE;
	MPU_InitStruct.IsShareable      = MPU_ACCESS_SHAREABLE;
	MPU_InitStruct.IsCacheable      = MPU_ACCESS_NOT_CACHEABLE;
	MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	/* Configure the MPU attributes for SDRAM 16MB to normal memory Cacheable */
	MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
	MPU_InitStruct.Number           = MPU_REGION_NUMBER5;
	MPU_InitStruct.BaseAddress      = 0xC0000000U;
	MPU_InitStruct.Size             = MPU_REGION_SIZE_8MB;
	MPU_InitStruct.SubRegionDisable = 0U;
	MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
	MPU_InitStruct.IsShareable      = MPU_ACCESS_SHAREABLE;
	MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
	MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	/* Enable the MPU */
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
	
	/* Disable FMC bank1 (0x6000 0000 - 0x6FFF FFFF), since it is not used.
	This setting avoids unnedded speculative access to the first FMC bank.
	See also STM Application Note AN4861 */
	FMC_Bank1->BTCR[0] = 0x000030D2U;
}

static void CACHE_Config(void)
{
	SCB_EnableICache();
	SCB_EnableDCache();	
}


static void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

	/**Configure the main internal regulator output voltage 
	*/
	__HAL_RCC_PWR_CLK_ENABLE();

	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/**Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 4U;
	RCC_OscInitStruct.PLL.PLLN = 200U;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 2U;
	
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler(MAIN_FUNC, SYS_CLOCK);
	}

	/**Activate the Over-Drive mode 
	*/
	if (HAL_PWREx_EnableOverDrive() != HAL_OK)
	{
		Error_Handler(MAIN_FUNC, SYS_CLOCK);
	}

	/**Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK)
	{
		Error_Handler(MAIN_FUNC, SYS_CLOCK);
	}

	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC|RCC_PERIPHCLK_RTC
							  |RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
							  |RCC_PERIPHCLK_SAI2|RCC_PERIPHCLK_I2C3
							  |RCC_PERIPHCLK_I2C4;
	PeriphClkInitStruct.PLLSAI.PLLSAIN = 57U;
	PeriphClkInitStruct.PLLSAI.PLLSAIR = 3U;
	PeriphClkInitStruct.PLLSAI.PLLSAIQ = 2U;
	PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV2;
	PeriphClkInitStruct.PLLSAIDivQ = 1U;
	PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_4;
	PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	PeriphClkInitStruct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLSAI;
	PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
	PeriphClkInitStruct.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
	PeriphClkInitStruct.I2c3ClockSelection = RCC_I2C3CLKSOURCE_PCLK1;
	PeriphClkInitStruct.I2c4ClockSelection = RCC_I2C4CLKSOURCE_PCLK1;
	
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
		Error_Handler(MAIN_FUNC, SYS_CLOCK);
	}

	/**Configure the Systick interrupt time 
	*/
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000U);

	/**Configure the Systick 
	*/
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}


void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
	RCC_OscInitTypeDef        RCC_OscInitStruct;
	RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

	RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
	
	if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{ 
		return;
	}

	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	
	if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{ 
		return;
	}

	__HAL_RCC_RTC_ENABLE(); 
}


void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
	__HAL_RCC_RTC_DISABLE();
}


static void MX_RTC_Init(void)
{
	hrtc.Instance = RTC; 
	hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
	hrtc.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
	hrtc.Init.SynchPrediv = RTC_SYNCH_PREDIV;
	hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

	if (HAL_RTC_Init(&hrtc) != HAL_OK) Error_Handler(MAIN_FUNC, RTC_DRIVER);

    time.Hours = 0x12U;
    time.Minutes = 0x00U;
    time.Seconds = 0x00U;
    date.Date = 0x01U;
    date.Month = 0x01U;
    date.WeekDay = 0x01U;
    date.Year = 0x18U;
    HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BCD);
    HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BCD);
    HAL_RTC_WaitForSynchro(&hrtc);
}

static void MX_RTC_DeInit(void)
{
	HAL_RTC_DeInit(&hrtc);
}


static void MX_TIM3_Init(void)
{
	/**
	*	SYSCLK=200MHz => AHB prescaler/1 => HCLK=200MHz =>APB1 prescaler/4 => x2 => APB1 timer clock=100MHz
	*/
    __HAL_RCC_TIM3_CLK_ENABLE();
    
	htim3.Instance = TIM3;
	htim3.Init.Period = 9999U;		// 100us 
	htim3.Init.Prescaler = 0U;			
	htim3.Init.ClockDivision = 0U;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;

	if(HAL_TIM_Base_Init(&htim3) != HAL_OK)
	{
		Error_Handler(MAIN_FUNC, TMR_DRIVER);
	}
    
    HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
}


static void MX_TIM3_DeInit(void)
{
    __HAL_RCC_TIM3_CLK_DISABLE();
	HAL_NVIC_DisableIRQ(TIM3_IRQn);
	HAL_TIM_Base_DeInit(&htim3);
}


static void MX_TIM9_Init(void)
{
	TIM_OC_InitTypeDef sConfigOC;
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_TIM9_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	htim9.Instance = TIM9;
	htim9.Init.Prescaler = 200U;
	htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim9.Init.Period = 1000U;
	htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim9.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	
	if (HAL_TIM_PWM_Init(&htim9) != HAL_OK)
	{
		Error_Handler(MAIN_FUNC, TMR_DRIVER);
	}

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = DISPLAY_BRIGHTNESS_HIGH;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	
	if (HAL_TIM_PWM_ConfigChannel(&htim9, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler(MAIN_FUNC, TMR_DRIVER);
	}

	/**TIM9 GPIO Configuration    
    PE5     ------> TIM9_CH1 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF3_TIM9;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_1);
}


static void MX_TIM9_DeInit(void)
{
	__HAL_RCC_TIM9_CLK_DISABLE();
	HAL_GPIO_DeInit(GPIOE, GPIO_PIN_5);
	HAL_TIM_PWM_DeInit(&htim9);
}


static void MX_UART2_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_USART2_CLK_ENABLE();

	
	/**USART2 GPIO Configuration    
	PD5     ------> USART2_TX
	PD6     ------> USART2_RX 
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(USART2_IRQn, 3, 0);
	HAL_NVIC_EnableIRQ(USART2_IRQn);

	huart2.Instance = USART2;
	huart2.Init.BaudRate = 9600U;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	
    if (HAL_UART_DeInit(&huart2) != HAL_OK) Error_Handler(MAIN_FUNC, USART_DRIVER);
	if (HAL_UART_Init(&huart2) != HAL_OK) Error_Handler(MAIN_FUNC, USART_DRIVER);
}


static void MX_UART2_DeInit(void)
{
	__HAL_RCC_USART2_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_5|GPIO_PIN_6);
    HAL_NVIC_DisableIRQ(USART2_IRQn);
    HAL_UART_DeInit(&huart2);
}


static void MX_CRC_Init(void)
{
    hcrc.Instance = CRC;
    __HAL_RCC_CRC_CLK_ENABLE();
    
	if (HAL_CRC_Init(&hcrc) != HAL_OK)
	{
		Error_Handler(MAIN_FUNC, CRC_DRIVER);
	}
}


static void MX_CRC_DeInit(void)
{
    __HAL_RCC_CRC_CLK_DISABLE();
	HAL_CRC_DeInit(&hcrc);
}


static void ADC3_Read(void)
{
    ADC_ChannelConfTypeDef sConfig;
    static uint32_t sys_startup = 0U;
    static uint32_t adc_timer = 0U;
    static uint32_t ambient_ntc_sample_cnt = 0U;
    static uint32_t fancoil_ntc_sample_cnt = 0U;
    static uint32_t analog_din_sample_cnt = 0U;
    static uint16_t ambient_ntc_sample_value[10] = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U};
    static uint16_t fancoil_ntc_sample_value[10] = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U};
    static uint16_t analog_din_sample_value[10]  = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U};
	static uint8_t adc_cnt = 0U;
    static float adc_calc;
    uint16_t analog_din;
    
    if(sys_startup == 0U) sys_startup = HAL_GetTick();
    else if (IsSYSTEM_StartupActiv())
    {
        if ((HAL_GetTick() - sys_startup) >= SYSTEM_STARTUP_TIME)
        {
            SYSTEM_StartupReset();
        }
    }
   
    if((HAL_GetTick() - adc_timer) < ADC_READOUT_PERIOD) return;
    else adc_timer = HAL_GetTick();
    
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
    if(adc_cnt == 0U) sConfig.Channel = ADC_CHANNEL_11;
    else if(adc_cnt == 1U) sConfig.Channel = ADC_CHANNEL_12;
    else if(adc_cnt == 2U) sConfig.Channel = ADC_CHANNEL_13;
    HAL_ADC_ConfigChannel(&hadc3, &sConfig);
    HAL_ADC_Start(&hadc3);
    HAL_ADC_PollForConversion(&hadc3, 10);
    
    if(adc_cnt == 0U)
    {
        ambient_ntc_sample_value[ambient_ntc_sample_cnt] = HAL_ADC_GetValue(&hadc3);
        if(++ambient_ntc_sample_cnt >  9U) ambient_ntc_sample_cnt = 0U;
        ambient_ntc_temperature = 0U;
        for(uint8_t t = 0U; t < 10U; t++) ambient_ntc_temperature += ambient_ntc_sample_value[t];
        ambient_ntc_temperature = ambient_ntc_temperature / 10U;
        
        if((ambient_ntc_temperature < 100U) || (ambient_ntc_temperature > 4000U)) 
        {
            if(IsAMBIENT_NTC_SensorConnected() && !IsSYSTEM_StartupActiv()) AMBIENT_NTC_SensorErrorSet();
            AMBIENT_NTC_SensorNotConnected();
        }
        else 
        {
            AMBIENT_NTC_SensorConnected();
            adc_calc = AMBIENT_NTC_GetTemperature(ambient_ntc_temperature);
            
            if(adc_calc < 0) 
            {
                adc_calc *= -1;
                ambient_ntc_temperature = (uint16_t) (adc_calc * 10U);
                ambient_ntc_temperature |= (1U << 15);
            }
            else ambient_ntc_temperature = (uint16_t) (adc_calc * 10U);
        }
        
        ++adc_cnt;
    }
    else if(adc_cnt == 1U)
    {
        fancoil_ntc_sample_value[fancoil_ntc_sample_cnt] = HAL_ADC_GetValue(&hadc3);
        if(++fancoil_ntc_sample_cnt >  9U) fancoil_ntc_sample_cnt = 0U;
        fancoil_ntc_temperature = 0U;
        for(uint8_t t = 0U; t < 10U; t++) fancoil_ntc_temperature += fancoil_ntc_sample_value[t];
        fancoil_ntc_temperature = fancoil_ntc_temperature / 10U;
        
        if((fancoil_ntc_temperature < 100U) || (fancoil_ntc_temperature > 4000U)) 
        {
            if(IsFANCOIL_NTC_SensorConnected() && !IsSYSTEM_StartupActiv()) FANCOIL_NTC_SensorErrorSet();
            FANCOIL_NTC_SensorNotConnected();
        }
        else 
        {
            FANCOIL_NTC_SensorConnected();
            adc_calc = FANCOIL_NTC_GetTemperature(fancoil_ntc_temperature);
            
            if(adc_calc < 0) 
            {
                adc_calc *= -1;
                fancoil_ntc_temperature = (uint16_t) (adc_calc * 10U);
                fancoil_ntc_temperature |= (1U << 15);
            }
            else fancoil_ntc_temperature = (uint16_t) (adc_calc * 10U);
        }
        
        ++adc_cnt;
    }
    else if(adc_cnt == 2U)
    {
        analog_din_sample_value[analog_din_sample_cnt] = HAL_ADC_GetValue(&hadc3);
        
        if(++analog_din_sample_cnt >  9U) 
        {
            analog_din_sample_cnt = 0U;
            analog_din = 0U;
            for(uint8_t t = 0U; t < 10U; t++) analog_din += analog_din_sample_value[t];
            analog_din = analog_din / 10U;
            if((analog_din < 10U) && !IsSYSTEM_StartupActiv()) THERMOSTAT_WindowSwitchClosed();
            else THERMOSTAT_WindowSwitchOpen();
        }
        
        adc_cnt = 0U;
    }
}


static void MX_ADC3_Init(void)
{
	ADC_ChannelConfTypeDef sConfig;
	GPIO_InitTypeDef GPIO_InitStruct;

	__HAL_RCC_ADC3_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	hadc3.Instance = ADC3;
	hadc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
	hadc3.Init.Resolution = ADC_RESOLUTION_12B;
	hadc3.Init.ScanConvMode = DISABLE;
	hadc3.Init.ContinuousConvMode = DISABLE;
	hadc3.Init.DiscontinuousConvMode = DISABLE;
	hadc3.Init.NbrOfDiscConversion = 0U;
	hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc3.Init.NbrOfConversion = 3U;
	hadc3.Init.DMAContinuousRequests = DISABLE;
	hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;

	if(HAL_ADC_Init(&hadc3) != HAL_OK)
	{
		Error_Handler(MAIN_FUNC, ADC_DRIVER);
	}
	
	sConfig.Channel = ADC_CHANNEL_11;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
	sConfig.Offset = 0U;
	HAL_ADC_ConfigChannel(&hadc3, &sConfig);
}


static void MX_ADC3_DeInit(void)
{
	__HAL_RCC_ADC3_CLK_DISABLE();
	HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	HAL_ADC_DeInit(&hadc3);
}


static void MX_I2C4_DeInit(void)
{
    __HAL_RCC_I2C4_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_12 | GPIO_PIN_13);
    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_2);
    HAL_I2C_DeInit(&hi2c4);
}

static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
	
	/*Configure GPIO pin : FAN_SPEED_SENSOR_Pin and FANCOIL_MAINS_ZERO_CROSS*/
	GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	
	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}


static void MX_GPIO_DeInit(void)
{
	HAL_GPIO_DeInit(GPIOC, GPIO_PIN_8);
	HAL_GPIO_DeInit(GPIOD, GPIO_PIN_7);
    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_13|GPIO_PIN_14);
	HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
}


static void TOUCH_SCREEN_Service(void)
{
    static GUI_PID_STATE TS_State = {0, 0, 0, 0};
    static uint32_t ts_update_timer = 0U;
    __IO TS_StateTypeDef  ts;
    uint16_t xDiff, yDiff; 
    
    if(HAL_GetTick() < ts_update_timer) return;
    else (ts_update_timer = HAL_GetTick() + TOUCH_SCREEN_UPDATE_TIME);
    
	BSP_TS_GetState((TS_StateTypeDef *)&ts);

	if((ts.touchX[0] >= LCD_GetXSize()) ||
       (ts.touchY[0] >= LCD_GetYSize())) 
	{
		ts.touchX[0] = 0U;
		ts.touchY[0] = 0U;
		ts.touchDetected = 0U;
        ts.touchWeight[0] = 0U; 
        ts.touchEventId[0] = 0U; 
        ts.touchArea[0] = 0U;   
        ts.gestureId = 0U;
	}

	xDiff = (TS_State.x > ts.touchX[0]) ? (TS_State.x - ts.touchX[0]) : (ts.touchX[0] - TS_State.x);
	yDiff = (TS_State.y > ts.touchY[0]) ? (TS_State.y - ts.touchY[0]) : (ts.touchY[0] - TS_State.y);


	if((TS_State.Pressed != ts.touchDetected ) || (xDiff > 30U) || (yDiff > 30U))
	{
		TS_State.Pressed = ts.touchDetected;
		TS_State.Layer = TOUCH_SCREEN_LAYER;
		
		if(ts.touchDetected) 
		{
			TS_State.x = ts.touchX[0];
			TS_State.y = ts.touchY[0];
			GUI_TOUCH_StoreStateEx(&TS_State);
		}
		else
		{
			GUI_TOUCH_StoreStateEx(&TS_State);
			TS_State.x = 0;
			TS_State.y = 0;
		}
	}
}

static void BUZZER_Service(void)
{
    static uint32_t buzzer_repeat_timer = 0U;
    static uint32_t buzzer_mode_timer = 0U;
    static uint32_t buzzer_pcnt = 0U;
    
    if(IsBUZZER_SignalActiv())
	{
        switch (buzzer_signal_id)
        {
            case BUZZER_OFF:
            {
                buzzer_pcnt = 0U;
                BUZZER_SignalOff();
                break;
            }
            
            case BUZZER_SHORT:
            {
                if(HAL_GetTick() > buzzer_mode_timer)
                {
                    if(buzzer_pcnt == 0U)
                    {
                        if(HAL_GetTick() > buzzer_repeat_timer)
                        {
                            BUZZER_On();
                            buzzer_mode_timer = HAL_GetTick() + BUZZER_SHORT_TIME;
                            ++buzzer_pcnt;                        
                        }
                    }
                    else if(buzzer_pcnt == 1U)
                    {
                        BUZZER_Off();
                        buzzer_pcnt = 0U;
                        if(buzzer_signal_time == 0U) buzzer_mode_timer = HAL_GetTick() + BUZZER_SHORT_TIME;
                        else buzzer_repeat_timer = (HAL_GetTick() + (buzzer_signal_time * 1000U));
                    }   
                }
                break;
            }
            
            case BUZZER_MIDDLE:
            {
                if(HAL_GetTick() > buzzer_mode_timer)
                {
                    if(buzzer_pcnt == 0U)
                    {
                        if(HAL_GetTick() > buzzer_repeat_timer)
                        {
                            BUZZER_On();
                            buzzer_mode_timer = HAL_GetTick() + BUZZER_MIDDLE_TIME;
                            ++buzzer_pcnt;                        
                        }
                    }
                    else if(buzzer_pcnt == 1U)
                    {
                        BUZZER_Off();
                        buzzer_pcnt = 0U;
                        if(buzzer_signal_time == 0U) buzzer_mode_timer = HAL_GetTick() + BUZZER_MIDDLE_TIME;
                        else buzzer_repeat_timer = (HAL_GetTick() + (buzzer_signal_time * 1000U));
                    }                    
                }
                break;
            }
            
            case BUZZER_LONG:
            {
                if(HAL_GetTick() > buzzer_mode_timer)
                {
                    if((buzzer_pcnt == 0U))
                    {
                        if(HAL_GetTick() > buzzer_repeat_timer)
                        {
                            BUZZER_On();
                            ++buzzer_pcnt;
                            buzzer_mode_timer = HAL_GetTick() + BUZZER_LONG_TIME;                
                        }
                    }
                    else if(buzzer_pcnt == 1U)
                    {
                        BUZZER_Off();
                        buzzer_pcnt = 0U;
                        if(buzzer_signal_time == 0U) buzzer_mode_timer = HAL_GetTick() + BUZZER_LONG_TIME;
                        else buzzer_repeat_timer = (HAL_GetTick() + (buzzer_signal_time * 1000U));
                    }
                }
                break;
            }
            
            case BUZZER_CLICK:
            {
                if(buzzer_pcnt == 0U)
                {
                    BUZZER_On();
                    buzzer_mode_timer = HAL_GetTick() + BUZZER_CLICK_TIME;
                    ++buzzer_pcnt;
                }
                else if(buzzer_pcnt == 1U)
                {
                    if(HAL_GetTick() > buzzer_mode_timer)
                    {
                        BUZZER_Off();
                        buzzer_signal_id = BUZZER_OFF;                       
                    }
                }
                break;
            }
            
            default:
            {
                BUZZER_SignalOff();
                break;
            }
        }
	}
    else
    {
        buzzer_repeat_timer = 0U;
        buzzer_mode_timer = 0U;
        BUZZER_SignalOff();
        buzzer_pcnt = 0U;
        BUZZER_Off();
    }
}


static void RESTART_Init(void)
{
    MX_RTC_DeInit();
    MX_CRC_DeInit();
    MX_ADC3_DeInit();
    MX_I2C4_DeInit();
    MX_TIM3_DeInit();
    MX_TIM9_DeInit();
    MX_UART2_DeInit();
    MX_GPIO_DeInit();
    HAL_DeInit();
    HAL_FLASH_OB_Launch();
}


static float AMBIENT_NTC_GetTemperature(uint16_t adc_value)
{
	float temperature;
    float ntc_resistance;
//	ntc_resistance = (float) (AMBIENT_NTC_PULLUP * adc_value / (4095.0  - adc_value));
	ntc_resistance = (float) (AMBIENT_NTC_PULLUP * ((4095.0 / (4095 - adc_value)) - 1));
//	temperature = (1.0 / ((log (ntc_resistance / AMBIENT_NTC_RREF)) / ambient_ntc_b_value  + 1.0 / 298.0)) - 273.0; 
	temperature = ((AMBIENT_NTC_B_VALUE * 298.1) /  (AMBIENT_NTC_B_VALUE + (298.1 * log(ntc_resistance / AMBIENT_NTC_RREF))) -273.1);
    return(temperature);
}


static float FANCOIL_NTC_GetTemperature(uint16_t adc_value)
{
	float temperature;
    float ntc_resistance;
//	ntc_resistance = (float) (FANCOIL_NTC_PULLUP * adc_value / (4095.0  - adc_value));
    ntc_resistance = (float) (FANCOIL_NTC_PULLUP * ((4095.0 / (4095 - adc_value)) - 1));
//	temperature = (1.0 / ((log (ntc_resistance / FANCOIL_NTC_RREF)) / FANCOIL_NTC_B_VALUE  + 1.0 / 298.0)) - 273.0;
    temperature = ((FANCOIL_NTC_B_VALUE * 298.1) /  (FANCOIL_NTC_B_VALUE + (298.1 * log(ntc_resistance / FANCOIL_NTC_RREF))) -273.1);
	return(temperature);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
