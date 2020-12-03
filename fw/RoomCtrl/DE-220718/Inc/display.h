/**
 ******************************************************************************
 * File Name          : display.h
 * Date               : 10.3.2018
 * Description        : GUI display module header
 ******************************************************************************
*
* DISPLAY           pins    ->  STM32F103 Rubicon controller
* ----------------------------------------------------------------------------
* DISPLAY   +3V3    pin 1   ->  controller +3V3
* DISPLAY   GND     pin 2   ->  controller VSS
* DISPLAY   CS      pin 3   ->  PA8
* DISPLAY   RST     pin 4   ->  PA3
* DISPLAY   DC      pin 5   ->  PA2
* DISPLAY   MOSI    pin 6   ->  PA7 - SPI1 MOSI
* DISPLAY   SCK     pin 7   ->  PA5 - SPI1 SCK
* DISPLAY   LED     pin 8   ->  PB7 - PWM TIM4 CH2
* DISPLAY   MISO    pin 9   ->  PA6 - SPI1 MISO
* SD CARD   CS      pin 10  ->  PA4
* 
*
*
*
*

				******************************************
				**										**
				**		SERVICE	MENU DISPLAY SCREEN 1	**<<<<<<<<
				**										**		/\
				******************************************		/\
				**		DIGITAL   INPUT   SETTINGS		**		/\
				**		--------------------------		**		/\
				**		CARD STACKER      SET-> ON		**		/\
				**		SOS ALARM               ON		**		/\
				**		SOS RESET              OFF		**		/\
				**		HANDMAID CALL          OFF		**		/\
				**		MINIBAR SENSOR    DISABLED		**		/\
				**		BALCONY DOOR            ON		**		/\
				**		DND SWITCH             OFF		**		/\
				**		ENTRY DOOR        DISABLED		**		|
				**		--------------------------		**		|
				**		ENTER              >> NEXT		**>>>	|
				******************************************	V	|
				**		NEXT >>				SELECT 		**	V	|
				******************************************	V	|
				**		SET	++			  DESELECT		**	V	|
				******************************************	V	|
															V	|
															V	|
															V	|
															V	|
				******************************************	V	|
				**										**	V	|
				**      SERVICE	MENU DISPLAY SCREEN 2	**<<<	|
				**										**		|
				******************************************		|
				**		DIGITAL  OUTPUT   SETTINGS		**		|
				**		--------------------------		**		|
				**		POWER CONTACTOR			ON		**		|
				**		DND & HM RESET			ON		**		/\
				**		BALCONY LIGHT		   OFF		**		/\
				**		DOOR BELL			   OFF		**		/\
				**		HVAC POWER			   OFF		**		/\
				**		HVAC THERMOSTAT		   OFF		**		/\
				**		DOOR LOCK		  SET-> ON		**		|
				**		PCB BUZZER			   OFF		**		|
				**		--------------------------		**		|
				**		ENTER 	           >> NEXT		**>>>	|
				******************************************	V	|
				**		NEXT >>				SELECT 		**	V	|
				******************************************	V	|
				**		SET	++			  DESELECT		**	V	|
				******************************************	V	|
															V	|
															V	|
															V	|
															V	|
															V	|
				******************************************	V	|
				**										**	V	|
				**		SERVICE	MENU DISPLAY SCREEN 3	**<<<	|
				**										**		|
				******************************************		|
				**		SETUP  SYSTEM  SETTINGS  1		**		|
				**		--------------------------		**		|
				**		RESTART CONTROLLER				**		|
				**		PREVIEW DISPLAY IMAGES			**		/\
				**		REQUEST IMAGES UPDATE			**		/\
				**		REQUEST FIRMWARE UPDATE			**		/\
				**		SCAN ONEWIRE: LUX Term. x3		**		/\
				**		SET RS485 ADDRESS    12345		**		/\
				**		SET SYSTEM ID	     65432		**		|
				**		SET TIME  12:00 01.01.2018		**		|
				**		--------------------------		**		|
				**		ENTER 	           >> NEXT		**>>>	|
				******************************************	V	|
				**		NEXT >>				SELECT 		**	V	|
				******************************************	V	|
				**		SET	++			  DESELECT		**	V	|
				******************************************	V	|
															V	|
															V	|
															V	|
															V	|
															V	|
				******************************************	V	|
				**										**	V	|
				**		SERVICE	MENU DISPLAY SCREEN 4	**<<<	|
				**										**		|
				******************************************		|
				**		SETUP  SYSTEM  SETTINGS  2		**		|
				**		--------------------------		**		|
				**		DISPLAY CARD TEXT	   OFF		**		/\
				**		TAMPER PROTECTION		ON		**		/\
				**		WIRELESS RADIO			ON		**		/\
				**		RADIO ADDRESS		 RC001		**		/\
				**		RADIO CHANEL			22		**		/\
				**		BUZZER VOLUME          123		**		|
				**		DOORLOCK POWER	       255		**		|
				**		LOAD SYSTEM DEFAULT				**		|
				**		--------------------------		**		|
				**		ENTER 	           >> NEXT		**>>>	|
				******************************************	V	|
				**		NEXT >>				SELECT 		**	V	|
				******************************************	V	|
				**		SET	++			  DESELECT		**	V	|
				******************************************	V	|
															V	|
															V	|
															V	|
															V	|
															V	|
				******************************************	V	|
				**										**	V	|
				**		SERVICE	MENU DISPLAY SCREEN 5	**<<<	|
				**										**		|
				******************************************		|
				**		EXTEND  ACCESS	PERMISSION		**		|
				**		--------------------------		**		|
				**		PERMITTED ADDRESS 1: 12345		**		/\
				**		PERMITTED ADDRESS 2: 00000		**		/\
				**		PERMITTED ADDRESS 3: 00000		**		/\
				**		PERMITTED ADDRESS 4: 00000		**		/\
				**		PERMITTED ADDRESS 5: 00000		**		/\
				**		PERMITTED ADDRESS 6: 00000		**		/\
				**		PERMITTED ADDRESS 7: 00000		**		/\
				**		PERMITTED ADDRESS 8: 00000		**		/\
				**		--------------------------		**		|
				**		ENTER 	           >> NEXT		**>>>>>>|
				******************************************
				**		NEXT >> 			SELECT 		**
				******************************************
				**		SET	++            DESELECT		**
				******************************************
            
            
******************************************************************************
*/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DISPLAY_H__
#define __DISPLAY_H__					221018	// version


