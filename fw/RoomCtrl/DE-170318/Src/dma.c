/**
 ******************************************************************************
 * File Name          : dma.c
 * Date               : 28/02/2016 23:16:19
 * Description        : dma  modul
 ******************************************************************************
 */
 
 
/* Include  ------------------------------------------------------------------*/
#include "main.h"
#include "common.h"
#include "dma.h"
#include "dio.h"
#include "eeprom.h"
#include "signal.h"
#include "tft_lcd.h"
#include "rs485.h"
#include "rc522.h"
#include "display.h"



#define DMA_BUF_SIZE 2048U
uint16_t dmaBufIndex = 0U;
uint16_t dmaBuffer[DMA_BUF_SIZE];


inline void DMA_ReceiveBytes(uint8_t *data, uint32_t n) 
{

    HAL_DMA_Start(&hdma_spi2_tx, (uint32_t)&hspi2.Instance->DR, (uint32_t) data , n);
}

inline void DMA_SendBytes(uint8_t *data, uint32_t n) 
{

    HAL_DMA_Start(&hdma_spi2_tx, (uint32_t) data, (uint32_t)&hspi2.Instance->DR, n);
    
}

inline void DMA_SendCircular16(uint16_t *data, uint32_t n) 
{
//    DMA_StructInit(&dmaStructure);
//    dmaStructure.DMA_PeripheralBaseAddr = (uint32_t) &(SPI_MASTER->DR);
//    dmaStructure.DMA_Priority           = DMA_Priority_Medium;

//    dmaStructure.DMA_MemoryBaseAddr = (uint32_t) data;
//    dmaStructure.DMA_BufferSize     = n;

//    dmaStructure.DMA_Mode               = DMA_Mode_Circular;
//    dmaStructure.DMA_MemoryInc          = DMA_MemoryInc_Disable;
//    dmaStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
//    dmaStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
//    dmaStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;

//    dmaStartTx();
}

inline void DMA_SendInt(uint16_t *data, uint32_t n) 
{
//    DMA_StructInit(&dmaStructure);
//    dmaStructure.DMA_PeripheralBaseAddr = (uint32_t) &(SPI_MASTER->DR);
//    dmaStructure.DMA_Priority           = DMA_Priority_Medium;

//    dmaStructure.DMA_MemoryBaseAddr = (uint32_t) data;
//    dmaStructure.DMA_BufferSize     = n;

//    dmaStructure.DMA_Mode               = DMA_Mode_Normal;
//    dmaStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
//    dmaStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
//    dmaStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
//    dmaStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;

//    dmaStartTx();
}

//</editor-fold>

//<editor-fold desc="DMA send receive functions">

inline void DMA_SendCmd(uint8_t cmd) 
{
//    TFT_CS_RESET;
//    TFT_DC_RESET;
//    dmaSend8(&cmd, 1);
//    dmaWait();
//    TFT_CS_SET;
}

inline void DMA_SendCmdCont(uint8_t cmd) 
{
//    TFT_DC_RESET;
//    dmaSend8(&cmd, 1);
//    dmaWait();
}

inline void DMA_ReceiveByteDataCont(uint8_t *data)    
{
//    uint8_t dummy = 0xFF;
//    dmaSend8(&dummy, 1);
//    dmaReceive8(data, 1);
//    dmaWait();
}

inline void DMA_SendByteData(uint8_t *data, uint32_t n) 
{
//    TFT_CS_RESET;
//    TFT_DC_SET;
//    dmaSend8(data, n);
//    dmaWait();
//    TFT_CS_SET;
}

inline void DMA_SendByteDataCont(uint8_t *data, uint32_t n) 
 {
//    TFT_DC_SET;
//    dmaSend8(data, n);
//    dmaWait();
}

inline void DMA_SendIntData(uint16_t *data, uint32_t n) 
{
//    TFT_CS_RESET;
//    TFT_DC_SET;
//    dmaSend16(data, n);
//    dmaWait();
//    TFT_CS_SET;
}

inline void DMA_SendDataCont16(uint16_t *data, uint32_t n) 
{
//    TFT_DC_SET;
//    dmaSend16(data, n);
//    dmaWait();
}

inline void DMA_SendIntDataBuff() 
{
//    if(dmaBufIndex == 0) return;
//    TFT_DC_SET;
//    dmaSend16(dmaBuffer, dmaBufIndex);
//    dmaBufIndex = 0;
//    dmaWait();
}

inline void DMA_SendIntDataContBuff(uint16_t *data, uint32_t n) 
{
//    while (n--) 
//    {
//        dmaBuffer[dmaBufIndex] = *data++;
//        if (dmaBufIndex == DMA_BUF_SIZE - 1) {
//            dmaSendDataBuf16();
//        }
//        dmaBufIndex++;
//    }
}


inline void DMA_SendIntDataCircular(uint16_t *data, uint32_t n) 
{
//    TFT_DC_SET;
//    dmaSendCircular16(data, n);
//    dmaWait();
}

//</editor-fold>

inline void DMA_IntFill(uint16_t color, uint32_t n) 
{
//    TFT_CS_RESET;
//    dmaSendCmdCont(LCD_GRAM);
//    while (n != 0) {
//        uint16_t ts = (uint16_t) (n > UINT16_MAX ? UINT16_MAX : n);
//        dmaSendDataCircular16(&color, ts);
//        n -= ts;
//    }
//    TFT_CS_SET;
}

//<editor-fold desc="IRQ handlers">

void DMA1_Channel2_IRQHandler(void) 
{
//    if (DMA_GetITStatus(DMA1_IT_TC2) == SET) 
//    {
//        DMA_Cmd(DMA1_Channel2, DISABLE);
//        DMA_ClearITPendingBit(DMA1_IT_TC2);
//    }
}


void DMA1_Channel3_IRQHandler(void) 
{
//    if (DMA_GetITStatus(DMA1_IT_TC3) == SET) 
//    {
//        DMA_Cmd(DMA1_Channel3, DISABLE);
//        DMA_ClearITPendingBit(DMA1_IT_TC3);
//    }
}

//</editor-fold>
