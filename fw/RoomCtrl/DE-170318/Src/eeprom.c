/**
 ******************************************************************************
 * File Name          : eeprom.c
 * Date               : 28/02/2016 23:16:19
 * Description        : eeprom memory manager modul 
 ******************************************************************************
 *
 *
 *
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "eeprom.h"
#include "common.h"
#include "dio.h"
#include "rc522.h"
#include "one_wire.h"
#include "rs485.h"
#include "display.h"
#include "signal.h"


/* Variables  ----------------------------------------------------------------*/
static uint8_t spi_buff[8];


/* Macros   ------------------------------------------------------------------*/
/* Function prototypes    ----------------------------------------------------*/
static uint8_t SPI_FLASH_ReadStatusRegister(void);
static uint8_t SPI_FLASH_ReadDeviceInfo(void);
static void SPI_FLASH_WriteEnable(void);


/* Program code   ------------------------------------------------------------*/
uint8_t SPI_FLASH_ReadByte(uint32_t address)
{
    HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_RESET);
    spi_buff[0] = SPI_FLASH_READ;
    spi_buff[1] = address >> 16U;
    spi_buff[2] = address >> 8U;
    spi_buff[3] = address; 
    spi_buff[4] = 0U;
    if (HAL_SPI_Transmit(&hspi2, spi_buff, 4U, SPI_FLASH_TIMEOUT) != HAL_OK) Error_Handler();
    if (HAL_SPI_Receive(&hspi2, &spi_buff[4], 1U, SPI_FLASH_TIMEOUT) != HAL_OK) Error_Handler();
    HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_SET);
    return (spi_buff[4]);
}


uint16_t SPI_FLASH_ReadInt(uint32_t address)
{
    uint16_t data;

    HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_RESET);
    spi_buff[0] = SPI_FLASH_READ;
    spi_buff[1] = address >> 16U;
    spi_buff[2] = address >> 8U;
    spi_buff[3] = address;
    spi_buff[4] = 0U;
    spi_buff[5] = 0U;
    if (HAL_SPI_Transmit(&hspi2, spi_buff, 4U, SPI_FLASH_TIMEOUT) != HAL_OK) Error_Handler();
    if (HAL_SPI_Receive(&hspi2, &spi_buff[4], 2U, SPI_FLASH_TIMEOUT) != HAL_OK) Error_Handler();
    HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_SET);
    data = (spi_buff[4] << 8U) + spi_buff[5];
    return (data);
}


void SPI_FLASH_ReadPage(uint32_t address, uint8_t *data, uint16_t size)
{
    HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_RESET);
    spi_buff[0] = SPI_FLASH_READ;
    spi_buff[1] = address >> 16U;
    spi_buff[2] = address >> 8U;
    spi_buff[3] = address;
    if (HAL_SPI_Transmit(&hspi2, spi_buff, 4U, SPI_FLASH_TIMEOUT) != HAL_OK) Error_Handler();
    if (HAL_SPI_Receive(&hspi2, data, size, SPI_FLASH_TIMEOUT) != HAL_OK) Error_Handler();
    HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_SET);
}


void SPI_FLASH_WriteByte(uint32_t address, uint8_t data)
{
    SPI_FLASH_WriteEnable();    
    spi_buff[0] = SPI_FLASH_PAGE_PGM;
    spi_buff[1] = address >> 16;
    spi_buff[2] = address >> 8;
    spi_buff[3] = address;
    spi_buff[4] = data;    
    HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi2, spi_buff, 5, SPI_FLASH_TIMEOUT) != HAL_OK) Error_Handler();
    HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_SET);
}


void SPI_FLASH_WriteInt(uint32_t address, uint16_t data)
{
	SPI_FLASH_WriteEnable();
	spi_buff[0] = SPI_FLASH_PAGE_PGM;
    spi_buff[1] = address >> 16;
    spi_buff[2] = address >> 8;
    spi_buff[3] = address;
    spi_buff[4] = data >> 8;
    spi_buff[5] = data;
    HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_RESET);   
    if (HAL_SPI_Transmit(&hspi2, spi_buff, 6, SPI_FLASH_TIMEOUT) != HAL_OK) Error_Handler();
    HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_SET);
}


void SPI_FLASH_WritePage(uint32_t address, uint8_t *data, uint16_t size)
{
    SPI_FLASH_WriteEnable();
    HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_RESET);
    spi_buff[0] = SPI_FLASH_PAGE_PGM;
    spi_buff[1] = address >> 16U;
    spi_buff[2] = address >> 8U;
    spi_buff[3] = address;
    if (HAL_SPI_Transmit(&hspi2, spi_buff, 4U, SPI_FLASH_TIMEOUT) != HAL_OK) Error_Handler();
    if (HAL_SPI_Transmit(&hspi2, data, size, SPI_FLASH_TIMEOUT) != HAL_OK) Error_Handler();
    HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_SET);
}


void SPI_FLASH_WriteStatusRegister(uint8_t status)
{
	SPI_FLASH_WriteEnable();
    HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_RESET);
    spi_buff[0] = SPI_FLASH_WRITE_STATUS_REG;
    spi_buff[1] = status;
    if (HAL_SPI_Transmit(&hspi2, spi_buff, 2U, SPI_FLASH_TIMEOUT) != HAL_OK) Error_Handler();
    HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_SET);
}