/* Include  ------------------------------------------------------------------*/
#include "stm32f1xx.h"


/* Exported Define  ----------------------------------------------------------*/
#define MENU_TIMEOUT                    30000U  // 30 sec. display_menu timeout 
#define DISPLAY_HORIZONTAL              1U
//#define DISPLAY_VERTICAL              2U
#define ROTATE_DISPLAY                  3U
#define LCD_DEFAULT_BRIGHTNESS          500U
#define LCD_DISPLAY_TIMEOUT             5U      // 5 ms lcd access timeout
#define MAX_SCREENS_IN_MENU             5U
#define MAX_ITEMS_IN_SCREEN             8U
#define VALUE_BUFF_SIZE                 16U
#define IDLE                            0U
#define PRINT                           1U
#define ACTIV                           2U
#define CHANGED                         3U
#define SELECTED                        4U
#define NOT_SELECTED                    5U
#define PENDING                         6U
#define SELECT_ITEM                     7U
#define VALUE_IN_BUFFER                 8U
#define LABEL_ITEM                      9U
#define TIME_SETUP                      10U


/* Exported Type  ------------------------------------------------------------*/
typedef enum
{
    FONT_IDLE               = ((uint8_t)0x00U),
    SMALL_FONT              = ((uint8_t)0x01U),
    MIDDLE_FONT	            = ((uint8_t)0x02U),
    BIG_FONT		        = ((uint8_t)0x03U)
    
}eDisplayFontSizeTypeDef;


typedef enum
{
    TASTER_IDLE             = ((uint8_t)0x00U),     // no taster event 
    LEFT_TASTER_PRESSED     = ((uint8_t)0x01U),     // handmaid taster pressed event
    RIGHT_TASTER_PRESSED    = ((uint8_t)0x02U),     // doorbell taster pressed event
    TASTER_END              = ((uint8_t)0x03U)      // set menu structure
    
}eTasterEventTypeDef;


/* Exported Variables  -------------------------------------------------------*/
extern __IO uint32_t display_flags;
extern uint8_t display_status;
extern uint16_t lcd_brightness;
extern uint8_t journal_mode;

