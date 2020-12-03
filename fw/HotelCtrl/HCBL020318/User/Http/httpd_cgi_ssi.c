/**
 ******************************************************************************
 * @file    httpd_cg_ssi.c
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    31-October-2011
 * @brief   Webserver SSI and CGI handlers
 ******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
 * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; Portions COPYRIGHT 2011 STMicroelectronics</center></h2>
 ******************************************************************************
 */
/**
 ******************************************************************************
 * <h2><center>&copy; Portions COPYRIGHT 2012 Embest Tech. Co., Ltd.</center></h2>
 * @file    httpd_cg_ssi.c
 * @author  CMP Team
 * @version V1.0.0
 * @date    28-December-2012
 * @brief   Webserver SSI and CGI handlers
 *          Modified to support the STM32F4DISCOVERY, STM32F4DIS-BB and
 *          STM32F4DIS-LCD modules. 
 ******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, Embest SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
 * OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/


#include "lwip/debug.h"
#include "httpd.h"
#include "lwip/tcp.h"
#include "fs.h"
#include "ff.h"
#include "main.h"
#include "rtc.h"
#include <string.h>
#include <stdlib.h>
#include "dio.h"
#include "barrier_controller.h"
#include "buzzer.h"
#include "spi_flash.h"
#include "stm32f429i_lcd.h"
#include "usart.h"
#include "stepper.h"
#include "i2c_eeprom.h"
#include "common.h"

tSSIHandler ADC_Page_SSI_Handler;
uint8_t *p_buffer;

#define LOG_DELETED_STRING_LENGHT 	22
const char log_deleted_answer[] = "log list block deleted";


/* we will use character "t" as tag for CGI */
char const* TAGCHAR = "t";
char const** TAGS = &TAGCHAR;

/* CGI handler for incoming http request */
const char * HTTP_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[]);

/* Html request for "/sysctrl.cgi" will start HTTP_CGI_Handler */
const tCGI HTTP_CGI = {"/sysctrl.cgi", HTTP_CGI_Handler};

/* Cgi call table, only one CGI used */
tCGI CGI_TAB[12];


u16_t ADC_Handler(int iIndex, char *pcInsert, int iInsertLen)
{
	uint16_t v;
    /* We have only one SSI handler iIndex = 0 */
    if (iIndex == 0)
    {
		if (HTTP_LogListTransfer.log_transfer_state == HTTP_LOG_LIST_READY)
		{
			v = 0;
			while(v < 512) 
			{
				*(pcInsert + v) = (char) aWorkingBuffer[v];
				v++;
			}
			barrier_ctrl_request = NULL;
			HTTP_LogListTransfer.log_transfer_state = HTTP_LOG_TRANSFER_IDLE;
		}
		else if (HTTP_LogListTransfer.log_transfer_state == HTTP_LOG_LIST_DELETED)
		{
			v = 0;
			while(v < LOG_DELETED_STRING_LENGHT) 
			{
				*(pcInsert + v) = (char) log_deleted_answer[v];
				v++;
			}
			barrier_ctrl_request = NULL;
			HTTP_LogListTransfer.log_transfer_state = HTTP_LOG_TRANSFER_IDLE;
		}
		else if (barrier_http_cmd_state == HTTP_BARRIER_TEMPERATURE_READY)
		{
			v = 0;
			*(pcInsert + v++) = 'T';
			*(pcInsert + v++) = 'M';
			*(pcInsert + v++) = '=';
			*(pcInsert + v++) = (char) rx_buffer[7];	
			*(pcInsert + v++) = (char) rx_buffer[8];
			*(pcInsert + v++) = (char) rx_buffer[9];
			*(pcInsert + v++) = (char) '.';
			*(pcInsert + v++) = (char) rx_buffer[10];
			*(pcInsert + v++) = ' ';
			*(pcInsert + v++) = 'T';
			*(pcInsert + v++) = 'S';
			*(pcInsert + v++) = '=';
			*(pcInsert + v++) = (char) rx_buffer[11];
			*(pcInsert + v++) = ' ';
			*(pcInsert + v++) = 'T';
			*(pcInsert + v++) = 'M';
			*(pcInsert + v++) = '=';
			*(pcInsert + v++) = (char) rx_buffer[12];
			*(pcInsert + v++) = ' ';
			*(pcInsert + v++) = 'S';
			*(pcInsert + v++) = 'P';
			*(pcInsert + v++) = '=';
			*(pcInsert + v++) = (char) rx_buffer[13];
			*(pcInsert + v++) = (char) rx_buffer[14];
			*(pcInsert + v++) = ' ';
			*(pcInsert + v++) = 'D';
			*(pcInsert + v++) = 'F';
			*(pcInsert + v++) = '=';
			*(pcInsert + v++) = (char) rx_buffer[15];
			*(pcInsert + v++) = (char) '.';
			*(pcInsert + v++) = (char) rx_buffer[16];			
			barrier_ctrl_request = NULL;
			barrier_http_cmd_state = NULL;
		}
		else if (barrier_http_cmd_state == HTTP_BARRIER_STATUS_READY)
		{
			v = 0;
			
			while((v < 512) && (aWorkingBuffer[v] != 0x00))
			{
				*(pcInsert + v) = (char) aWorkingBuffer[v];
				v++;
			}
			
			barrier_ctrl_request = NULL;
			barrier_http_cmd_state = NULL;
		}
		else if (barrier_http_cmd_state == HTTP_BARRIER_SYS_INFO_READY)
		{
			v = 0;
			*(pcInsert + v++) = (char) rx_buffer[51];	
			*(pcInsert + v++) = (char) rx_buffer[52];
			*(pcInsert + v++) = (char) rx_buffer[53];
			*(pcInsert + v++) = (char) rx_buffer[54];	
			*(pcInsert + v++) = (char) rx_buffer[55];
			*(pcInsert + v++) = (char) rx_buffer[56];
			*(pcInsert + v++) = (char) rx_buffer[57];
			*(pcInsert + v++) = (char) rx_buffer[58];
			*(pcInsert + v++) = (char) rx_buffer[59];				
			barrier_ctrl_request = NULL;
			barrier_http_cmd_state = NULL;
		}
        return 512;
    }
    return 0;
}

