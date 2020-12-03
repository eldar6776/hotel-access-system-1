/**
  ******************************************************************************
  * @file    common.c 
  * @author  Mi
  * @version 1.0.0
  * @date    9-Septembar-2015
  * @brief   konverteri stinga u hex i int i obratno
  ******************************************************************************
  */ 

/** @addtogroup STM32F1xx_IAP_Main
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "common.h"

/* Imported variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
void Delay(__IO uint32_t nCount)
{
	__IO uint32_t index = 0;
	
	for(index = 80000*nCount; index != 0; index--)
	{
	}
}

int GetLenght(char * p) 
{
  int Len;
  
  for (Len = 0; *p++; Len++);
  return Len;
}

void Str2Hex(uint8_t *p_str, uint16_t lenght, uint8_t *p_hex)
{
	uint32_t hex_cnt = 0, str_cnt = 0;
	
	while(lenght)
	{
		if((p_str[str_cnt] - 48) > 9) p_hex[hex_cnt] = CONVERTHEX_ALPHA(p_str[str_cnt]) << 4;
		else p_hex[hex_cnt] = CONVERTDEC(p_str[str_cnt]) << 4;
		++str_cnt;
		if((p_str[str_cnt] - 48) > 9) p_hex[hex_cnt] += CONVERTHEX_ALPHA(p_str[str_cnt]);
		else p_hex[hex_cnt] += CONVERTDEC(p_str[str_cnt]);
		++str_cnt;
		++hex_cnt;
		--lenght;
	}
}

void Hex2Str(uint8_t *p_hex, uint16_t lenght, uint8_t *p_str)
{
	uint32_t hex_cnt = 0, str_cnt = 0;
	
	while(lenght)
	{
		if((p_hex[hex_cnt] >> 4) > 9) p_str[str_cnt] = (p_hex[hex_cnt] >> 4) + 55; // convert to upper ascii letter
		else p_str[str_cnt] = (p_hex[hex_cnt] >> 4) + 48;
		++str_cnt;
		if((p_hex[hex_cnt] & 0x0f) > 9) p_str[str_cnt] = (p_hex[hex_cnt] & 0x0f) + 55; // convert to upper ascii letter
		else p_str[str_cnt]  = (p_hex[hex_cnt]& 0x0f) + 48;
		++str_cnt;
		++hex_cnt;
		--lenght;
	}
}	
void Int2Str(uint8_t *p_str, uint32_t intnum)
{
  uint32_t i, divider = 1000000000, pos = 0, status = 0;

  for (i = 0; i < 10; i++)
  {
    p_str[pos++] = (intnum / divider) + 48;

    intnum = intnum % divider;
    divider /= 10;
    if ((p_str[pos-1] == '0') & (status == 0))
    {
      pos = 0;
    }
    else
    {
      status++;
    }
  }
}


uint32_t Str2Int(uint8_t *p_inputstr, uint32_t *p_intnum)
{
  uint32_t i = 0, res = 0;
  uint32_t val = 0;

  if ((p_inputstr[0] == '0') && ((p_inputstr[1] == 'x') || (p_inputstr[1] == 'X')))
  {
    i = 2;
    while ( ( i < 11 ) && ( p_inputstr[i] != '\0' ) )
    {
      if (ISVALIDHEX(p_inputstr[i]))
      {
        val = (val << 4) + CONVERTHEX(p_inputstr[i]);
      }
      else
      {
        /* Return 0, Invalid input */
        res = 0;
        break;
      }
      i++;
    }

    /* valid result */
    if (p_inputstr[i] == '\0')
    {
      *p_intnum = val;
      res = 1;
    }
  }
  else /* max 10-digit decimal input */
  {
    while ( ( i < 11 ) && ( res != 1 ) )
    {
      if (p_inputstr[i] == '\0')
      {
        *p_intnum = val;
        /* return 1 */
        res = 1;
      }
      else if (((p_inputstr[i] == 'k') || (p_inputstr[i] == 'K')) && (i > 0))
      {
        val = val << 10;
        *p_intnum = val;
        res = 1;
      }
      else if (((p_inputstr[i] == 'm') || (p_inputstr[i] == 'M')) && (i > 0))
      {
        val = val << 20;
        *p_intnum = val;
        res = 1;
      }
      else if (ISVALIDDEC(p_inputstr[i]))
      {
        val = val * 10 + CONVERTDEC(p_inputstr[i]);
      }
      else
      {
        /* return 0, Invalid input */
        res = 0;
        break;
      }
      i++;
    }
  }

  return res;
}


void ClearBuffer(uint8_t * buffer, uint16_t size)
{

	while(size) 
    {
        *buffer = 0;
        ++buffer;
        --size;
    }
}


void ClearBuffer16(uint16_t * buffer, uint16_t size)
{
	uint16_t bcnt;
	
	bcnt = 0;	
	while(bcnt < size) buffer[bcnt++] = 0x0000;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

