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
#include "eeprom.h"

/* Variables  ----------------------------------------------------------------*/
extern uint16_t BACK_COLOR, POINT_COLOR;
 
uint8_t  aEepromBuffer[EE_BUFFER_SIZE];
uint8_t *p_spi_ee_buff;

extern void SPI1_Reinit(void);
/* Defines    ----------------------------------------------------------------*/

/* Objects    ----------------------------------------------------------------*/

/* Program code   ------------------------------------------------------------*/
uint8_t SPI_FLASH_ReadStatusRegister(void)
{
	uint8_t tx, rx;
	
	SPI_EE_CS_Low();
    tx = SPI_EE_READ_STATUS_REG_1;
    if (HAL_SPI_Transmit(&hspi1, &tx, 1, FLASH_EE_TIMEOUT) != HAL_OK) SPI1_Reinit();
	if (HAL_SPI_Receive(&hspi1, &rx, 1, FLASH_EE_TIMEOUT) != HAL_OK) SPI1_Reinit();
    SPI_EE_CS_High();
 
    return (rx);
    
}// End of read status register

void SPI_FLASH_WriteStatusRegister(uint8_t status)
{
	uint8_t tx[2];
	
	SPI_FLASH_WriteEnable();
    SPI_EE_CS_Low();
    tx[0] = SPI_EE_WRITE_STATUS_REG;
    tx[1] = status;
    if (HAL_SPI_Transmit(&hspi1, tx, 2, FLASH_EE_TIMEOUT) != HAL_OK) SPI1_Reinit();
    SPI_EE_CS_High();
    
}// End of write status register

uint32_t SPI_FLASH_ReadDeviceInfo(void)
{
	uint32_t dev_info;
	uint8_t tx;
	uint8_t rx[3];
	
	SPI_EE_CS_Low();
	tx = SPI_EE_JEDEC_ID;
	if (HAL_SPI_Transmit(&hspi1, &tx, 1, FLASH_EE_TIMEOUT) != HAL_OK) SPI1_Reinit();
	if (HAL_SPI_Receive(&hspi1, rx, 3, FLASH_EE_TIMEOUT) != HAL_OK) SPI1_Reinit();
	SPI_EE_CS_High();
	dev_info = (rx[0] << 24) + (rx[1] << 16) + (rx[2] << 8);
	return(dev_info);
}

void SPI_FLASH_WriteEnable(void)
{
    uint8_t tx;
   
    SPI_EE_CS_Low();
    tx = SPI_EE_WRITE_ENABLE;
    if (HAL_SPI_Transmit(&hspi1, &tx, 1, FLASH_EE_TIMEOUT) != HAL_OK) SPI1_Reinit();
    SPI_EE_CS_High();
	
}// End of write enable


void SPI_FLASH_Write_Disable(void)
{
    uint8_t tx;
	
    SPI_EE_CS_Low();
    tx = SPI_EE_WRITE_DISABLE;
    if (HAL_SPI_Transmit(&hspi1, &tx, 1, FLASH_EE_TIMEOUT) != HAL_OK) SPI1_Reinit();
    SPI_EE_CS_High();
           
}// End of write disable


uint8_t SPI_FLASH_ReadByte(uint32_t address)
{
    uint8_t tx[4];

    SPI_EE_CS_Low();
    tx[0] = SPI_EE_READ;
    tx[1] = address >> 16;
    tx[2] = address >> 8;
    tx[3] = address;  
    if (HAL_SPI_Transmit(&hspi1, tx, 4, FLASH_EE_TIMEOUT) != HAL_OK) SPI1_Reinit();
    if (HAL_SPI_Receive(&hspi1, tx, 1, FLASH_EE_TIMEOUT) != HAL_OK) SPI1_Reinit();
    SPI_EE_CS_High();
    return (tx[0]);

}// End of read byte from eeprom function

uint16_t SPI_FLASH_ReadInt(uint32_t address)
{
    uint16_t data;
    uint8_t tmp[4];

    SPI_EE_CS_Low();
    tmp[0] = SPI_EE_READ;
    tmp[1] = address >> 16;
    tmp[2] = address >> 8;
    tmp[3] = address;  
    if (HAL_SPI_Transmit(&hspi1, tmp, 4, FLASH_EE_TIMEOUT) != HAL_OK) SPI1_Reinit();
    if (HAL_SPI_Receive(&hspi1, tmp, 2, FLASH_EE_TIMEOUT) != HAL_OK) SPI1_Reinit();
    SPI_EE_CS_High();
    data = (tmp[0] << 8) + tmp[1];
    return (data);
    
}// End of read int from eeprom function

void SPI_FLASH_WriteByte(uint32_t address, uint8_t data)
{
    uint8_t tx[5];

    SPI_FLASH_WriteEnable();    
    tx[0] = SPI_EE_PAGE_PGM;
    tx[1] = address >> 16;
    tx[2] = address >> 8;
    tx[3] = address;
    tx[4] = data;    
    SPI_EE_CS_Low();    
    if (HAL_SPI_Transmit(&hspi1, &tx[0], 5, FLASH_EE_TIMEOUT) != HAL_OK) SPI1_Reinit();
    SPI_EE_CS_High();
    
}// End of write byte to eeprom function

void SPI_FLASH_WriteInt(uint32_t address, uint16_t data)
{
    uint8_t tx[6];
	
	tx[0] = SPI_EE_PAGE_PGM;
    tx[1] = address >> 16;
    tx[2] = address >> 8;
    tx[3] = address;
    tx[4] = data >> 8;
    tx[5] = data;
    SPI_FLASH_WriteEnable();
    SPI_EE_CS_Low();    
    if (HAL_SPI_Transmit(&hspi1, &tx[0], 6, FLASH_EE_TIMEOUT) != HAL_OK) SPI1_Reinit();
    SPI_EE_CS_High();
    
}// End of write int to eeprom function

