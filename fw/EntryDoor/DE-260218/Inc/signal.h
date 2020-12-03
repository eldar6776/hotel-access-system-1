/**
 ******************************************************************************
 * File Name          : signaling.h
 * Date               : 28/02/2016 23:16:19
 * Description        : audio visual signaling software modul header
 ******************************************************************************
 *
 *
 ******************************************************************************
 */
#ifndef SIGNAL_H
#define SIGNAL_H     		100


/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"


/* Defines    ----------------------------------------------------------------*/
#define OPEN_TIME			2345	// 3s door lock open


/* Types  --------------------------------------------------------------------*/
typedef enum 
{
    BUZZ_OFF                = 0x00,
    BUZZ_CARD_INVALID       = 0x01,
    BUZZ_CARD_VALID         = 0x02,
    BUZZ_DOOR_BELL		   	= 0x03,
    BUZZ_HANDMAID_ENTRY     = 0x04,
    BUZZ_HANDMAID_FINISH    = 0x05,
    BUZZ_ROOM_BUSY          = 0x06,
    BUZZ_SOS_ALARM       	= 0x07,
    BUZZ_FIRE_ALARM    		= 0x08
	
}SIG_BUZZER_TypeDef;


/* Variables  ----------------------------------------------------------------*/
extern volatile uint32_t signal_timer;
extern volatile uint32_t doorlock_timer;
extern volatile uint32_t signal_flags;
extern SIG_BUZZER_TypeDef          		SignalBuzzer;


/* Macros     ----------------------------------------------------------------*/
#define SIGNAL_StartTimer(TIME)					(signal_timer = TIME)
#define SIGNAL_StopTimer()						(signal_timer = 0)
#define IsSIGNAL_TimerExpired()					(signal_timer == 0)
#define DOORLOCK_StartTimer(DOOR_TIME)			(doorlock_timer = DOOR_TIME)
#define DOORLOCK_StopTimer()					(doorlock_timer = 0)
#define IsDOORLOCK_TimerExpired()				(doorlock_timer == 0)
#define Buzzer_On()                     		(HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET))
#define Buzzer_Off()                    		(HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET))
#define IsBuzzerActiv()							(HAL_GPIO_ReadPin(BUZZER_GPIO_Port, BUZZER_Pin))
#define LED_RedOn()                     		(HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET))
#define LED_RedOff()                    		(HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET))
#define IsLED_RedActiv()						(HAL_GPIO_ReadPin(LED_RED_GPIO_Port, LED_RED_Pin))
#define LED_GreenOn()                     		(HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET))
#define LED_GreenOff()                    		(HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET))
#define IsLED_GreenActiv()						(HAL_GPIO_ReadPin(LED_GREEN_GPIO_Port, LED_GREEN_Pin) == GPIO_PIN_RESET)


/* Function prototypes    ----------------------------------------------------*/
void SIGNAL_Init(void);
void SIGNAL_Service(void);

#endif
/******************************   END OF FILE  **********************************/
