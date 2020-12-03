/**
  * @file    main.c 
  * @author  eldar6776@hotmail.com
  * @version V1,0
  * @date    06.03.2017
  * @brief   Main Program
  */
  
#include "ff.h"	
#include "main.h"
#include "buzzer.h"
#include "command.h"
#include "flash_if.h"
#include "i2c_eeprom.h"
#include "stm32f429i_lcd.h"
#include "stm32f4xx_fmc.h"

/* Constante ----------------------------------------------------------------*/
const char sys_info[] = "HW: RED_DRAGON_F429, REV:1,0;      FW: HCBL020318 ";

/* Private typedef -----------------------------------------------------------*/
#define TASTER_S2_PORT				GPIOA
#define TASTER_S2_PIN				GPIO_Pin_0
#define TASTER_S2_CLK				RCC_AHB1Periph_GPIOA


#define LED_GPIO_PORT           	GPIOD
#define LED1_GPIO_PIN          		GPIO_Pin_11
#define LED2_GPIO_PIN          		GPIO_Pin_12
#define LED_GPIO_CLK           		RCC_AHB1Periph_GPIOD

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern FATFS fatfs;
extern FIL file;
extern FIL fileR;
extern DIR dir;
extern FILINFO fno;

/* Private function prototypes -----------------------------------------------*/

uint32_t system_id;
uint32_t system_config;

pFunction Jump_To_Application;
uint32_t JumpAddress;

__IO uint32_t TimingDelay;


/*****************************************************************************
**   Main Function  main()
******************************************************************************/
int main(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    SysTick_Config(SystemCoreClock / 1000);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
    RCC_ClearFlag();
    
	RCC_AHB1PeriphClockCmd(TASTER_S2_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = TASTER_S2_PIN;
	GPIO_Init(TASTER_S2_PORT, &GPIO_InitStructure);
    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD , ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOD, GPIO_Pin_3);
    
	I2C_EERPOM_Init();
    
	system_config = (uint8_t)I2C_EERPOM_ReadByte16(I2C_EE_READ_PAGE_0, EE_SYSTEM_CONFIG_ADDRESS);
	
	if(!IsBootloaderRequested() && (GPIO_ReadInputDataBit(TASTER_S2_PORT, TASTER_S2_PIN) == Bit_RESET))
	{
		ApplicationExe();
	}
		
	LCD_Init(); 
	LCD_LayerInit();
	LTDC_Cmd(ENABLE);
	LCD_SetLayer(LCD_FOREGROUND_LAYER);
	LCD_Clear(LIGHTBLUE);
	LCD_SetTextColor(WHITE);
	LCD_SetBackColor(LIGHTBLUE);
	LCD_DisplayStringLine(LCD_LINE_2, (uint8_t*)"    BOOTLOADER AKTIVAN");
	GPIO_WriteBit(GPIOD, GPIO_Pin_3, Bit_SET);
	TimingDelay = 100;
	while(TimingDelay) continue;
	GPIO_WriteBit(GPIOD, GPIO_Pin_3, Bit_RESET);
	/* Initialises the File System*/
	if (f_mount(&fatfs, "0:", 0) != FR_OK ) 
	{
		BootloaderSDcardError();
		I2C_EERPOM_WriteByte16(I2C_EE_WRITE_PAGE_0, EE_SYSTEM_CONFIG_ADDRESS, (uint8_t) system_config);
		LCD_DisplayStringLine(LCD_LINE_4, (uint8_t*)"Greska kod citanja kartice");
		LCD_DisplayStringLine(LCD_LINE_5, (uint8_t*)"1. SD kartica nije ubacena");
		LCD_DisplayStringLine(LCD_LINE_6, (uint8_t*)"2. SD kartica neispravna");
		LCD_DisplayStringLine(LCD_LINE_7, (uint8_t*)"3. pogresan format SD kartice");
		LCD_DisplayStringLine(LCD_LINE_10, (uint8_t*)"Restart aplikacije za 2 sekunde");
		TimingDelay = 2000;
		while(TimingDelay) continue;
		ApplicationExe();
	}
	/* Flash unlock */
	FLASH_If_FlashUnlock();
	COMMAND_Download();
	GPIO_WriteBit(GPIOD, GPIO_Pin_3, Bit_SET);
	TimingDelay = 100;
	while(TimingDelay) continue;
	GPIO_WriteBit(GPIOD, GPIO_Pin_3, Bit_RESET);
	TimingDelay = 100;
	while(TimingDelay) continue;
	GPIO_WriteBit(GPIOD, GPIO_Pin_3, Bit_SET);
	TimingDelay = 100;
	while(TimingDelay) continue;
	GPIO_WriteBit(GPIOD, GPIO_Pin_3, Bit_RESET);
	BootloaderUpdateSucces();
	I2C_EERPOM_WriteByte16(I2C_EE_WRITE_PAGE_0, EE_SYSTEM_CONFIG_ADDRESS, (uint8_t) system_config);
	LCD_DisplayStringLine(LCD_LINE_8, (uint8_t*)"-upis firmware-a uspjesan");
	LCD_DisplayStringLine(LCD_LINE_10, (uint8_t*)"Start aplikacije za 3 sekunde");
	TimingDelay = 3000;
	while(TimingDelay) continue;
	NVIC_SystemReset();
	
	while (1) 
	{
		LED_GPIO_PORT->ODR ^= LED2_GPIO_PIN;
		Delay(500);
	}
}

void ApplicationExe(void)
{
    if (((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
    {
        I2C_DeInit(I2C2);
        LTDC_DeInit();
        FMC_SDRAMDeInit(FMC_Bank2_SDRAM);
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, DISABLE);
        GPIO_DeInit(GPIOA);
        GPIO_DeInit(GPIOB);
        GPIO_DeInit(GPIOC);
        GPIO_DeInit(GPIOD);
        GPIO_DeInit(GPIOE);
        GPIO_DeInit(GPIOF);
        GPIO_DeInit(GPIOG);
        GPIO_DeInit(GPIOH);
        RCC_DeInit();
        RCC_ClearFlag();
		/* Jump to user application */
		JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
		Jump_To_Application = (pFunction) JumpAddress;
		/* Initialize user application's Stack Pointer */
		__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
		Jump_To_Application();
    }
	GPIO_WriteBit(GPIOD, GPIO_Pin_3, Bit_SET);
	TimingDelay = 2000;
	while(TimingDelay) continue;
	GPIO_WriteBit(GPIOD, GPIO_Pin_3, Bit_RESET);
	LCD_Clear(LIGHTBLUE);
	LCD_DisplayStringLine(LCD_LINE_2, (uint8_t*)"   GRESKA FIRMWARE-a");
	LCD_DisplayStringLine(LCD_LINE_6, (uint8_t*)"1. iskljuci napajanje");
	LCD_DisplayStringLine(LCD_LINE_7, (uint8_t*)"2. na micro SD karticu");
	LCD_DisplayStringLine(LCD_LINE_8, (uint8_t*)"	upisi ispravan fajl");
	LCD_DisplayStringLine(LCD_LINE_9, (uint8_t*)"3. ukljuci napajanje");
	Fail_Handler();
}

void Fail_Handler(void)
{
	while(1)
	{
		/* Toggle Red LED */
		LED_GPIO_PORT->ODR ^= LED1_GPIO_PIN;
		Delay(50);
	}
}

void Delay(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

