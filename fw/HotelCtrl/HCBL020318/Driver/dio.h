/**
 ******************************************************************************
 * File Name          : dio.h
 * Date               : 08/05/2016 23:16:19
 * Description        : digital io interface modul header
 ******************************************************************************
 *
 *
 ******************************************************************************
 */
#ifndef DIO_H
#define DIO_H     						100	// version 1.00

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported defines    -------------------------------------------------------*/
#define RED_SIGNAL_TIME		5000
#define GREEN_SIGNAL_TIME	5000
#define DIO_TIMEOUT     	0x3000	// Timeout Zeit

/* Defines    ----------------------------------------------------------------*/
#define TASTER_S2_PORT				GPIOA
#define TASTER_S2_PIN				GPIO_Pin_0
#define TASTER_S2_CLK				RCC_AHB1Periph_GPIOA

#define HALL_CW_DIR_PORT			GPIOA
#define HALL_CW_DIR_PIN				GPIO_Pin_15
#define HALL_CW_DIR_CLK				RCC_AHB1Periph_GPIOA

#define BARRIER_MOTOR_CLOCK_PORT	GPIOD
#define BARRIER_MOTOR_CLOCK_PIN 	GPIO_Pin_13
#define BARRIER_MOTOR_CLOCK_CLK		RCC_AHB1Periph_GPIOB

#define STACKER_M1_PORT				GPIOB
#define STACKER_M1_PIN				GPIO_Pin_10
#define STACKER_M1_CLK				RCC_AHB1Periph_GPIOB

#define STACKER_M2_PORT				GPIOC
#define STACKER_M2_PIN				GPIO_Pin_2
#define STACKER_M2_CLK				RCC_AHB1Periph_GPIOC

#define STACKER_M3_PORT				GPIOC
#define STACKER_M3_PIN				GPIO_Pin_3
#define STACKER_M3_CLK				RCC_AHB1Periph_GPIOC

#define BEEP_GPIO_PORT        		GPIOD
#define BEEP_GPIO_PIN       		GPIO_Pin_3
#define BEEP_GPIO_CLK       		RCC_AHB1Periph_GPIOD

#define TASTER_S3_PORT				GPIOD
#define TASTER_S3_PIN				GPIO_Pin_11
#define TASTER_S3_CLK				RCC_AHB1Periph_GPIOD

#define LED_GPIO_PORT           	GPIOD
#define LED1_GPIO_PIN          		GPIO_Pin_11
#define LED2_GPIO_PIN          		GPIO_Pin_12
#define LED_GPIO_CLK           		RCC_AHB1Periph_GPIOD

#define STACKER_SENSOR_PORT			GPIOE
#define SENSOR_CARD_LOADED_PIN		GPIO_Pin_2
#define SENSOR_CARD_INSERTED_PIN	GPIO_Pin_3
#define STACKER_SENSOR_CLK			RCC_AHB1Periph_GPIOE

#define HALL_CCW_DIR_PORT			GPIOE
#define HALL_CCW_DIR_PIN			GPIO_Pin_5
#define HALL_CCW_DIR_CLK			RCC_AHB1Periph_GPIOE

#define HALL_10_DEGREE_PORT			GPIOE
#define HALL_10_DEGREE_PIN			GPIO_Pin_6
#define HALL_10_DEGREE_CLK			RCC_AHB1Periph_GPIOE

#define STACKER_M4_PORT				GPIOF
#define STACKER_M4_PIN				GPIO_Pin_6
#define STACKER_M4_CLK				RCC_AHB1Periph_GPIOF


/* Exported variables  -------------------------------------------------------*/
extern volatile uint8_t dout_0_7;
extern volatile uint8_t dout_8_15;
extern volatile uint8_t dout_0_7_old;
extern volatile uint8_t dout_8_15_old;
extern volatile uint16_t red_signal_timer;
extern volatile uint16_t green_signal_timer;