void SPI_FLASH_UnprotectSector(uint32_t address)
{
    SPI_FLASH_WriteEnable();
	HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_RESET);
    spi_buff[0] = SPI_FLASH_UNPROTECT_SECTOR;
    spi_buff[1] = address >> 16U;
    spi_buff[2] = address >> 8U;
    spi_buff[3] = address;
    if (HAL_SPI_Transmit(&hspi2, spi_buff, 4U, SPI_FLASH_TIMEOUT) != HAL_OK) Error_Handler();
    HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_SET);
}


uint8_t SPI_FLASH_WaitReadyStatus(uint32_t timeout)
{
    uint32_t tickstart = HAL_GetTick();
    
    while((HAL_GetTick() - tickstart) < timeout)
    {
        if((SPI_FLASH_ReadStatusRegister() & SPI_FLASH_STATUS_BUSY_MASK) == 0U)
        {
            return (1U);
        }
#ifndef	USE_DEBUGGER
        HAL_IWDG_Refresh(&hiwdg);
#endif
    }
    
    return (0U);
}


uint8_t SPI_FLASH_ReleasePowerDown(void)
{
    HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_RESET);
    spi_buff[0] = SPI_FLASH_RESUME_POWER_DOWN;
    if (HAL_SPI_Transmit(&hspi2, spi_buff, 1U, SPI_FLASH_TIMEOUT) != HAL_OK) Error_Handler();
    HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
    return (SPI_FLASH_ReadDeviceInfo());
}
/*************************************************************************/
/**         S A V E         V A L U E       T O     E E P R O M         **/
/*************************************************************************/
void EEPROM_Save(uint16_t ee_address, uint8_t* value, uint8_t size)
{
    uint8_t ee_buff[32];
    uint32_t t = 2U;
    
    ee_buff[0] = (ee_address >> 8U);
    ee_buff[1] = (ee_address & 0xFFU);
    
    while(size)
    {
       ee_buff[t] = *value;
        --size;
        ++value;
        ++t;
    }
    
    if (HAL_I2C_Master_Transmit(&hi2c1, I2C_EE_WRITE, ee_buff, t, I2C_EE_TIMEOUT) != HAL_OK) Error_Handler();
    if (HAL_I2C_IsDeviceReady(&hi2c1, I2C_EE_READ, I2C_EE_TRIALS, I2C_EE_WRITE_DELAY) != HAL_OK) Error_Handler();
}


void SPI_FLASH_Erase(uint32_t address, uint8_t erase_type)
{
    SPI_FLASH_WriteEnable();
    HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_RESET);
    spi_buff[0] = erase_type;
    spi_buff[1] = address >> 16U;
    spi_buff[2] = address >> 8U;
    spi_buff[3] = address;
    
    if((erase_type  == SPI_FLASH_CHIP_ERASE) || (erase_type  == SPI_FLASH_CHIP_ERASE_2))
	{
        if (HAL_SPI_Transmit(&hspi2, spi_buff, 1U, SPI_FLASH_TIMEOUT) != HAL_OK) Error_Handler();     
    }
	else if ((erase_type  == SPI_FLASH_4K_BLOCK_ERASE) || (erase_type  == SPI_FLASH_32K_BLOCK_ERASE) || (erase_type  == SPI_FLASH_64K_BLOCK_ERASE))
	{
        if (HAL_SPI_Transmit(&hspi2, spi_buff, 4U, SPI_FLASH_TIMEOUT) != HAL_OK) Error_Handler();
    }
    
    HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_SET);
}


static uint8_t SPI_FLASH_ReadStatusRegister(void)
{
	HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_RESET);
    spi_buff[0] = SPI_FLASH_READ_STATUS_REG_1;
    spi_buff[1] = 0U;
    if (HAL_SPI_Transmit(&hspi2, spi_buff, 1U, SPI_FLASH_TIMEOUT) != HAL_OK) Error_Handler();
	if (HAL_SPI_Receive(&hspi2, &spi_buff[1], 1U, SPI_FLASH_TIMEOUT) != HAL_OK) Error_Handler();
    HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_SET);
    return (spi_buff[1]);
}


static uint8_t SPI_FLASH_ReadDeviceInfo(void)
{
	HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_RESET);
	spi_buff[0] = SPI_FLASH_JEDEC_ID;
    spi_buff[1] = 0U;
    spi_buff[2] = 0U;
    spi_buff[3] = 0U;
	if (HAL_SPI_Transmit(&hspi2, spi_buff, 1U, SPI_FLASH_TIMEOUT) != HAL_OK) Error_Handler();
	if (HAL_SPI_Receive(&hspi2, &spi_buff[1], 3U, SPI_FLASH_TIMEOUT) != HAL_OK) Error_Handler();
	HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_SET);
	if (((spi_buff[1] << 16U) + (spi_buff[2] << 8U) + spi_buff[3]) == SPI_FLASH_MANUFACTURER_WINBOND) return(1U);
	else return(0U);
}


static void SPI_FLASH_WriteEnable(void)
{
    HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_RESET);
    spi_buff[0] = SPI_FLASH_WRITE_ENABLE;
    if (HAL_SPI_Transmit(&hspi2, spi_buff, 1U, SPI_FLASH_TIMEOUT) != HAL_OK) Error_Handler();
    HAL_GPIO_WritePin(FLASH_CS_Port, FLASH_CS_Pin, GPIO_PIN_SET);
}



/******************************   END OF FILE  **********************************/
