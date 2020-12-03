/**
 ******************************************************************************
 * File Name          : main.c
 * Date               : 10.3.2018.
 * Description        : Hotel Room Thermostat Program Header
 ******************************************************************************
 *
 *
 ******************************************************************************
 */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__                            RT200119 // firmware version

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx.h"

/* Code switch define --------------------------------------------------------*/
//#define USE_FULL_ASSERT	1U	// use for full assert and debug of hal parameters 
#define USE_DEBUGGER		2U	// if using serial wire debugger remap PA13 & PA14 to swdio & swclk 
//#define WRITE_DEFAULT		3U	// used for firs time flash and eeprom initialization
//#define DEMO_MODE			4U	// used for room controller demo presentation


#define I2C_EE_DENSITY 						    16384U      	// available memory bytes
#define I2C_EE_NUMBER_OF_PAGE				    256U			// number of pages
#define I2C_EE_PAGE_BYTE_SIZE   			    64U         	// number of bytes per page
#define I2C_EE_END_ADDRESS					    0x3FFFU
#define I2C_EE_READ    						    0xA1U
#define I2C_EE_WRITE   						    0xA0U
#define I2C_EE_TIMEOUT						    5U
#define I2C_EE_WRITE_DELAY					    15U
#define I2C_EE_TRIALS						    100U
                    

#define EE_THERMOSTAT_MAX_COOLING_TEMP          ((uint16_t)0x0000U)
#define EE_THERMOSTAT_MIN_HEATING_TEMP          ((uint16_t)0x0002U)
#define EE_THERMOSTAT_ERROR_DURATION            ((uint16_t)0x0004U)
#define EE_THERMOSTAT_AMBIENT_NTC_OFFSET        ((uint16_t)0x0006U)
#define EE_THERMOSTAT_FANCOIL_NTC_OFFSET        ((uint16_t)0x0008U)
#define EE_FANCOIL_TEMPERATURE_LIMIT_MODE       ((uint16_t)0x000aU)
#define EE_THERMOSTAT_FLAGS                     ((uint16_t)0x000bU)
#define EE_THERMOSTAT_OPERATION_MODE            ((uint16_t)0x0010U)
#define EE_THERMOSTAT_MAX_SETPOINT              ((uint16_t)0x0011U)
#define EE_THERMOSTAT_MIN_SETPOINT              ((uint16_t)0x0012U)
#define EE_FANCOIL_FAN_SPEED_TRESHOLD           ((uint16_t)0x0013U)
#define EE_FANCOIL_FAN_LOW_SPEED_BAND           ((uint16_t)0x0014U)
#define EE_FANCOIL_FAN_MIDDLE_SPEED_BAND        ((uint16_t)0x0015U)
#define EE_ONEWIRE_INTERFACE_ADDRESS            ((uint16_t)0x0016U)
#define EE_RS485_INTERFACE_ADDRESS              ((uint16_t)0x0017U)
#define EE_FANCOIL_FAN_CONTROL_TYPE             ((uint16_t)0x0019U)
#define EE_FREEZING_PROTECTION_TEMPERATURE      ((uint16_t)0x001aU)
#define EE_NIGHT_QUIET_FAN_MODE_START_HOUR      ((uint16_t)0x001bU)
#define EE_THERMOSTAT_SET_POINT                 ((uint16_t)0x001cU)


/* Exported types ------------------------------------------------------------*/
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim9;
extern I2C_HandleTypeDef hi2c4;
extern I2C_HandleTypeDef hi2c3;
extern RTC_HandleTypeDef hrtc;
extern RTC_TimeTypeDef time;
extern RTC_DateTypeDef date;
extern LTDC_HandleTypeDef  hltdc;
extern DMA2D_HandleTypeDef hdma2d;
#ifndef	USE_DEBUGGER
extern IWDG_HandleTypeDef hiwdg;
#endif

typedef struct 
{
	uint8_t seconds;     	/*!< Seconds parameter, from 00 to 59 */
	uint16_t subseconds; 	/*!< Subsecond downcounter. When it reaches zero, it's reload value is the same as */
	uint8_t minutes;     	/*!< Minutes parameter, from 00 to 59 */
	uint8_t hours;       	/*!< Hours parameter, 24Hour mode, 00 to 23 */
	uint8_t day;         	/*!< Day in a week, from 1 to 7 */
	uint8_t date;        	/*!< Date in a month, 1 to 31 */
	uint8_t month;       	/*!< Month in a year, 1 to 12 */
	uint8_t year;        	/*!< Year parameter, 00 to 99, 00 is 2000 and 99 is 2099 */
	uint32_t unix;       	/*!< Seconds from 01.01.1970 00:00:00 */
	
} RTC_t;


/* Exported constants --------------------------------------------------------*/
#define RTC_STATUS_REG                  RTC_BKP_DR19 /* Status Register */
#define RTC_STATUS_INIT_OK              0x1234U       /* RTC initialised */
#define RTC_STATUS_TIME_OK              0x4321U       /* RTC time OK */
#define RTC_STATUS_ZERO                 0x0000U
#define RTC_OFFSET_YEAR                 1970U
#define RTC_SECONDS_PER_DAY             86400U
#define RTC_SECONDS_PER_HOUR            3600U
#define RTC_SECONDS_PER_MINUTE          60U