/* Exported macros     -------------------------------------------------------*/
//#define ExitGreenSignal_On()							((dout_0_7 &= 0xfe),(green_signal_timer = GREEN_SIGNAL_TIME))
#define ExitGreenSignal_On()							(dout_0_7 &= 0xfe)
#define ExitGreenSignal_Off()							(dout_0_7 |= 0x01)
#define GetExitGreenSignalState()						(dout_0_7 & 0x01)
//#define EntryRedSignal_On()							((dout_0_7 &= 0xfd), (red_signal_timer = RED_SIGNAL_TIME))
#define EntryRedSignal_On()								(dout_0_7 &= 0xfd)
#define EntryRedSignal_Off()							(dout_0_7 |= 0x02)
#define GetEntryRedSignalState()						(dout_0_7 & 0x02)
//#define EntryGreenSignal_On()							((dout_0_7 &= 0xfb),(green_signal_timer = GREEN_SIGNAL_TIME))
#define EntryGreenSignal_On()							(dout_0_7 &= 0xfb)
#define EntryGreenSignal_Off()							(dout_0_7 |= 0x04)
#define GetEntryGreenSignalState()						(dout_0_7 & 0x04)
//#define ExitRedSignal_On()							((dout_0_7 &= 0xf7), (red_signal_timer = RED_SIGNAL_TIME))
#define ExitRedSignal_On()								(dout_0_7 &= 0xf7)
#define ExitRedSignal_Off()								(dout_0_7 |= 0x08)
#define GetExitRedSignalState()							(dout_0_7 & 0x08)
#define CardStackerGreenSignal_On()						(dout_0_7 &= 0xef)
#define CardStackerGreenSignal_Off()					(dout_0_7 |= 0x10)
#define GetCardStackerGreenSignalState()				(dout_0_7 & 0x10)
#define CardStackerRedSignal_On()						(dout_0_7 &= 0xdf)
#define CardStackerRedSignal_Off()						(dout_0_7 |= 0x20)
#define GetCardStackerRedSignalState()					(dout_0_7 & 0x20)

#define BarrierLedSignal_AllOff()						(dout_0_7 |= 0x0f)

#define BarrierDriveEnableAssert()						(dout_8_15 |= 0x01)
#define BarrierDriveEnableRelease()						(dout_8_15 &= 0xfe)
#define GetBarrierDriveEnableState()					(dout_8_15_old & 0x01)
#define BarrierDirectionCW()							(dout_8_15 &= 0xfd)
#define BarrierDirectionCCW()							(dout_8_15 |= 0x02)
#define GetBarrierDriveDirection()						(dout_8_15_old & 0x02)
#define BarrierDriveResetAssert()						(dout_8_15 &= 0xfb)	
#define BarrierDriveResetRelease()						(dout_8_15 |= 0x04)
#define GetBarrierDriveResetState()						(dout_8_15_old & 0x04)
#define BarrierDriveVref1Assert()						(dout_8_15 |= 0x08)
#define BarrierDriveVref1Release()						(dout_8_15 &= 0xf7)
#define GetBarrierDriveVref1State()						(dout_8_15_old & 0x08)
#define Mifare1_Select()								(dout_8_15 &= 0xef)
#define Mifare1_Unselect()								(dout_8_15 |= 0x10)
#define Mifare2_Select()								(dout_8_15 &= 0xdf)
#define Mifare2_Unselect()								(dout_8_15 |= 0x20)
#define Mifare3_Select()								(dout_8_15 &= 0xbf)
#define Mifare3_Unselect()								(dout_8_15 |= 0x40)
#define MifareResetAssert()								(dout_8_15 &= 0x7f)
#define MifareResetRelease()							(dout_8_15 |= 0x80)


#define GetCardStackerInsertSensorState()				GPIO_ReadInputDataBit(STACKER_SENSOR_PORT, SENSOR_CARD_INSERTED_PIN)
#define GetCardStackerLoadSensorState()					GPIO_ReadInputDataBit(STACKER_SENSOR_PORT, SENSOR_CARD_LOADED_PIN)
#define GetBarrierCwSensorState()						GPIO_ReadInputDataBit(HALL_CW_DIR_PORT, HALL_CW_DIR_PIN)
#define GetBarrierCcwSensorState()						GPIO_ReadInputDataBit(HALL_CCW_DIR_PORT, HALL_CCW_DIR_PIN)
#define GetBarrierEncoderSensorState()					GPIO_ReadInputDataBit(HALL_10_DEGREE_PORT, HALL_10_DEGREE_PIN)
#define GetDisplayTouchControllerIrqState()				1
#define GetDisplayTouchControllerBusyState()			1


/* Exported functions  -------------------------------------------------------*/
void DIO_Init(void);
void DOUT_Service(void);
void LED_Toggle(uint8_t led);
void BUZZER_Off(void);
void BUZZER_On(void);

#endif
/******************************   END OF FILE  **********************************/