/* Exported Macro   --------------------------------------------------------- */
#define DISPLAY_BootloaderUpdated()				(display_flags |= (1U << 0))
#define IsDISPLAY_BootloaderUpdatedActiv()		((display_flags & (1U << 0)) != 0U)
#define DISPLAY_BootloaderUpdatedDelete()		(display_flags &= (~ (1U << 0)))

#define DISPLAY_BootloaderUpdateFail()			(display_flags |= (1U << 1))
#define IsDISPLAY_BootloaderUpdateFailActiv()	((display_flags & (1U << 1)) != 0U)
#define DISPLAY_BootloaderUpdateFailDelete()	(display_flags &= (~ (1U << 1)))

#define DISPLAY_PreviewImage()					(display_flags |= (1U << 2))
#define IsDISPLAY_PreviewImageActiv()			((display_flags & (1U << 2)) != 0U)
#define DISPLAY_PreviewImageDelete()			((display_flags &= (~ (1U << 2))),  (display_flags |= (1U << 31)))

#define DISPLAY_RoomNumberImage()				((display_flags |= (1U << 3)),      (display_flags |= (1U << 31)))
#define IsDISPLAY_RoomNumberImageActiv()		((display_flags & (1U << 3)) != 0U)
#define DISPLAY_RoomNumberImageDelete()			((display_flags &= (~ (1U << 3))),  (display_flags |= (1U << 31)))

#define DISPLAY_DoNotDisturbImage()				((display_flags |= (1U << 4)),      (display_flags |= (1U << 31)))
#define IsDISPLAY_DoNotDisturbImageActiv()		((display_flags & (1U << 4)) != 0U)
#define DISPLAY_DoNotDisturbImageDelete()		((display_flags &= (~ (1U << 4))),  (display_flags |= (1U << 31)))

#define DISPLAY_SosAlarmImage()					((display_flags |= (1U << 5)),      (display_flags |= (1U << 31)))
#define IsDISPLAY_SosAlarmImageActiv()			((display_flags & (1U << 5)) != 0U)
#define DISPLAY_SosAlarmImageDelete()			((display_flags &= (~ (1U << 5))),  (display_flags |= (1U << 31)))

#define DISPLAY_CleanUpImage()					((display_flags |= (1U << 6)),      (display_flags |= (1U << 31)))
#define IsDISPLAY_CleanUpImageActiv()			((display_flags & (1U << 6)) != 0U)
#define DISPLAY_CleanUpImageDelete()			((display_flags &= (~ (1U << 6))),  (display_flags |= (1U << 31)))

#define DISPLAY_GeneralCleanUpImage()			((display_flags |= (1U << 7)),      (display_flags |= (1U << 31)))
#define IsDISPLAY_GeneralCleanUpImageActiv()	((display_flags & (1U << 7)) != 0U)
#define DISPLAY_GeneralCleanUpImageDelete()		((display_flags &= (~ (1U << 7))),  (display_flags |= (1U << 31)))

#define DISPLAY_CardValidImage()				(display_flags |= (1U << 8))
#define IsDISPLAY_CardValidImageActiv()			((display_flags & (1U << 8)) != 0U)
#define DISPLAY_CardValidImageDelete()			((display_flags &= (~ (1U << 8))),  (display_flags |= (1U << 31)))

#define DISPLAY_CardInvalidImage()				(display_flags |= (1U << 9))
#define IsDISPLAY_CardInvalidImageActiv()		((display_flags & (1U << 9)) != 0U)	
#define DISPLAY_CardInvalidImageDelete()		((display_flags &= (~ (1U << 9))),  (display_flags |= (1U << 31)))

#define DISPLAY_WrongRoomImage()				((display_flags |= (1U << 10)),     (display_flags |= (1U << 31)))
#define IsDISPLAY_WrongRoomImageActiv()			((display_flags & (1U << 10)) != 0U)
#define DISPLAY_WrongRoomImageDelete()			((display_flags &= (~ (1U << 10))), (display_flags |= (1U << 31)))

#define DISPLAY_TimeExpiredImage()				((display_flags |= (1U << 11)),     (display_flags |= (1U << 31)))
#define IsDISPLAY_TimeExpiredImageActiv()		((display_flags & (1U << 11)) != 0U)
#define DISPLAY_TimeExpiredImageDelete()		((display_flags &= (~ (1U << 11))), (display_flags |= (1U << 31)))

