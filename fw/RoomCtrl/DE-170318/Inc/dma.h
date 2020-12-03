/**
 ******************************************************************************
 * File Name          : dma.h
 * Date               : 28/02/2016 23:16:19
 * Description        : dma header file
 ******************************************************************************
 */
 
 
 /* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DMA_H__
#define __DMA_H__					170318	// version


/* Include  ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"


/* Exporeted Types   ---------------------------------------------------------*/
/* Exporeted Define   --------------------------------------------------------*/
/* Exporeted Variable   ------------------------------------------------------*/

#define dmaWait() while(SPI_I2S_GetFlagStatus(SPI_MASTER,SPI_I2S_FLAG_BSY) == SET);

#define dmaStartRx() DMA_Init(DMA1_Channel2, &dmaStructure); \
    DMA_Cmd(DMA1_Channel2, ENABLE);

#define dmaStartTx() DMA_Init(DMA1_Channel3, &dmaStructure); \
    DMA_Cmd(DMA1_Channel3, ENABLE);

void dmaInit(void);
void dmaSendDataBuf16(void);
void dmaSendCmd(uint8_t cmd);
void dmaSendCmdCont(uint8_t cmd);
void dmaReceiveDataCont8(uint8_t *data);
void dmaSendData8(uint8_t *data, uint32_t n);
void dmaSendData16(uint16_t *data, uint32_t n);
void dmaSendDataCont8(uint8_t *data, uint32_t n);
void dmaSendDataCont16(uint16_t *data, uint32_t n);
void dmaSendDataContBuf16(uint16_t *data, uint32_t n);
void dmaFill16(uint16_t color, uint32_t n);

#endif //TEST1_DMA_H
