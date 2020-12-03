/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : W25Q64 SPI FLASH BOOTLOADER
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h" 
#include "main.h"

/* Private typedefs ----------------------------------------------------------*/
typedef  void (*pFunction)(void);
I2C_HandleTypeDef hi2c1;
SPI_HandleTypeDef hspi2;


/* Private variables ---------------------------------------------------------*/
//const char sys_info[] = "HW: DE-290815, REV-3_18;                  FW: RCBL160318 ";

pFunction JumpToApplication;
uint32_t jump_address;
uint8_t dout = 0U;


#define LED_HandmaidGreen_On()        	(dout |= (1U << 0U))
#define LED_HandmaidGreen_Off()       	(dout &= (~(1U << 0U)))
#define LED_HandmaidGreen_Toggle()    	((dout & (1U << 0U)) ? LED_HandmaidGreen_Off() : LED_HandmaidGreen_On())
#define LED_RoomStatusRed_On()      	(dout |= (1U << 1U))
#define LED_RoomStatusRed_Off()     	(dout &= (~(1U << 1U)))
#define LED_RoomStatusRed_Toggle() 		((dout & (1U << 1U)) ? LED_RoomStatusRed_Off() : LED_RoomStatusRed_On())
#define LED_DoorBellBlue_On()     		(dout |= (1U << 2U))
#define LED_DoorBellBlue_Off()    		(dout &= (~(1U << 2U)))
#define LED_DoorBellBlue_Toggle()		((dout & (1U << 2U)) ? LED_DoorBellBlue_Off() : LED_DoorBellBlue_On())
#define LED_RfidReaderWhite_On()        (dout |= (1U << 3U))
#define LED_RfidReaderWhite_Off()       (dout &= (~(1U << 3U)))
#define LED_RfidReaderWhite_Toggle()    ((dout & (1U << 3U)) ? LED_RfidReaderWhite_Off() : LED_RfidReaderWhite_On())
#define DOUT_3_SetHigh()				(dout |= (1U << 4U))
#define DOUT_3_SetLow()					(dout &= (~(1U << 4U)))
#define Get_DOUT_3_State()				(dout &  (1U << 4U))
#define DOUT_4_SetHigh()				(dout |= (1U << 5U))
#define DOUT_4_SetLow()					(dout &= (~(1U << 5U)))
#define Get_DOUT_4_State()				(dout &  (1U << 5U))
#define DISPLAY_RST_SetHigh()			(dout |= (1U << 6U))
#define DISPLAY_RST_SetLow()			(dout &= (~(1U << 6U)))
#define RC522_RST_SetHigh()				(dout |= (1U << 7U))
#define RC522_RST_SetLow()				(dout &= (~(1U << 7U)))


/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI2_Init(void);
static void HW_DeInit(void);
static void HC595_Load(void);
static void Pause(uint32_t delay);
static void LED_RedBlink(uint8_t cnt);
static void ApplicationExe(void);
static void FLASH_Init(void);
static uint32_t FLASH_Erase(uint32_t StartSector);
static uint32_t FLASH_Write(uint32_t destination, uint32_t *p_source, uint32_t length);
static void SPI_FLASH_ReadPage(uint32_t address, uint8_t *data, uint16_t size);
static void FLASH_WriteFirmware(uint32_t fw_address, uint32_t fw_size);
static void EEPROM_WriteUpdateStatus(uint8_t status);

