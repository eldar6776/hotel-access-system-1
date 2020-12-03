/**
  ******************************************************************************
  * @file    IAP_Main/Src/common.c 
  * @author  MCD Application Team
  * @version 1.0.0
  * @date    8-April-2015
  * @brief   This file provides all the common functions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
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

/** @addtogroup STM32F1xx_IAP_Main
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "common.h"


/* Imported variables --------------------------------------------------------*/
extern UART_HandleTypeDef huart1;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
ROOM_StatusTypeDef ROOM_Status = ROOM_READY;
ROOM_StatusTypeDef ROOM_OldStatus = ROOM_IDLE;

/* Private functions ---------------------------------------------------------*/
//void to_bin(unsigned char c, char *out) {
//  *(unsigned long long*)out = 3472328296227680304ULL +
//    (((c * 9241421688590303745ULL) / 128) & 72340172838076673ULL);
//}
void ClearBuffer(uint8_t *buffer, uint16_t size)
{
	uint16_t bcnt = 0;
	
	while(bcnt < size) buffer[bcnt++] = NULL;
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

void Int2StrSized(uint8_t *p_str, uint32_t intnum, uint8_t size)
{
	uint8_t result[16];
	uint32_t i, divider = 1000000000, pos = 0;

	ClearBuffer(result, 16);
	
	for (i = 0; i < 10; i++)
	{
		result[pos++] = (intnum / divider) + 48;
		intnum = intnum % divider;
		divider /= 10;
	}
	
	pos -= 1;
	
	while(size && pos)
	{
		p_str[size - 1] = result[pos];
		size--;
		pos--;
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

uint8_t Bcd2Dec(uint8_t bcd)
{
	return(((bcd >> 4) * 10) + (bcd & 0x0f));
}

uint8_t Dec2Bcd(uint8_t dec)
{
	uint8_t ret;
	
	ret = 0;
	while(dec > 9)
	{
		dec -= 10;
		ret++;
	}
	
	ret = ret << 4;
	ret += dec;
	return (ret);
}

void Serial_PutString(uint8_t *p_string)
{   
    uint16_t length = 0;

    while (p_string[length] != '\0'){
      
        length++;
        
    }// End of while loop

//	__HAL_UART_DISABLE_IT(&huart1, UART_IT_PE);
//    __HAL_UART_DISABLE_IT(&huart1, UART_IT_ERR);
//    __HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
//	//__HAL_UART_FLUSH_DRREGISTER(&huart1);
//	__HAL_UNLOCK(&huart1);
//	huart1.State = HAL_UART_STATE_READY;
	
//	RS485_DirTx();
//	RS485_DirTx();	
//	RS485_StartTimer(PACKET_TRANSFER_TIMEOUT);
//	HAL_UART_Transmit(&huart1, p_string, length, PACKET_TRANSFER_TIMEOUT);
//	while(HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY) continue;	
//	RS485_StopTimer(); 
//    RS485_DirRx();
	
//	__HAL_UART_ENABLE_IT(&huart1, UART_IT_PE);
//	__HAL_UART_ENABLE_IT(&huart1, UART_IT_ERR);
//	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	
}// End of send string function

void Serial_PutByte( uint8_t param )
{ 
//	__HAL_UART_DISABLE_IT(&huart1, UART_IT_PE);
//    __HAL_UART_DISABLE_IT(&huart1, UART_IT_ERR);
//    __HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
//	//__HAL_UART_FLUSH_DRREGISTER(&huart1);
//	__HAL_UNLOCK(&huart1);
//	huart1.State = HAL_UART_STATE_READY;
//	
//    RS485_DirTx();
//	RS485_DirTx();
//	RS485_StartTimer(PACKET_TRANSFER_TIMEOUT);
//    HAL_UART_Transmit(&huart1, &param, 1, PACKET_TRANSFER_TIMEOUT);	
//	while(HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY) continue;
//	RS485_StopTimer();
//	RS485_DirRx();  
//	
//	__HAL_UART_ENABLE_IT(&huart1, UART_IT_PE);
//	__HAL_UART_ENABLE_IT(&huart1, UART_IT_ERR);
//	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	
}// End of send byte to serial function
/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
