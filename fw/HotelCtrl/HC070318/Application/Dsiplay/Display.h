/*********************************************************************
*
*       Include
*
**********************************************************************
*/
#include "stm32f4xx.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define RS485_SCAN_MAX_ATTEMPTS		10
#define DISPLAY_KEYPAD_DELAY		1000
#define DISPLAY_UNLOCK_TIMEOUT		30000

typedef enum
{
	DISPLAY_DEFAULT					= 0x00,
	DISPLAY_TOOLS					= 0x01,
	DISPLAY_SETTINGS				= 0x02,
	DISPLAY_SCAN_RS485_BUS			= 0x03,
	DISPLAY_SETTINGS_EDIT			= 0x04
	
}eActivDisplayTypeDef;	


/*********************************************************************
*
*       Exported variable
*
**********************************************************************
*/
extern uint8_t log_list_row;
extern uint32_t display_flags;
extern uint32_t display_timer;
extern uint32_t display_timeout_timer;
extern uint8_t string1[84];
extern uint8_t string2[84];
extern uint8_t string3[84];
extern uint8_t string4[84];
extern uint8_t string5[84];
extern uint8_t *pString;
extern eActivDisplayTypeDef eActivDisplay;



#define DISPLAY_FileProgressBarShowSet()				(display_flags |= 0x00000001)
#define DISPLAY_FileProgressBarShowReset()				(display_flags &= 0xfffffffe)
#define IsDISPLAY_FileProgressBarShowActiv()			(display_flags & 0x00000001)
#define DISPLAY_FileProgressBarDeleteSet()				(display_flags |= 0x00000002)
#define DISPLAY_FileProgressBarDeleteReset()			(display_flags &= 0xfffffffd)
#define IsDISPLAY_FileProgressBarDeleteActiv()			(display_flags & 0x00000002)
#define DISPLAY_uSD_CardErrorSet()						(display_flags |= 0x00000004)
#define DISPLAY_uSD_CardErrorReset()					(display_flags &= 0xfffffffb)
#define IsDISPLAY_uSD_CardErrorActiv()					(display_flags & 0x00000004)
#define DISPLAY_uSD_CardReadySet()						(display_flags |= 0x00000008)
#define DISPLAY_uSD_CardReadyReset()					(display_flags &= 0xfffffff7)
#define IsDISPLAY_uSD_CardReadyActiv()					(display_flags & 0x00000008)
#define DISPLAY_uSD_CardReadyStatusSet()				(display_flags |= 0x00000010)
#define DISPLAY_uSD_CardErrorStatusSet()				(display_flags &= 0xffffffef)
#define GetDisplay_uSD_CardActivStatus()				(display_flags & 0x00000010)
#define DISPLAY_OnScreenKeypadSet()						(display_flags |= 0x00000020)
#define DISPLAY_OnScreenKeypadReset()					(display_flags &= 0xffffffdf)
#define IsDISPLAY_OnScreenKeypadActiv()					(display_flags & 0x00000020)
#define DISPLAY_RefreshSet()							(display_flags |= 0x00000040)	
#define DISPLAY_RefreshReset()							(display_flags &= 0xffffffbf)
#define IsDISPLAY_RefreshActiv()						(display_flags & 0x00000040)
#define DISPLAY_UnlockSet()								(display_flags |= 0x00000080)
#define DISPLAY_UnlockReset()							(display_flags &= 0xffffff7f)
#define IsDISPLAY_UnlockActiv()							(display_flags & 0x00000080)	
#define DISPLAY_UpdateTimeSet()							(display_flags |= 0x00000100)
#define DISPLAY_UpdateTimeReset()						(display_flags &= 0xfffffeff)
#define IsDISPLAY_UpdateTimeActiv()						(display_flags & 0x00000100)	

#define DISPLAY_TimerStart(TIME)						(display_timer = TIME)
#define DISPLAY_TimerStop()								(display_timer = 0)
#define IsDISPLAY_TimerExpired()						(display_timer == 0)
#define DISPLAY_TimeoutTimerStart(TIME)					(display_timeout_timer = TIME)
#define DISPLAY_TimeoutTimerStop()						(display_timeout_timer = 0)
#define IsDISPLAY_TimeoutTimerExpired()					(display_timeout_timer == 0)
/*********************************************************************
*
*       Exported functions
*
**********************************************************************
*/
void DisplayInit(void);
void DisplayService(void);
void UpdateLogDisplay(void);
void UpdateFileTransferDisplay(void);
int APP_GetData(void * p, const unsigned char ** ppData, unsigned NumBytes, unsigned long Off);
