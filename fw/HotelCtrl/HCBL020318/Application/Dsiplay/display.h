/*********************************************************************
*
*       Include
*
**********************************************************************
*/
#include "stm32f4xx.h"
#include "GUI.h"


/*********************************************************************
*
*       Exported variable
*
**********************************************************************
*/

extern volatile uint32_t display_timer;
extern volatile uint32_t display_flags;

/*********************************************************************
*
*       Exported macros
*
**********************************************************************
*/
#define DISPLAY_INFO_TIME						10000
#define DISPLAY_IMAGE_TIME						5000			// 5 sec. image displayed time
#define DISPLAY_MESSAGE_TIME					3000
#define DISPLAY_SERVICE_REFRESH_TIME			1000


#define DISPLAY_StartTimer(TIME)				((display_timer = TIME), (display_flags &= 0xfffffffe))
#define DISPLAY_StopTimer()						(display_flags |= 0x00000001)
#define IsDISPLAY_TimerExpired()				(display_flags &  0x00000001)

#define DISPLAY_Update()						(display_flags |= 0x80008000)
#define IsDISPLAY_UpdateActiv()					(display_flags & 0x00008000)
#define DISPLAY_UpdateDelete()					((display_flags &= 0xffff7fff),(display_flags |= 0x80000000))

#define DISPLAY_RefreshSet()					(display_flags |= 0x80000000)
#define IsDISPLAY_RefreshRequested()			(display_flags & 0x80000000)	
#define DISPLAY_RefreshReset()					(display_flags &= 0x7fffffff)


/*********************************************************************
*
*       Exported functions
*
**********************************************************************
*/
void DISPLAY_Init(void);
void DISPLAY_Service(void);