#define BUZZER_OFF                      0U
#define BUZZER_CLICK                    1U
#define BUZZER_SHORT                    2U
#define BUZZER_MIDDLE                   3U
#define BUZZER_LONG                     4U


    
/* Defines related to Clock configuration */
#define RTC_ASYNCH_PREDIV  0x7FU   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FFU /* LSE as RTC clock */

extern __IO uint32_t sys_flags;
//extern __IO uint32_t fan_rpm_timer;
extern __IO uint32_t fan_rpm_pulse;

extern uint16_t rs485_interface_address;
extern uint32_t thermostat_flags_memo;
extern uint8_t buzzer_signal_time;
extern uint8_t buzzer_signal_id;


//#define FANCOIL_RPM_StartTimer(TIME)            (fan_rpm_timer = TIME)
//#define FANCOIL_RPM_StopTimer()                 (fan_rpm_timer = 0)
//#define IsFANCOIL_RPM_TimerExpired()            (fan_rpm_timer == 0)

#define BUZZER_SignalOn()                       (sys_flags |= (1U << 0))
#define BUZZER_SignalOff()                      (sys_flags &= (~ (1U << 0)))
#define IsBUZZER_SignalActiv()                  ((sys_flags & (1U << 0)) != 0U)

#define FANCOIL_RPM_SensorConnected()           (sys_flags |= (1U << 1)) 
#define FANCOIL_RPM_SensorNotConnected()        (sys_flags &= (~ (1U << 1)))
#define IsFANCOIL_RPM_SensorConnected()         ((sys_flags & (1U << 1)) != 0U)

#define FANCOIL_RPM_SensorErrorSet()            (sys_flags |= (1U << 2)) 
#define FANCOIL_RPM_SensorErrorReset()          (sys_flags &= (~ (1U << 2)))
#define IsFANCOIL_RPM_SensorErrorActiv()        ((sys_flags & (1U << 2)) != 0U)

#define SYSTEM_StartupReset()                   (sys_flags |= (1U << 3)) 
#define SYSTEM_StartupSet()                     (sys_flags &= (~ (1U << 3)))
#define IsSYSTEM_StartupActiv()                 ((sys_flags & (1U << 3)) == 0U)

#define TOUCH_SCREEN_Enable()                   (sys_flags |= (1U << 4)) 
#define TOUCH_SCREEN_Disable()                  (sys_flags &= (~ (1U << 4)))
#define IsTOUCH_SCREEN_Enabled()                ((sys_flags & (1U << 4)) != 0U)

#define THERMOSTAT_WindowSwitchClosed()         (sys_flags |= (1U << 5)) 
#define THERMOSTAT_WindowSwitchOpen()           (sys_flags &= (~ (1U << 5)))
#define IsTHERMOSTAT_WindowSwitchClosed()       ((sys_flags & (1U << 5)) != 0U)

#define THERMOSTAT_WindowSwitchActivSet()       (sys_flags |= (1U << 6)) 
#define THERMOSTAT_WindowSwitchActivReset()     (sys_flags &= (~ (1U << 6)))
#define IsTHERMOSTAT_WindowSwitchActiv()        ((sys_flags & (1U << 6)) != 0U)


#define BUZZER_On()             (HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET))
#define BUZZER_Off()            (HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET))
#define IsBUZZER_On()           (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_4) == GPIO_PIN_SET)


#define CAP_FUNC                11U  // capacitive sensor service fail error log define
#define RC522_FUNC              12U  // mifare card read service fail error log define
#define ONEWIRE_FUNC            13U  // onewire service fail error log define
#define RS485_FUNC              14U  // rs485 service fail error log define
#define MAIN_FUNC               15U  // main function fail error log define
#define DISPLAY_FUNC            16U  // display sevice fail error log define
#define LOGGER_FUNC             17U  // logger sevice fail error log define
#define DIO_FUNC                18U  // digital input-output service fail error log define
#define EEPROM_FUNC             19U  // eeprom service fail error log define
#define SIGNAL_FUNC             20U  // signal service fail error log define
#define HARD_FAULT              21U
#define MEM_FAULT               22U       
#define BUS_FAULT               23U
#define USAGE_FAULT             24U

#define SPI_DRIVER              1U   // spi driver fail error log define
#define I2C_DRIVER              2U   // i2c driver fail error log define
#define USART_DRIVER            3U   // uart driver fail error log define
#define RTC_DRIVER              4U   // rtc driver fail error log define
#define TMR_DRIVER              5U   // timer driver fail error log define
#define CRC_DRIVER              6U
#define ADC_DRIVER              7U
#define SYS_CLOCK               8U
#define SYS_EXEPTION            9U


void Error_Handler(uint8_t function, uint8_t driver);
uint32_t RTC_GetUnixTimeStamp(RTC_t* data);
void RTC_GetDateTimeFromUnix(RTC_t* data, uint32_t unix);
void RTC_GetDateTime(RTC_t* data, uint32_t format);
void FAN_SetSpeed(uint8_t fan_speed);
void FAN_SetControlType(uint8_t fan_ctrl_type);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