EEPROM_StatusTypeDef SPI_FLASH_ReleasePowerDown(void)
{
    uint8_t tx;
    
    SPI_EE_CS_Low();
    tx = SPI_EE_RESUME_POWER_DOWN;
    if (HAL_SPI_Transmit(&hspi1, &tx, 1, FLASH_EE_TIMEOUT) != HAL_OK) SPI1_Reinit();
    SPI_EE_CS_High();
    HAL_Delay(1);
	if((SPI_FLASH_ReadDeviceInfo() >> 24) != FLASH_EE_MANUFACTURER_ID) return(EE_ERROR);
    return (EE_OK);
    
}// End of release from power down

EEPROM_StatusTypeDef SPI_FLASH_Erase(uint32_t address, uint8_t erase_type)
{
    uint8_t tx[4];
	
    tx[0] = erase_type;
    tx[1] = address >> 16;
    tx[2] = address >> 8;
    tx[3] = address;
	
    SPI_FLASH_WriteEnable();
    SPI_EE_CS_Low();
    
    if((erase_type  == SPI_EE_CHIP_ERASE) || (erase_type  == SPI_EE_CHIP_ERASE_2))
	{        
        if (HAL_SPI_Transmit(&hspi1, tx, 1, FLASH_EE_TIMEOUT) != HAL_OK) SPI1_Reinit();       
    }
	else if ((erase_type  == SPI_EE_4K_BLOCK_ERASE) || (erase_type  == SPI_EE_32K_BLOCK_ERASE) || (erase_type  == SPI_EE_64K_BLOCK_ERASE))
	{    
        if (HAL_SPI_Transmit(&hspi1, tx, 4, FLASH_EE_TIMEOUT) != HAL_OK) SPI1_Reinit();        
    } 
	else
	{
        SPI_EE_CS_High();
        return(EE_ERROR);         
    }
    
    SPI_EE_CS_High();
    return (EE_OK);
    
}// End of erase function   
                
void SPI_FLASH_WritePage(uint32_t address, uint8_t *data, uint16_t size)
{
    uint8_t tx[4];
	
    tx[0] = SPI_EE_PAGE_PGM;
    tx[1] = address >> 16;
    tx[2] = address >> 8;
    tx[3] = address;	
    SPI_FLASH_WriteEnable();
    SPI_EE_CS_Low();
    if (HAL_SPI_Transmit(&hspi1, tx, 4, FLASH_EE_TIMEOUT) != HAL_OK) SPI1_Reinit();
    if (HAL_SPI_Transmit(&hspi1, data, size, FLASH_EE_TIMEOUT) != HAL_OK) SPI1_Reinit();
    SPI_EE_CS_High();
    
}// End of write data

void SPI_FLASH_ReadPage(uint32_t address, uint8_t *data, uint16_t size)
{
    uint8_t tx[4];
	
    tx[0] = SPI_EE_READ;
    tx[1] = address >> 16;
    tx[2] = address >> 8;
    tx[3] = address;    
    SPI_EE_CS_Low();
    if (HAL_SPI_Transmit(&hspi1, tx, 4, FLASH_EE_TIMEOUT) != HAL_OK) SPI1_Reinit();
    if (HAL_SPI_Receive(&hspi1, data, size, FLASH_EE_TIMEOUT) != HAL_OK) SPI1_Reinit();
    SPI_EE_CS_High();
    
}// Eprom read data

uint8_t SPI_FLASH_ReadSectorProtectionRegister(uint32_t address)
{
    uint8_t tx[4];
	
    tx[0] = SPI_EE_READ_SECTOR_PROT;
    tx[1] = address >> 16;
    tx[2] = address >> 8;
    tx[3] = address;	
	SPI_EE_CS_Low();
	if (HAL_SPI_Transmit(&hspi1, tx, 4, FLASH_EE_TIMEOUT) != HAL_OK) SPI1_Reinit();
	if (HAL_SPI_Receive(&hspi1, tx, 1, FLASH_EE_TIMEOUT) != HAL_OK) SPI1_Reinit();
    SPI_EE_CS_High();
    return (tx[0]);
}

void SPI_FLASH_UnprotectSector(uint32_t address)
{
    uint8_t tx[4];
    
	tx[0] = SPI_EE_UNPROTECT_SECTOR;
    tx[1] = address >> 16;
    tx[2] = address >> 8;
    tx[3] = address;
	SPI_FLASH_WriteEnable();
	SPI_EE_CS_Low();
    if (HAL_SPI_Transmit(&hspi1, tx, 4, FLASH_EE_TIMEOUT) != HAL_OK) SPI1_Reinit();
    SPI_EE_CS_High();
}

void SPI_FLASH_ProtectSector(uint32_t address)
{
    uint8_t tx[4];
    
	tx[0] = SPI_EE_PROTECT_SECTOR;
    tx[1] = ((address & 0xff0000) >> 16);
    tx[2] = ((address & 0xFF00) >> 8);
    tx[3] = (address & 0xFF);
	SPI_FLASH_WriteEnable();
	SPI_EE_CS_Low();
    if (HAL_SPI_Transmit(&hspi1, &tx[0], 4, FLASH_EE_TIMEOUT) != HAL_OK) SPI1_Reinit();
    SPI_EE_CS_High();
}


/******************************   END OF FILE  **********************************/
