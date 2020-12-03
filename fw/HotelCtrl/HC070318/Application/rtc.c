/**
  ******************************************************************************
  * @file    rtc.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    11-November-2013
  * @brief   RTC functions
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "rtc.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define LSE_MAX_TRIALS_NB         6

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
RTC_InitTypeDef   RTC_InitStructure;
uint32_t RTC_Timeout = 0x10000;
int8_t RTC_Error = 0;
uint8_t RTC_HandlerFlag;
RTC_TimeTypeDef   RTC_Time;
RTC_DateTypeDef   RTC_Date;

uint8_t __IO alarm_now = 1;
__IO uint32_t LsiFreq = 0;
__IO uint32_t CaptureNumber = 0, PeriodValue = 0;

/* Private function prototypes -----------------------------------------------*/
uint32_t GetLSIFrequency(void);

/* Private functions ---------------------------------------------------------*/

/**
* @brief  Configures the RTC peripheral.
* @param  None
* @retval None
*/
int8_t RTC_Config(void)
{
	SysTick_Config(SystemCoreClock / 1000);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);	
	RTC_Error = 0;
	
	if (RTC_ReadBackupRegister(RTC_BKP_DR1) != 0xA5A5)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
		PWR_BackupAccessCmd(ENABLE);
		RCC_BackupResetCmd(ENABLE);
		RCC_BackupResetCmd(DISABLE);
		RCC_LSEConfig(RCC_LSE_ON);
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) continue;
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		RCC_RTCCLKCmd(ENABLE);
		RTC_WaitForSynchro();
		RTC_WriteBackupRegister(RTC_BKP_DR1, 0xA5A5);
	}
	else
	{
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
		{
			//Power On Reset occurred
		}
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
		{
			//External Reset occurred
		}
		
		RTC_WaitForSynchro();
	} 
	
	RCC_ClearFlag();
	return RTC_Error;
}

/**
  * @brief  Configures TIM5 to measure the LSI oscillator frequency. 
  * @param  None
  * @retval LSI Frequency
  */
uint32_t GetLSIFrequency(void)
{
  NVIC_InitTypeDef   NVIC_InitStructure;
  TIM_ICInitTypeDef  TIM_ICInitStructure;

  /* Enable the LSI oscillator ************************************************/
  RCC_LSICmd(ENABLE);
  
  /* Wait till LSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {}

  /* TIM5 configuration *******************************************************/ 
  /* Enable TIM5 clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
  
  /* Connect internally the TIM5_CH4 Input Capture to the LSI clock output */
  TIM_RemapConfig(TIM5, TIM5_LSI);

  /* Configure TIM5 presclaer */
  TIM_PrescalerConfig(TIM5, 0, TIM_PSCReloadMode_Immediate);
  
  /* TIM5 configuration: Input Capture mode ---------------------
     The LSI oscillator is connected to TIM5 CH4
     The Rising edge is used as active edge,
     The TIM5 CCR4 is used to compute the frequency value 
  ------------------------------------------------------------ */
  TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV8;
  TIM_ICInitStructure.TIM_ICFilter = 0;
  TIM_ICInit(TIM5, &TIM_ICInitStructure);
  
  /* Enable TIM5 Interrupt channel */
  NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable TIM5 counter */
  TIM_Cmd(TIM5, ENABLE);

  /* Reset the flags */
  TIM5->SR = 0;
    
  /* Enable the CC4 Interrupt Request */  
  TIM_ITConfig(TIM5, TIM_IT_CC4, ENABLE);


  /* Wait until the TIM5 get 2 LSI edges (refer to TIM5_IRQHandler() in 
    stm32fxxx_it.c file) ******************************************************/
  while(CaptureNumber != 2)
  {
  }
  /* Deinitialize the TIM5 peripheral registers to their default reset values */
  TIM_DeInit(TIM5);
  
  /* Get PCLK1 prescaler */
  if ((RCC->CFGR & RCC_CFGR_PPRE1) == 0)
  { 
    /* PCLK1 prescaler equal to 1 => TIMCLK = PCLK1 */
    return (((SystemCoreClock/4) / PeriodValue) * 8);
  }
  else
  { /* PCLK1 prescaler different from 1 => TIMCLK = 2 * PCLK1 */
    return (((2 * (SystemCoreClock/4)) / PeriodValue) * 8) ;
  }
}

/**
* @brief  This function handles RTC Alarm A interrupt request.
* @param  None
* @retval None
*/
void RTC_Alarm_IRQHandler(void)
{
  /* Clear the EXTIL line 17 */
  EXTI_ClearITPendingBit(EXTI_Line17);

  /* Check on the AlarmA falg and on the number of interrupts per Second (60*8) */
  if (RTC_GetITStatus(RTC_IT_ALRA) != RESET)
  {

    alarm_now = 0;
    /* Clear RTC AlarmA Flags */
    RTC_ClearITPendingBit(RTC_IT_ALRA);
    RTC_HandlerFlag = ENABLE;
  }
}

void RTC_HttpCgiDateTimeUpdate(uint8_t *ibuff)
{
	  /* WeekDay set by user can be ignored because automatically calculated */
//	hrtc->DateToUpdate.WeekDay = RTC_WeekDayNum(hrtc->DateToUpdate.Year, hrtc->DateToUpdate.Month, hrtc->DateToUpdate.Date);
//	sDate->WeekDay = hrtc->DateToUpdate.WeekDay;
	
	RTC_Date.RTC_WeekDay = (ibuff[0] - 48);
	RTC_Date.RTC_Date = (ibuff[1] - 48) << 4;
	RTC_Date.RTC_Date += ibuff[2] - 48;
	RTC_Date.RTC_Month = (ibuff[3] - 48) << 4;
	RTC_Date.RTC_Month += ibuff[4] - 48;

	// skipp two byte for full year format "20"
	RTC_Date.RTC_Year = (ibuff[7] - 48) << 4;
	RTC_Date.RTC_Year += ibuff[8] - 48;
	RTC_Time.RTC_Hours = (ibuff[9] - 48) << 4;
	RTC_Time.RTC_Hours += ibuff[10] - 48;
	RTC_Time.RTC_Minutes = (ibuff[11] - 48) << 4;
	RTC_Time.RTC_Minutes += ibuff[12] - 48;
	RTC_Time.RTC_Seconds = (ibuff[13] - 48) << 4;
	RTC_Time.RTC_Seconds += ibuff[14] - 48;

	PWR_BackupAccessCmd(ENABLE);
	RTC_SetTime(RTC_Format_BCD, &RTC_Time);
	RTC_SetDate(RTC_Format_BCD, &RTC_Date);
	PWR_BackupAccessCmd(DISABLE);
	
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
