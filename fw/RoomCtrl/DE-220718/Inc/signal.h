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
#ifndef __SIGNAL_H__
#define __SIGNAL_H__					221018	// version

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"


/* Defines    ----------------------------------------------------------------*/
#define ROOM_CLEANING_TIME			        0x08U   // 8h AM when clean up icon display start time
#define NORMAL_CHECK_OUT_TIME		        0x12U   // 									
#define RFID_LED_TOGGLE_TIME		        250U    // 250 ms rfid led normal blink time
#define ROOM_STATUS_TOGGLE_TIME		        500U    // 500 ms status toggle timer for DND modul to reset


/* Types  --------------------------------------------------------------------*/
typedef enum
{
	ROOM_IDLE					= ((uint8_t)0x00U),
	ROOM_READY 					= ((uint8_t)0x01U),
	ROOM_BUSY					= ((uint8_t)0x02U),
	ROOM_CLEANING				= ((uint8_t)0x03U),
	ROOM_BEDDING_REPLACEMENT	= ((uint8_t)0x04U),
	ROOM_GENERAL_CLEANING		= ((uint8_t)0x05U),
	ROOM_OUT_OF_ORDER			= ((uint8_t)0x06U),
	ROOM_LATE_CHECKOUT			= ((uint8_t)0x07U),
	ROOM_HANDMAID_IN			= ((uint8_t)0x08U),
	ROOM_FORCING_DND			= ((uint8_t)0x09U),
	ROOM_FIRE_ALARM				= ((uint8_t)0x0aU),
	ROOM_FIRE_EXIT				= ((uint8_t)0x0bU)
	
}ROOM_StatusTypeDef;


typedef enum 
{
    BUZZ_OFF                    = ((uint8_t)0x00U),
    BUZZ_CARD_INVALID           = ((uint8_t)0x01U),
    BUZZ_CARD_VALID             = ((uint8_t)0x02U),
    BUZZ_DOOR_BELL		   	    = ((uint8_t)0x03U),
    BUZZ_HANDMAID_FINISH        = ((uint8_t)0x04U),
    BUZZ_ROOM_BUSY              = ((uint8_t)0x05U),
    BUZZ_SOS_ALARM       	    = ((uint8_t)0x06U),
    BUZZ_FIRE_ALARM    		    = ((uint8_t)0x07U)
	
}SIG_BUZZER_TypeDef;


extern ROOM_StatusTypeDef ROOM_Status;
extern ROOM_StatusTypeDef ROOM_OldStatus;
extern SIG_BUZZER_TypeDef SignalBuzzer;


/* Variables  ----------------------------------------------------------------*/
extern __IO uint32_t signal_flags;
extern uint8_t aRoomPowerExpiryDateTime[6];


/* Macros     ----------------------------------------------------------------*/
#define SIGNAL_ExpiryTimeFromGuestCardSet()             (signal_flags |= (1U << 0U))
#define SIGNAL_ExpiryTimeFromGuestCardReset()	        (signal_flags &= (~ (1U << 0U)))
#define IsSIGNAL_ExpiryTimeFromGuestCardSet()	        ((signal_flags & (1U << 0U)) != 0U)


/* Function prototypes    ----------------------------------------------------*/
void SIGNAL_Service(void);


#endif
/******************************   END OF FILE  **********************************/
