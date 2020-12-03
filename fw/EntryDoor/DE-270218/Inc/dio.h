/**
 ******************************************************************************
 * File Name          : dio_interface.h
 * Date               : 28/02/2016 23:16:19
 * Description        : digital io interface modul header
 ******************************************************************************
 *
 *  Rubicon controller      ->  STM32F103
 * ----------------------------------------------------------------------------
 * LED ROOM MAID    - GREEN ->  PB0
 * LED ROOM MAID    - RED   ->  PB1
 * LED DONT DISTURB - BLUE  ->  PB2
 * LED ROOM BUSY    - RED   ->  PB3
 * LED ROOM NUMBER  - WHITE ->  PB4
 * SHIFT STROBE MUx MB      ->  PB11
 * MUx IN                   ->  PB12
 * LED RFID READER  - BLUE  ->  PC13
 *
 ******************************************************************************
 */
 
#ifndef DIO_H
#define DIO_H     			105

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"


/* Defines    ----------------------------------------------------------------*/


/* Types  --------------------------------------------------------------------*/
/* Variables  ----------------------------------------------------------------*/
extern volatile uint32_t dio_flags;
extern volatile uint32_t dio_timer;


/* Macros     ----------------------------------------------------------------*/
#define LED_GuestInRoomGreen_On()     	(HAL_GPIO_WritePin(LED_ROOM_GN_GPIO_Port, LED_ROOM_GN_Pin, GPIO_PIN_SET))
#define LED_GuestInRoomGreen_Off()    	(HAL_GPIO_WritePin(LED_ROOM_GN_GPIO_Port, LED_ROOM_GN_Pin, GPIO_PIN_RESET))
#define LED_GuestInRoomGreen_Toggle()	(HAL_GPIO_TogglePin(LED_ROOM_GN_GPIO_Port, LED_ROOM_GN_Pin))

#define DIO_StartTimer(TIME)   			((dio_timer = TIME),(dio_flags &= 0xfffffffe))
#define DIO_StopTimer()					(dio_flags |= 0x00000001)
#define IsDIO_TimerExpired()			(dio_flags &  0x00000001)


/* Function prototypes    ----------------------------------------------------*/
extern void DIO_Init(void);
extern void DIO_Service(void);
#endif
/******************************   END OF FILE  **********************************/