#define DISPLAY_RoomOutOfServiceImage()			((display_flags |= (1U << 12)),     (display_flags |= (1U << 31)))
#define IsDISPLAY_RoomOutOfServiceImageActiv()	((display_flags & (1U << 12)) != 0U)
#define DISPLAY_RoomOutOfServiceImageDelete()	((display_flags &= (~ (1U << 12))), (display_flags |= (1U << 31)))

#define DISPLAY_MinibarUsedImage()				((display_flags |= (1U << 13)),     (display_flags |= (1U << 31)))
#define IsDISPLAY_MinibarUsedImageActiv()		((display_flags & (1U << 13)) != 0U)
#define DISPLAY_MinibarUsedImageDelete()		((display_flags &= (~ (1U << 13))), (display_flags |= (1U << 31)))

#define DISPLAY_FireExitImage()					((display_flags |= (1U << 14)),     (display_flags |= (1U << 31)))
#define IsDISPLAY_FireExitImageActiv()			((display_flags & (1U << 14)) != 0U)
#define DISPLAY_FireExitImageDelete()			((display_flags &= (~ (1U << 14))), (display_flags |= (1U << 31)))

#define DISPLAY_FireAlarmImage()				((display_flags |= (1U << 15)),     (display_flags |= (1U << 31)))
#define IsDISPLAY_FireAlarmImageActiv()			((display_flags & (1U << 15)) != 0U)
#define DISPLAY_FireAlarmImageDelete()			((display_flags &= (~ (1U << 15))), (display_flags |= (1U << 31)))

#define DISPLAY_BeddingReplacementImage()		((display_flags |= (1U << 16)),     (display_flags |= (1U << 31)))
#define IsDISPLAY_BeddingReplacementImageActiv()((display_flags & (1U << 16)) != 0U)
#define DISPLAY_BeddingReplacementImageDelete()	((display_flags &= (~ (1U << 16))), (display_flags |= (1U << 31)))

#define DISPLAY_UpdateInProgressMessage()		(display_flags |= (1U << 17))
#define IsDISPLAY_UpdateInProgressMessageActiv()((display_flags & (1U << 17)) != 0U)
#define DISPLAY_UpdateInProgressMessageDelete()	(display_flags &= (~ (1U << 17)))

#define DISPLAY_FirmwareUpdated()               (display_flags |= (1U << 18))
#define DISPLAY_FirmwareUpdatedDelete()         (display_flags &= (~ (1U << 18)))
#define IsDISPLAY_FirmwareUpdatedActiv()        ((display_flags & (1U << 18)) != 0U)

#define DISPLAY_FirmwareUpdateFail()            (display_flags |= (1U << 19))
#define DISPLAY_FirmwareUpdateFailDelete()      (display_flags &= (~ (1U << 19)))
#define IsDISPLAY_FirmwareUpdateFailActiv()     ((display_flags & (1U << 19)) != 0U)

#define DISPLAY_UserCardInfoTextEnable()        (display_status |= (1U << 1))
#define DISPLAY_UserCardInfoTextDisable()       (display_status &= (~ (1U << 1)))
#define IsDISPLAY_UserCardInfoTextEnabled()     ((display_status & (1U << 1)) != 0U)

#define DISPLAY_RefreshSet()					(display_flags |= (1U << 31))
#define DISPLAY_RefreshReset()                  (display_flags &= (~ (1U << 31)))
#define IsDISPLAY_RefreshRequested()			((display_flags & (1U << 31)) != 0U)


/* Exported functions ------------------------------------------------------- */
void DISPLAY_Init(void);
void DISPLAY_Service(void);
void DISPLAY_SetMenuState(uint8_t set_state);
void DISPLAY_SetMenuEvent(const eTasterEventTypeDef set_event);

uint8_t IsDISPLAY_MenuActiv(void);
uint8_t IsDISPLAY_MenuPending(void);
uint8_t GetDISPLAY_ScreenState(void);
uint8_t IsDISPLAY_MenuValueChanged(void);
uint8_t IsDISPLAY_MenuPrintPending(void);
uint8_t IsDISPLAY_MenuTasterEventEnd(void);

#endif  /*__DISPLAY_H*/

