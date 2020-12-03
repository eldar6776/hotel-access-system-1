/**
 ******************************************************************************
 * File Name          : common.c
 * Date               : 10.3.2018
 * Description        : usefull function set
 ******************************************************************************
 *
 *
 ******************************************************************************
 */
 

/* Include  ------------------------------------------------------------------*/
#include "main.h"
#include "eeprom.h"
#include "common.h"
#include "dio_interface.h"
#include "logger.h"
#include "rc522.h"
#include "one_wire.h"
#include "rs485.h"
#include "display.h"
#include "signal.h"


/* Imported Type  ------------------------------------------------------------*/
/* Imported Variable  --------------------------------------------------------*/
/* Imported Function  --------------------------------------------------------*/
/* Private Type --------------------------------------------------------------*/
/* Private Define ------------------------------------------------------------*/
/* Private Variable ----------------------------------------------------------*/
/* Private Macro -------------------------------------------------------------*/
/* Private Function Prototype ------------------------------------------------*/
/* Program Code  -------------------------------------------------------------*/
uint8_t Bcd2Dec(uint8_t bcd)
{
	return(((bcd >> 4U) * 10U) + (bcd & 0x0fU));
}


uint8_t Dec2Bcd(uint8_t dec)
{
	uint32_t r = 0U;
    
	while(dec > 9U)
	{
		dec -= 10U;
		++r;
	}

	r <<= 4U;
	r += dec;
	return (r);
}


uint8_t CalcCRC(uint8_t *buff, uint8_t size)
{
	uint32_t g = 0U;
    uint32_t h = size;
 
    do
    {
        g += *buff;
        ++buff;
        --h;
        
    }while(h != 0U);

	return(((~g) & 0xffU) + 1U);
}

void Int2Str(uint8_t *p_str, uint32_t intnum)
{
	uint32_t i, divider = 1000000000U, pos = 0U, status = 0U;

	for (i = 0U; i < 10U; i++)
	{
		p_str[pos++] = (intnum / divider) + 48U;

		intnum = intnum % divider;
		divider /= 10U;
		
		if ((p_str[pos-1] == '0') & (status == 0U))
		{
			pos = 0U;
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
	uint32_t i, divider = 1000000000U, pos = 0U;

	ClearBuffer(result, 16U);
	
	for (i = 0U; i < 10U; i++)
	{
		result[pos++] = (intnum / divider) + 48U;
		intnum = intnum % divider;
		divider /= 10U;
	}
	
	pos -= 1U;
	
	while(size && pos)
	{
		p_str[size - 1U] = result[pos];
		size--;
		pos--;
	}
}


void Str2Hex(uint8_t *p_str, uint16_t lenght, uint8_t *p_hex)
{
	uint32_t hex_cnt = 0U, str_cnt = 0U;
	
	while(lenght)
	{
		if((p_str[str_cnt] - 48U) > 9U) p_hex[hex_cnt] = CONVERTHEX_ALPHA(p_str[str_cnt]) << 4U;
		else p_hex[hex_cnt] = CONVERTDEC(p_str[str_cnt]) << 4U;
		++str_cnt;
		if((p_str[str_cnt] - 48U) > 9U) p_hex[hex_cnt] += CONVERTHEX_ALPHA(p_str[str_cnt]);
		else p_hex[hex_cnt] += CONVERTDEC(p_str[str_cnt]);
		++str_cnt;
		++hex_cnt;
		--lenght;
	}
}

void Hex2Str(uint8_t *p_hex, uint16_t lenght, uint8_t *p_str)
{
	uint32_t hex_cnt = 0U, str_cnt = 0U;
	
	while(lenght)
	{
		if((p_hex[hex_cnt] >> 4U) > 9U) p_str[str_cnt] = (p_hex[hex_cnt] >> 4U) + 55U; // convert to upper ascii letter
		else p_str[str_cnt] = (p_hex[hex_cnt] >> 4U) + 48U;
		++str_cnt;
		if((p_hex[hex_cnt] & 0x0fU) > 9U) p_str[str_cnt] = (p_hex[hex_cnt] & 0x0fU) + 55U; // convert to upper ascii letter
		else p_str[str_cnt]  = (p_hex[hex_cnt]& 0x0fU) + 48U;
		++str_cnt;
		++hex_cnt;
		--lenght;
	}
}	
void ClearBuffer(uint8_t *buffer, uint16_t size)
{
	uint32_t t = 0U;
    
	while(t < size) buffer[t++] = 0U;
}


uint32_t Str2Int(uint8_t *p_inputstr, uint32_t *p_intnum)
{
  uint32_t i = 0U, res = 0U;
  uint32_t val = 0U;

  if ((p_inputstr[0] == '0') && ((p_inputstr[1] == 'x') || (p_inputstr[1] == 'X')))
  {
    i = 2U;
    while ((i < 11U) && (p_inputstr[i] != '\0'))
    {
      if (ISVALIDHEX(p_inputstr[i]))
      {
        val = (val << 4U) + CONVERTHEX(p_inputstr[i]);
      }
      else
      {
        /* Return 0, Invalid input */
        res = 0U;
        break;
      }
      i++;
    }

    /* valid result */
    if (p_inputstr[i] == '\0')
    {
      *p_intnum = val;
      res = 1U;
    }
  }
  else /* max 10-digit decimal input */
  {
    while ((i < 11U) && (res != 1U))
    {
      if (p_inputstr[i] == '\0')
      {
        *p_intnum = val;
        /* return 1 */
        res = 1U;
      }
      else if (((p_inputstr[i] == 'k') || (p_inputstr[i] == 'K')) && (i > 0U))
      {
        val = val << 10U;
        *p_intnum = val;
        res = 1U;
      }
      else if (((p_inputstr[i] == 'm') || (p_inputstr[i] == 'M')) && (i > 0U))
      {
        val = val << 20;
        *p_intnum = val;
        res = 1U;
      }
      else if (ISVALIDDEC(p_inputstr[i]))
      {
        val = val * 10U + CONVERTDEC(p_inputstr[i]);
      }
      else
      {
        /* return 0, Invalid input */
        res = 0U;
        break;
      }
      i++;
    }
  }

  return res;
}


void CharToBin(unsigned char c, char *out) 
{
	*(unsigned long long*)out = 3472328296227680304ULL +
    (((c * 9241421688590303745ULL) / 128) & 72340172838076673ULL);
}



uint32_t BaseToPower(uint8_t base, uint8_t power)
{   
	uint32_t result = 1U;
    
	while(power--) result *= base;
	return (result);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
