/**
 ******************************************************************************
 * File Name          : time.h
 * Date               : 28/02/2016 23:15:16
 * Description        : time management software modul header
 ******************************************************************************
 *
 *
 ******************************************************************************
 */

#ifndef TIME_H
#define TIME_H 100  // version 1.00

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Defines    ----------------------------------------------------------------*/

/* Types  --------------------------------------------------------------------*/

/* Variables  ----------------------------------------------------------------*/
extern TIM_HandleTypeDef htim3;
extern UART_HandleTypeDef huart1;
extern RTC_TimeTypeDef time;
extern RTC_DateTypeDef date;
extern RTC_TimeTypeDef time;
extern const char month_january[];
extern const char month_february[];
extern const char month_march[];
extern const char month_april[];
extern const char month_may[];
extern const char month_june[];
extern const char month_july[];
extern const char month_august[];
extern const char month_september[];
extern const char month_october[];
extern const char month_november[];
extern const char month_december[];
extern const char day_monday[];
extern const char day_tuesday[];
extern const char day_wednesday[];
extern const char day_thursday[];
extern const char day_friday[];
extern const char day_saturday[];
extern const char day_sunday[];

/* Function prototypes    ----------------------------------------------------*/

#endif
/******************************   END OF FILE  **********************************/


