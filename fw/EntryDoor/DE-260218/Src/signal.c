/**
 ******************************************************************************
 * File Name          : signaling.c
 * Date               : 28/02/2016 23:16:19
 * Description        : audio visual signaling software modul
 ******************************************************************************
 *
 *
 ******************************************************************************
 */
 
/* Includes ------------------------------------------------------------------*/
#include "signal.h"

/* Defines    ----------------------------------------------------------------*/

/* Types  --------------------------------------------------------------------*/

/* Macros     ----------------------------------------------------------------*/

/* Variables  ----------------------------------------------------------------*/
volatile uint32_t signal_timer;
volatile uint32_t signal_flags;
volatile uint32_t doorlock_timer;

SIG_BUZZER_TypeDef          	SignalBuzzer;


/* Private prototypes    -----------------------------------------------------*/

/* Program code   ------------------------------------------------------------*/
void SIGNAL_Init(void)
{
	SignalBuzzer = BUZZ_OFF;
	LED_GreenOff();
	LED_RedOff();
}

void SIGNAL_Service(void)
{
	static uint16_t signal_pcnt = 0;
	
	if(IsDOORLOCK_TimerExpired() && IsLED_GreenActiv()) LED_GreenOff();	
	if(!IsSIGNAL_TimerExpired()) return;

	/*************************************************/
	/**
	*       B U Z Z E R   S I G N A L
	*/
	/*************************************************/
	if(SignalBuzzer == BUZZ_OFF)
	{
	   Buzzer_Off();
	   signal_pcnt = 0;        
	} 
	else if(SignalBuzzer == BUZZ_CARD_INVALID)
	{       
		if(signal_pcnt == 0) 
		{
			Buzzer_On();
			SIGNAL_StartTimer(50);
			++signal_pcnt;
		}
		else if(signal_pcnt == 1) 
		{
			Buzzer_Off();
			SIGNAL_StartTimer(50);
			++signal_pcnt;
		}
		else if(signal_pcnt == 2) 
		{
			Buzzer_On();
			SIGNAL_StartTimer(50);
			++signal_pcnt;
		}
		else if(signal_pcnt == 3) 
		{
			Buzzer_Off();
			SIGNAL_StartTimer(50);
			++signal_pcnt;
		}
		else if(signal_pcnt == 4) 
		{
			Buzzer_On();
			SIGNAL_StartTimer(50);
			++signal_pcnt;
		}
		else if(signal_pcnt == 5) 
		{
			Buzzer_Off();
			SIGNAL_StartTimer(50);
			++signal_pcnt; 			
		}  
		else if(signal_pcnt >= 6) 
		{ 
			SignalBuzzer = BUZZ_OFF;
		}  
	} 
	else if(SignalBuzzer == BUZZ_CARD_VALID)
	{
		if(signal_pcnt == 0) 
		{
			Buzzer_On();
			SIGNAL_StartTimer(100);
			++signal_pcnt;
		}
		else if(signal_pcnt >= 1) 
		{
			SignalBuzzer = BUZZ_OFF;
		}
		
	} 
	else if(SignalBuzzer == BUZZ_DOOR_BELL)
	{    
		if(signal_pcnt == 0) 
		{
			Buzzer_On();
			SIGNAL_StartTimer(20);
			++signal_pcnt;
		}
		else if(signal_pcnt >= 1) 
		{
			SignalBuzzer = BUZZ_OFF;
		}
	} 
}

/******************************   END OF FILE  **********************************/