/* Program code   ------------------------------------------------------------*/
int main(void)
{
    uint8_t buf[8], status;
    uint32_t image_size;
    
    
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    FLASH_Init();    
	MX_I2C1_Init();
	MX_SPI2_Init();
    

	/**
    *   jump around
    */	
    while (1)
	{
		LED_HandmaidGreen_On();
        HC595_Load();
		Pause(500U);
        
		if(HAL_I2C_IsDeviceReady(&hi2c1, I2C_EE_WRITE, 1000U, 1000U) != HAL_OK)
		{
			LED_RedBlink(1);
			HW_DeInit();
			HAL_FLASH_OB_Launch();
		}
        
		buf[0] = EE_FW_UPDATE_STATUS >> 8U;
		buf[1] = EE_FW_UPDATE_STATUS & 0xFFU;
		HAL_I2C_Master_Transmit(&hi2c1, I2C_EE_WRITE, buf, 2U, I2C_EE_TIMEOUT);
		HAL_I2C_Master_Receive(&hi2c1, I2C_EE_READ, &status, 1U, I2C_EE_TIMEOUT);
        
		if (status == BOOTLOADER_CMD_RUN)
		{
			buf[0] = EE_FW_UPDATE_BYTE_CNT >> 8U;
			buf[1] = EE_FW_UPDATE_BYTE_CNT & 0xFFU;
			HAL_I2C_Master_Transmit(&hi2c1, I2C_EE_WRITE, buf, 2U, I2C_EE_TIMEOUT);
			HAL_I2C_Master_Receive(&hi2c1, I2C_EE_READ, &buf[2], 4U, I2C_EE_TIMEOUT);
			image_size = (buf[2] << 24) + (buf[3] << 16) + (buf[4] << 8) + buf[5];
            
            if((image_size > USER_FLASH_SIZE) || (image_size == 0U))
            {
                LED_RedBlink(2);
                HW_DeInit();
                HAL_FLASH_OB_Launch();
            }
            
			FLASH_Erase(APPLICATION_ADDRESS);
			FLASH_WriteFirmware(EE_NEW_FIRMWARE_START_ADDRESS, image_size);
			EEPROM_WriteUpdateStatus(BOOTLOADER_STATUS_UPDATE_SUCCESS);
		}
		else if(status == BOOTLOADER_STATUS_UPDATE_SUCCESS)
		{
            LED_RedBlink(3);
			FLASH_Erase(APPLICATION_ADDRESS);
            FLASH_WriteFirmware(EE_OLD_FIRMWARE_START_ADDRESS, USER_FLASH_SIZE);
            EEPROM_WriteUpdateStatus(BOOTLOADER_STATUS_UPDATE_FAIL);
		}
		else if(status == BOOTLOADER_STATUS_UPDATE_FAIL)
		{
			EEPROM_WriteUpdateStatus(BOOTLOADER_CMD_RUN);
			LED_HandmaidGreen_Off();
			
			while(1)
			{
                LED_RedBlink(1);
			}			
		}
		
		ApplicationExe();
    }  
}

static void Pause(uint32_t delay)
{
    uint32_t timer = HAL_GetTick();
    
    while((HAL_GetTick() - timer) < delay)
    {
       continue; 
    }
}


static void LED_RedBlink(uint8_t cnt)
{
    uint32_t t = cnt * 2U;
    
    LED_RoomStatusRed_Off();
    
    do
    {
        LED_RoomStatusRed_Toggle();
        HC595_Load();
        Pause(500U);
        --t;
    }
    while(t != 0U);
}    


static void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInit;
	
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSEState = RCC_HSI_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.LSEState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}


static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();
	/**
	*	preset gpio output pin
	*/
    HAL_GPIO_WritePin(GPIOA, DISPLAY_LED_PWM_Pin|DOUT_0_Pin|DOUT_1_Pin|DOUT_2_Pin|DOORLOCK_PWM_Pin|
                             SOUND_PWM_Pin|RS485_DIR_Pin|SHIFT_CLK_Pin, GPIO_PIN_RESET);	
    HAL_GPIO_WritePin(GPIOB, DISPLAY_CS_Pin|FLASH_CS_Pin, GPIO_PIN_SET);

    GPIO_InitStruct.Pin = 	DISPLAY_LED_PWM_Pin |
                            DOUT_0_Pin |
                            DOUT_1_Pin |
                            DOUT_2_Pin |
                            DOORLOCK_PWM_Pin |
                            SOUND_PWM_Pin |
                            RS485_DIR_Pin |
                            SHIFT_CLK_Pin;
    GPIO_InitStruct.Mode = 	GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(GPIOA, 	&GPIO_InitStruct);
	
    GPIO_InitStruct.Pin = 	DISPLAY_CS_Pin |
                            FLASH_CS_Pin;
    GPIO_InitStruct.Mode = 	GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(GPIOB, 	&GPIO_InitStruct);
}


static void MX_I2C1_Init(void)
{
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
    HAL_I2C_Init(&hi2c1);
}


static void MX_SPI2_Init(void)
{
    hspi2.Instance = SPI2;
    hspi2.Init.Mode = SPI_MODE_MASTER;
    hspi2.Init.Direction = SPI_DIRECTION_2LINES;
    hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi2.Init.NSS = SPI_NSS_SOFT;
    hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi2.Init.TIMode = SPI_TIMODE_DISABLED;
    hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
    hspi2.Init.CRCPolynomial = 10U;
    HAL_SPI_Init(&hspi2);
}


static void HC595_Load(void)
{
    HAL_SPI_Transmit(&hspi2, &dout, 1, 10);	
    HAL_GPIO_WritePin(SHIFT_CLK_Port, SHIFT_CLK_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SHIFT_CLK_Port, SHIFT_CLK_Pin, GPIO_PIN_SET);
}


