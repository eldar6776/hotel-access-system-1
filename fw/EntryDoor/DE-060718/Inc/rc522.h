/**
 ******************************************************************************
 * File Name          : rc522.h
 * Date               : 08/05/2016 23:15:16
 * Description        : mifare RC522 modul header
 ******************************************************************************
 *
 *
 ******************************************************************************
 */
 
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RC522_H__
#define __RC522_H__					060718	// version


/* Include  ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"


/* Exported Type  ------------------------------------------------------------*/
/* Exported Define  ----------------------------------------------------------*/
/* Exported types    ---------------------------------------------------------*/
typedef struct
{
	uint8_t card_status;
    uint8_t user_group;
    uint16_t system_id;
    uint16_t controller_id;
	uint8_t expiry_time[6];
	uint8_t card_id[5];
	
}RC522_CardDataTypeDef;

extern RC522_CardDataTypeDef sCard;


/* Exported variables  -------------------------------------------------------*/
/* Exported macros     -------------------------------------------------------*/
/* Exported functions  -------------------------------------------------------*/
void RC522_Init(void);
void RC522_Service(void);


#endif
/******************************   END OF FILE  **********************************/