void led_set(uint8_t led)
{
    if (led == 1)
    {
        LED_GPIO_PORT->BSRRH = LED1_GPIO_PIN;
    }
    else if (led == 2)
    {
        LED_GPIO_PORT->BSRRH = LED2_GPIO_PIN;
    }
}


void led_clr(uint8_t led)
{
    if (led == 1)
    {
        LED_GPIO_PORT->BSRRL = LED1_GPIO_PIN;
    }
    else if (led == 2)
    {
        LED_GPIO_PORT->BSRRL = LED2_GPIO_PIN;
    }
}

/**
 * @brief  CGI handler for HTTP request 
 */
const char * HTTP_CGI_Handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    uint32_t i = 0;
    uint32_t t = 0;
    uint32_t x = 0;
	uint8_t to_char[16];

    /* We have only one SSI handler iIndex = 0 */
    if (iIndex == 0)
    {
        /* All leds off */
        led_clr(1);
        led_clr(2);

        /* Check cgi parameter for LED control: 					GET /sysctrl.cgi?led 	*/
        for (i = 0; i < iNumParams; i++)
        {
            if (strcmp(pcParam[i], "led") == 0)
            {
                if (strcmp(pcValue[i], "1") == 0)
                {
                    led_set(1);
                }
                else if (strcmp(pcValue[i], "2") == 0)
                {
                    led_set(2);
                }
            }
        }

        /* Check cgi parameter for date & time: 					GET /sysctrl.cgi?v  	*/
        for (i = 0; i < iNumParams; i++)
        {
            if (strcmp(pcParam[i], "v") == 0)
            {
                p_buffer = aWorkingBuffer;
                while (p_buffer < aWorkingBuffer + sizeof (aWorkingBuffer)) *p_buffer++ = NULL;
                x = 0;

                while ((x < sizeof (aWorkingBuffer)) && (pcValue[i][x] != NULL))
                {
                    aWorkingBuffer[x] = (uint8_t) pcValue[i][x];
                    x++;
                }

                if (x == 14)
                {
                    RTC_HttpCgiDateTimeUpdate(aWorkingBuffer);
                }
                else
                {
                    p_buffer = aWorkingBuffer;
                    while (p_buffer < aWorkingBuffer + sizeof (aWorkingBuffer)) *p_buffer++ = NULL;
                }
            }
        }

        /* Check cgi parameter for log list request: 				GET /sysctrl.cgi?log 	*/
        for (i = 0; i < iNumParams; i++)
        {
            if (strcmp(pcParam[i], "log") == 0)
            {
                if (strcmp(pcValue[i], "3") == 0)
                {
					HTTP_LogListTransfer.log_transfer_state = HTTP_GET_LOG_LIST;
                    barrier_ctrl_request = BARRIER_GET_LOG_LIST;
					BARRIER_ReadBlockFromLogList();
                    return "/log.html";
                }
				else if (strcmp(pcValue[i], "4") == 0)
                {
					HTTP_LogListTransfer.log_transfer_state = HTTP_DELETE_LOG_LIST;
                    barrier_ctrl_request = BARRIER_DELETE_LOG_LIST;
					BARRIER_DeleteBlockFromLogList();
                    return "/log.html";
                }
            }
        }
		
		/* Check cgi parameter for BARRIER ctrl version request:	GET /sysctrl.cgi?cve  	*/
        for (i = 0; i < iNumParams; i++)
        {
            if (strcmp(pcParam[i], "cve") == 0)
            {	
                p_buffer = aWorkingBuffer;
                while (p_buffer < aWorkingBuffer + sizeof (aWorkingBuffer)) *p_buffer++ = NULL;
							
                x = 0;

                /* copy all string till next '&' parameter marker */
                while ((x < sizeof (aWorkingBuffer)) && (pcValue[i][x] != NULL))
                {
                    aWorkingBuffer[x] = (uint8_t) pcValue[i][x];
                    x++;
                }
                
				barrier_http_cmd_state = HTTP_GET_BARRIER_SYS_INFO;
				barrier_ctrl_request = BARRIER_GET_SYS_INFO;
				BARRIER_StartResponseTimer(BARRIER_HTTP_RESPONSE_TIMEOUT);
				
				while(!IsBARRIER_ResponseTimerExpired()) 
				{
					BARRIER_Service();
					
					if(barrier_http_cmd_state != HTTP_GET_BARRIER_SYS_INFO)
					{
						BARRIER_StopResponseTimer();
						return "/log.html";
					}
				}
				
				barrier_http_cmd_state = NULL;
				barrier_ctrl_request = NULL;
				return "/log.html";
            }
        }

        /* Check cgi parameter for BARRIER status request:			GET /sysctrl.cgi?cst  	*/
        for (i = 0; i < iNumParams; i++)
        {
            if (strcmp(pcParam[i], "cst") == 0)
            {	
				I2C_EERPOM_ReadBytes16(I2C_EE_READ_PAGE_0, EE_ONE_TIME_USER_ENTRY_COUNTER_ADDRESS, 66);
				
				aWorkingBuffer[0] = 'S';
				aWorkingBuffer[1] = 'T';
				aWorkingBuffer[2] = '=';
				
				if(BarrierState == BARRIER_STOP) aWorkingBuffer[3] = '0';
				else if(BarrierState == BARRIER_ENTRY) aWorkingBuffer[3] = '1';
				else if(BarrierState == BARRIER_EXIT) aWorkingBuffer[3] = '2';
				else if(BarrierState == BARRIER_UNBLOCKED) aWorkingBuffer[3] = '3';
				else if(BarrierState == BARRIER_OUT_OF_ORDER) aWorkingBuffer[3] = '4';
				else if(BarrierState == BARRIER_ERROR) aWorkingBuffer[3] = '5';
				else if(BarrierState == BARRIER_TIME_OUT) aWorkingBuffer[3] = '6';
				else if(BarrierState == BARRIER_BRAKE) aWorkingBuffer[3] = '7';
				
				aWorkingBuffer[4] = 'O';
				aWorkingBuffer[5] = 'E';
				aWorkingBuffer[6] = '=';
				
				x = 0;
				
				while(x < sizeof(to_char))
				{
					to_char[x] = 0x00;
					x++;
				}
				
				t = aEepromBuffer[0];
				t = (t << 8);
				t += aEepromBuffer[1];
				t = (t << 8);
				t += aEepromBuffer[2];
				t = (t << 8);
				t += aEepromBuffer[3];
				
				Int2Str(to_char, t);

				x = 0;
				i = 7;
				
				while (to_char[x] != 0x00)
				{
					aWorkingBuffer[i++] = to_char[x++];
				}
				
				
				aWorkingBuffer[i++] = 'O';
				aWorkingBuffer[i++] = 'X';
				aWorkingBuffer[i++] = '=';
				
				x = 0;
				
				while(x < sizeof(to_char))
				{
					to_char[x] = 0x00;
					x++;
				}
				
				t = aEepromBuffer[4];
				t = (t << 8);
				t += aEepromBuffer[5];
				t = (t << 8);
				t += aEepromBuffer[6];
				t = (t << 8);
				t += aEepromBuffer[7];
				
				Int2Str(to_char, t);

				x = 0;
				
				while (to_char[x] != 0x00)
				{
					aWorkingBuffer[i++] = to_char[x++];
				}
				
				aWorkingBuffer[i++] = 'P';
				aWorkingBuffer[i++] = 'E';
				aWorkingBuffer[i++] = '=';
				
				x = 0;
				
				while(x < sizeof(to_char))
				{
					to_char[x] = 0x00;
					x++;
				}
				
				t = aEepromBuffer[8];
				t = (t << 8);
				t += aEepromBuffer[9];
				t = (t << 8);
				t += aEepromBuffer[10];
				t = (t << 8);
				t += aEepromBuffer[11];
				
				Int2Str(to_char, t);

				x = 0;
				
				while (to_char[x] != 0x00)
				{
					aWorkingBuffer[i++] = to_char[x++];
				}
				
				aWorkingBuffer[i++] = 'P';
				aWorkingBuffer[i++] = 'X';
				aWorkingBuffer[i++] = '=';
				
				x = 0;
				
				while(x < sizeof(to_char))
				{
					to_char[x] = 0x00;
					x++;
				}
				
				t = aEepromBuffer[12];
				t = (t << 8);
				t += aEepromBuffer[13];
				t = (t << 8);
				t += aEepromBuffer[14];
				t = (t << 8);
				t += aEepromBuffer[15];
				
				Int2Str(to_char, t);

				x = 0;
				
				while (to_char[x] != 0x00)
				{
					aWorkingBuffer[i++] = to_char[x++];
				}
				
				aWorkingBuffer[i++] = 'C';
				aWorkingBuffer[i++] = 'E';
				aWorkingBuffer[i++] = '=';
				
				x = 0;
				
				while(x < sizeof(to_char))
				{
					to_char[x] = 0x00;
					x++;
				}
				
				t = aEepromBuffer[16];
				t = (t << 8);
				t += aEepromBuffer[17];
				t = (t << 8);
				t += aEepromBuffer[18];
				t = (t << 8);
				t += aEepromBuffer[19];
				
				Int2Str(to_char, t);

				x = 0;
				
				while (to_char[x] != 0x00)
				{
					aWorkingBuffer[i++] = to_char[x++];
				}
				
				aWorkingBuffer[i++] = 'C';
				aWorkingBuffer[i++] = 'X';
				aWorkingBuffer[i++] = '=';
				
				x = 0;
				
				while(x < sizeof(to_char))
				{
					to_char[x] = 0x00;
					x++;
				}
				
				t = aEepromBuffer[20];
				t = (t << 8);
				t += aEepromBuffer[21];
				t = (t << 8);
				t += aEepromBuffer[22];
				t = (t << 8);
				t += aEepromBuffer[23];
				
				Int2Str(to_char, t);

				x = 0;
				
				while (to_char[x] != 0x00)
				{
					aWorkingBuffer[i++] = to_char[x++];
				}
				
				aWorkingBuffer[i++] = 'W';
				aWorkingBuffer[i++] = 'E';
				aWorkingBuffer[i++] = '=';
				
				x = 0;
				
				while(x < sizeof(to_char))
				{
					to_char[x] = 0x00;
					x++;
				}
				
				t = aEepromBuffer[24];
				t = (t << 8);
				t += aEepromBuffer[25];
				t = (t << 8);
				t += aEepromBuffer[26];
				t = (t << 8);
				t += aEepromBuffer[27];
				
				Int2Str(to_char, t);

				x = 0;
				
				while (to_char[x] != 0x00)
				{
					aWorkingBuffer[i++] = to_char[x++];
				}
				
				aWorkingBuffer[i++] = 'W';
				aWorkingBuffer[i++] = 'X';
				aWorkingBuffer[i++] = '=';
				
				x = 0;
				
				while(x < sizeof(to_char))
				{
					to_char[x] = 0x00;
					x++;
				}
				
				t = aEepromBuffer[28];
				t = (t << 8);
				t += aEepromBuffer[29];
				t = (t << 8);
				t += aEepromBuffer[30];
				t = (t << 8);
				t += aEepromBuffer[31];
				
				Int2Str(to_char, t);

				x = 0;
				
				while (to_char[x] != 0x00)
				{
					aWorkingBuffer[i++] = to_char[x++];
				}
				
				aWorkingBuffer[i++] = 'K';
				aWorkingBuffer[i++] = 'E';
				aWorkingBuffer[i++] = '=';
				
				x = 0;
				
				while(x < sizeof(to_char))
				{
					to_char[x] = 0x00;
					x++;
				}
				
				t = aEepromBuffer[32];
				t = (t << 8);
				t += aEepromBuffer[33];
				t = (t << 8);
				t += aEepromBuffer[34];
				t = (t << 8);
				t += aEepromBuffer[35];
				
				Int2Str(to_char, t);

				x = 0;
				
				while (to_char[x] != 0x00)
				{
					aWorkingBuffer[i++] = to_char[x++];
				}
				
				aWorkingBuffer[i++] = 'K';
				aWorkingBuffer[i++] = 'X';
				aWorkingBuffer[i++] = '=';
				
				x = 0;
				
				while(x < sizeof(to_char))
				{
					to_char[x] = 0x00;
					x++;
				}
				
				t = aEepromBuffer[36];
				t = (t << 8);
				t += aEepromBuffer[37];
				t = (t << 8);
				t += aEepromBuffer[38];
				t = (t << 8);
				t += aEepromBuffer[39];
				
				Int2Str(to_char, t);

				x = 0;
				
				while (to_char[x] != 0x00)
				{
					aWorkingBuffer[i++] = to_char[x++];
				}
				
				aWorkingBuffer[i++] = 'T';
				aWorkingBuffer[i++] = 'E';
				aWorkingBuffer[i++] = '=';
				
				x = 0;
				
				while(x < sizeof(to_char))
				{
					to_char[x] = 0x00;
					x++;
				}
				
				t = aEepromBuffer[40];
				t = (t << 8);
				t += aEepromBuffer[41];
				t = (t << 8);
				t += aEepromBuffer[42];
				t = (t << 8);
				t += aEepromBuffer[43];
				
				Int2Str(to_char, t);

				x = 0;
				
				while (to_char[x] != 0x00)
				{
					aWorkingBuffer[i++] = to_char[x++];
				}
				
				aWorkingBuffer[i++] = 'T';
				aWorkingBuffer[i++] = 'X';
				aWorkingBuffer[i++] = '=';
				
				x = 0;
				
				while(x < sizeof(to_char))
				{
					to_char[x] = 0x00;
					x++;
				}
				
				t = aEepromBuffer[44];
				t = (t << 8);
				t += aEepromBuffer[45];
				t = (t << 8);
				t += aEepromBuffer[46];
				t = (t << 8);
				t += aEepromBuffer[47];
				
				Int2Str(to_char, t);

				x = 0;
				
				while (to_char[x] != 0x00)
				{
					aWorkingBuffer[i++] = to_char[x++];
				}
				
				aWorkingBuffer[i++] = 'T';
				aWorkingBuffer[i++] = 'R';
				aWorkingBuffer[i++] = '=';
				aWorkingBuffer[i++] = ((aEepromBuffer[48] >> 4) + 48);
				aWorkingBuffer[i++] = ((aEepromBuffer[48] &0x0f) + 48);
				aWorkingBuffer[i++] = ((aEepromBuffer[49] >> 4) + 48);
				aWorkingBuffer[i++] = ((aEepromBuffer[49] &0x0f) + 48);
				aWorkingBuffer[i++] = '2';
				aWorkingBuffer[i++] = '0';
				aWorkingBuffer[i++] = ((aEepromBuffer[50] >> 4) + 48);
				aWorkingBuffer[i++] = ((aEepromBuffer[50] &0x0f) + 48);
				aWorkingBuffer[i++] = ((aEepromBuffer[51] >> 4) + 48);
				aWorkingBuffer[i++] = ((aEepromBuffer[51] &0x0f) + 48);
				aWorkingBuffer[i++] = ((aEepromBuffer[52] >> 4) + 48);
				aWorkingBuffer[i++] = ((aEepromBuffer[52] &0x0f) + 48);
				aWorkingBuffer[i++] = ((aEepromBuffer[53] >> 4) + 48);
				aWorkingBuffer[i++] = ((aEepromBuffer[53] &0x0f) + 48);
				
				barrier_http_cmd_state = HTTP_BARRIER_STATUS_READY;
				return "/log.html";
            }
        }
        /* Check cgi parameter for BARRIER set status:				SET /sysctrl.cgi?stg 	*/
		for (i = 0; i < iNumParams; i++) 
		{
			if (strcmp(pcParam[i] , "stg") == 0) 
			{       
				p_buffer = aWorkingBuffer;
				while(p_buffer < aWorkingBuffer + sizeof(aWorkingBuffer)) *p_buffer++ = NULL;
				x = 0;
				
				while((x < sizeof(aWorkingBuffer)) && (pcValue[i][x] != NULL))
				{
					aWorkingBuffer[x] = (uint8_t) pcValue[i][x];
					x++;
				}
				aWorkingBuffer[x] = NULL;
				x++;
			}
			else if (strcmp(pcParam[i] , "val") == 0) 
			{       
				if (strcmp(pcValue[i], "0") == 0)
                {
                    BarrierState = BARRIER_STOP;
                }
				else if (strcmp(pcValue[i], "1") == 0)
                {
                    BarrierState = BARRIER_ENTRY;
                }
				else if (strcmp(pcValue[i], "2") == 0)
                {
                    BarrierState = BARRIER_EXIT;
                }
				else if (strcmp(pcValue[i], "3") == 0)
                {
                    BarrierState = BARRIER_UNBLOCKED;
                }
				else if (strcmp(pcValue[i], "4") == 0)
                {
                    BarrierState = BARRIER_OUT_OF_ORDER;
                }
				else if (strcmp(pcValue[i], "5") == 0)
                {
                    BarrierState = BARRIER_ERROR;
                }
				
				
				
//				t = 0;
//				/* copy all string till next '&' parameter marker */
//				while((t < sizeof(aWorkingBuffer) + x) && (pcValue[i][t] != NULL))
//				{
//					aWorkingBuffer[x++] = (uint8_t) pcValue[i][t++];
//				}
//				
//				barrier_ctrl_request = BARRIER_SET_ROOM_STATUS;
			}
		}
        /* Check cgi parameter for BARRIER set digital output:		GET /sysctrl.cgi?cdo 	*/

        /* Check cgi parameter for BARRIER update firmware: 		GET /sysctrl.cgi?cud 	*/
        for (i = 0; i < iNumParams; i++)
        {
            if (strcmp(pcParam[i], "cud") == 0)
            {
                p_buffer = aWorkingBuffer;
                while (p_buffer < aWorkingBuffer + sizeof (aWorkingBuffer)) *p_buffer++ = NULL;
                x = 0;

                /* copy all string till next '&' parameter marker */
                while ((x < sizeof (aWorkingBuffer)) && (pcValue[i][x] != NULL))
                {
                    aWorkingBuffer[x] = (uint8_t) pcValue[i][x];
                    x++;
                }

                barrier_ctrl_request = BARRIER_DOWNLOAD_FIRMWARE;
            }
        }

        /* Check cgi parameter for BARRIER set display brightness:	GET /sysctrl.cgi?cbr 	*/
		for (i = 0; i < iNumParams; i++) 
		{
			if (strcmp(pcParam[i] , "cbr") == 0) 
			{       
				p_buffer = aWorkingBuffer;
				while(p_buffer < aWorkingBuffer + sizeof(aWorkingBuffer)) *p_buffer++ = NULL;
				x = 0;
				
				while((x < sizeof(aWorkingBuffer)) && (pcValue[i][x] != NULL))
				{
					aWorkingBuffer[x] = (uint8_t) pcValue[i][x];
					x++;
				}
				aWorkingBuffer[x] = NULL;
				x++;
			}
			else if (strcmp(pcParam[i] , "br") == 0) 
			{       
				t = 0;
				/* copy all string till next '&' parameter marker */
				while((t < sizeof(aWorkingBuffer) + x) && (pcValue[i][t] != NULL))
				{
					aWorkingBuffer[x++] = (uint8_t) pcValue[i][t++];
				}
				
				barrier_ctrl_request = BARRIER_SET_DISPLAY_BRIGHTNESS;
				
			}
		}
		/* Check cgi parameter for BARRIER presed card id:			GET /sysctrl.cgi?sud 	*/
		for (i = 0; i < iNumParams; i++) 
		{
			if (strcmp(pcParam[i] , "sud") == 0) 
			{       
				p_buffer = aWorkingBuffer;
				while(p_buffer < aWorkingBuffer + sizeof(aWorkingBuffer)) *p_buffer++ = NULL;
				x = 0;
				
				while((x < sizeof(aWorkingBuffer)) && (pcValue[i][x] != NULL))
				{
					aWorkingBuffer[x] = (uint8_t) pcValue[i][x];
					x++;
				}
				
				aWorkingBuffer[x] = NULL;
				x++;
			}
			else if (strcmp(pcParam[i] , "slo") == 0) 
			{       
				t = 0;
				
				while((t < sizeof(aWorkingBuffer) + x) && (pcValue[i][t] != NULL))
				{
					aWorkingBuffer[x++] = (uint8_t) pcValue[i][t++];
				}
				
				aWorkingBuffer[x] = NULL;
				x++;
				
			}
			else if (strcmp(pcParam[i] , "ska") == 0) 
			{       
				t = 0;
				
				while((t < sizeof(aWorkingBuffer) + x) && (pcValue[i][t] != NULL))
				{
					aWorkingBuffer[x++] = (uint8_t) pcValue[i][t++];
				}
				
				aWorkingBuffer[x] = NULL;
				x++;
				
			}
			else if (strcmp(pcParam[i] , "sda") == 0) 
			{       
				t = 0;
				
				while((t < sizeof(aWorkingBuffer) + x) && (pcValue[i][t] != NULL))
				{
					aWorkingBuffer[x++] = (uint8_t) pcValue[i][t++];
				}
				
				barrier_ctrl_request = BARRIER_SET_MIFARE_PERMITED_CARD;
			}
		}
		/* Check cgi parameter for BARRIER SOS alarm reset request:	GET /sysctrl.cgi?rud	*/
        for (i = 0; i < iNumParams; i++)
        {
            if (strcmp(pcParam[i], "rud") == 0)
            {
				x = 0;
				p_buffer = aWorkingBuffer;	
				
                while (p_buffer < aWorkingBuffer + sizeof (aWorkingBuffer)) *p_buffer++ = NULL;
				
				p_buffer = aWorkingBuffer;
				
                while ((p_buffer < aWorkingBuffer + sizeof (aWorkingBuffer)) && (pcValue[i][x] != NULL))
                {
                    *p_buffer++ = (uint8_t) pcValue[i][x++];
                }
				
                if ((x > 0) || (x < 5))
                {
					if(*(p_buffer - x) == '0') BUZZER_Off();
					else barrier_ctrl_request = BARRIER_RESET_SOS_ALARM;
                }
                else
                {
                    p_buffer = aWorkingBuffer;
                    while (p_buffer < aWorkingBuffer + sizeof (aWorkingBuffer)) *p_buffer++ = NULL;
                }
            }
        }
		/* Check cgi parameter for BARRIER reset controller:		GET /sysctrl.cgi?rst 	*/
		for (i = 0; i < iNumParams; i++) 
		{
			if (strcmp(pcParam[i], "rst") == 0)
            {	
                p_buffer = aWorkingBuffer;
                while (p_buffer < aWorkingBuffer + sizeof (aWorkingBuffer)) *p_buffer++ = NULL;
							
                x = 0;

                /* copy all string till next '&' parameter marker */
                while ((x < sizeof (aWorkingBuffer)) && (pcValue[i][x] != NULL))
                {
                    aWorkingBuffer[x] = (uint8_t) pcValue[i][x];
                    x++;
                }

                barrier_ctrl_request = BARRIER_START_BOOTLOADER;                
            }
		}
		/* Check cgi parameter for BARRIER selected image upload:	GET /sysctrl.cgi?sld	*/
		for (i = 0; i < iNumParams; i++) 
		{
			if (strcmp(pcParam[i] , "sld") == 0) 
			{       
				p_buffer = aWorkingBuffer;
				while(p_buffer < aWorkingBuffer + sizeof(aWorkingBuffer)) *p_buffer++ = NULL;
				x = 0;
				
				while((x < sizeof(aWorkingBuffer)) && (pcValue[i][x] != NULL))
				{
					aWorkingBuffer[x] = (uint8_t) pcValue[i][x];
					x++;
				}
				aWorkingBuffer[x] = NULL;
				x++;
			}
			else if (strcmp(pcParam[i] , "sli") == 0) 
			{       
				t = 0;
				/* copy all string till next '&' parameter marker */
				while((t < sizeof(aWorkingBuffer) + x) && (pcValue[i][t] != NULL))
				{
					aWorkingBuffer[x++] = (uint8_t) pcValue[i][t++];
				}
				
				barrier_ctrl_request = BARRIER_DOWNLOAD_DIPLAY_IMAGE;		
			}
		}
		/* Check cgi parameter for BARRIER all image upload:		GET /sysctrl.cgi?sle	*/
		for (i = 0; i < iNumParams; i++) 
		{
			if (strcmp(pcParam[i] , "sle") == 0) 
			{       
				p_buffer = aWorkingBuffer;
				while(p_buffer < aWorkingBuffer + sizeof(aWorkingBuffer)) *p_buffer++ = NULL;
				x = 0;
				
				while((x < sizeof(aWorkingBuffer)) && (pcValue[i][x] != NULL))
				{
					aWorkingBuffer[x] = (uint8_t) pcValue[i][x];
					x++;
				}
				aWorkingBuffer[x] = NULL;
				x++;
			}
			else if (strcmp(pcParam[i] , "sla") == 0) 
			{       
				t = 0;
				/* copy all string till next '&' parameter marker */
				while((t < sizeof(aWorkingBuffer) + x) && (pcValue[i][t] != NULL))
				{
					aWorkingBuffer[x++] = (uint8_t) pcValue[i][t++];
				}
				
				barrier_ctrl_request = BARRIER_DOWNLOAD_ALL_IMAGE;
			}
		}
		/* Check cgi parameter for BARRIER update from config file:	GET /sysctrl.cgi?slo	*/
		for (i = 0; i < iNumParams; i++) 
		{
			if (strcmp(pcParam[i] , "slo") == 0) 
			{       
				p_buffer = aWorkingBuffer;
				while(p_buffer < aWorkingBuffer + sizeof(aWorkingBuffer)) *p_buffer++ = NULL;				
				barrier_ctrl_request = BARRIER_UPDATE_FROM_CONFIG_FILE;
			}
		}
		/* Check cgi parameter for BARRIER address set:				GET /sysctrl.cgi?iud 	*/
		for (i = 0; i < iNumParams; i++) 
		{
			if (strcmp(pcParam[i] , "iud") == 0) 
			{       
				p_buffer = aWorkingBuffer;
				while(p_buffer < aWorkingBuffer + sizeof(aWorkingBuffer)) *p_buffer++ = NULL;
				x = 0;
				
				while((x < sizeof(aWorkingBuffer)) && (pcValue[i][x] != NULL))
				{
					aWorkingBuffer[x] = (uint8_t) pcValue[i][x];
					x++;
				}
				aWorkingBuffer[x] = NULL;
				x++;
			}
			else if (strcmp(pcParam[i] , "iun") == 0) 
			{       
				t = 0;
				/* copy all string till next '&' parameter marker */
				while((t < sizeof(aWorkingBuffer) + x) && (pcValue[i][t] != NULL))
				{
					aWorkingBuffer[x++] = (uint8_t) pcValue[i][t++];
				}
				
				barrier_ctrl_request = BARRIER_SET_RS485_CONFIG;
			}
		}
		/* Check cgi parameter for BARRIER get room temperature:	GET /sysctrl.cgi?tct 	*/    
		/* Check cgi parameter for BARRIER set room temperature:	GET /sysctrl.cgi?tsp 	*/

	}
    /* uri to send after cgi call*/
    return "/sysctrl.html";
}

/**
 * Initialize SSI handlers
 */
void httpd_ssi_init(void)
{
    /* configure SSI handlers (ADC page SSI) */
    http_set_ssi_handler(ADC_Handler, (char const **) TAGS, 1);
}

/**
 * Initialize CGI handlers
 */
void httpd_cgi_init(void)
{
    /* configure CGI handlers */
    CGI_TAB[0] = HTTP_CGI;
    http_set_cgi_handlers(CGI_TAB, 1);
}