static void ApplicationExe(void)
{
    if (((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0x2FFE0000U) == 0x20000000U)
    {
        HW_DeInit();
		jump_address = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4U);
		JumpToApplication = (pFunction) jump_address;
		__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
		JumpToApplication();
    }
}


static void HW_DeInit(void)
{
    HAL_GPIO_DeInit(GPIOA, DISPLAY_LED_PWM_Pin|DOUT_0_Pin|DOUT_1_Pin|DOUT_2_Pin|DOORLOCK_PWM_Pin|SOUND_PWM_Pin|RS485_DIR_Pin|SHIFT_CLK_Pin);
    HAL_GPIO_DeInit(GPIOB, DISPLAY_CS_Pin|FLASH_CS_Pin);
    HAL_I2C_MspDeInit(&hi2c1);
    HAL_SPI_MspDeInit(&hspi2);
    HAL_DeInit();
}    


static void FLASH_WriteFirmware(uint32_t fw_address, uint32_t fw_size)
{
    uint8_t buff[FLASH_BUFFER_SIZE];
    uint32_t bcnt, ram_source;
    uint32_t flash_destination = APPLICATION_ADDRESS;
    
    while(fw_size)
    {		
        if(fw_size >= FLASH_BUFFER_SIZE)
        {
            bcnt = FLASH_BUFFER_SIZE;
            fw_size -= FLASH_BUFFER_SIZE;
        }
        else
        {
            bcnt = fw_size;
            fw_size = 0U;
        }
        
        SPI_FLASH_ReadPage(fw_address, buff,  bcnt);				
        ram_source = (uint32_t) buff;
        FLASH_Write(flash_destination, (uint32_t*) ram_source, (bcnt / 4U));
        flash_destination += bcnt;
        fw_address += bcnt;
    }
}


static void EEPROM_WriteUpdateStatus(uint8_t status)
{
    uint8_t eebuff[4];
    
    eebuff[0] = EE_FW_UPDATE_STATUS >> 8U;
    eebuff[1] = EE_FW_UPDATE_STATUS & 0xFFU;
    eebuff[2] = status;
    HAL_I2C_Master_Transmit(&hi2c1, I2C_EE_WRITE, eebuff, 3, I2C_EE_TIMEOUT);
    HAL_I2C_IsDeviceReady(&hi2c1, I2C_EE_WRITE, I2C_EE_TRIALS, I2C_EE_TIMEOUT);
}


static void FLASH_Init(void)
{
    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
    HAL_FLASH_Lock();
}


static uint32_t FLASH_Erase(uint32_t start)
{
    uint32_t NbrOfPages = 0U;
    uint32_t PageError = 0U;
    FLASH_EraseInitTypeDef pEraseInit;
    HAL_StatusTypeDef status = HAL_OK;

    HAL_FLASH_Unlock();

    NbrOfPages = (USER_FLASH_END_ADDRESS - start)/FLASH_PAGE_SIZE;

    pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    pEraseInit.PageAddress = start;
    pEraseInit.Banks = FLASH_BANK_1;
    pEraseInit.NbPages = NbrOfPages;
    status = HAL_FLASHEx_Erase(&pEraseInit, &PageError);

    HAL_FLASH_Lock();

    if (status != HAL_OK)
    {
        return FLASHIF_ERASEKO;
    }

    return FLASHIF_OK;
}


static uint32_t FLASH_Write(uint32_t destination, uint32_t *p_source, uint32_t length)
{
    uint32_t i;
    
    HAL_FLASH_Unlock();

    for (i = 0U; (i < length) && (destination <= (USER_FLASH_END_ADDRESS - 4U)); i++)
    {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, destination, *(uint32_t*)(p_source + i)) == HAL_OK)      
        {
            if (*(uint32_t*)destination != *(uint32_t*)(p_source + i))
            {
                return(FLASHIF_WRITINGCTRL_ERROR);
            }
            destination += 4U;
        }
        else
        {
            return (FLASHIF_WRITING_ERROR);
        }
    }

    HAL_FLASH_Lock();

    return (FLASHIF_OK);
}


static void SPI_FLASH_ReadPage(uint32_t address, uint8_t *data, uint16_t size)
{
    uint8_t tx[4];
	
    tx[0] = SPI_EE_READ;
    tx[1] = (address >> 16U);
    tx[2] = (address >> 8U);
    tx[3] = (address & 0xFFU);    
    HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi2, tx, 4U, FLASH_EE_TIMEOUT);
    HAL_SPI_Receive(&hspi2, data, size, FLASH_EE_TIMEOUT);
    HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_SET);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
