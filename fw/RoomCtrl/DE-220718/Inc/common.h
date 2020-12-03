/**
 ******************************************************************************
 * File Name          : common.h
 * Date               : 10.3.2018
 * Description        : usefull function set and program shared constants
 ******************************************************************************
 */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COMMON_H__
#define __COMMON_H__					221018	// version


/* Include  ------------------------------------------------------------------*/
#include "stm32f1xx.h"


/* Exported Type  ------------------------------------------------------------*/
/* Exported Define  ----------------------------------------------------------*/
/** ==========================================================================*/
/**		G P I O			P O R T		&		P I N			A L I A S		  */
/** ==========================================================================*/
#define DISPLAY_LED_PWM_Pin             GPIO_PIN_0
#define DISPLAY_LED_PWM_Port 		    GPIOA
#define AMBIENT_LIGHT_SENSOR_Pin	    GPIO_PIN_1
#define AMBIENT_LIGHT_SENSOR_Port	    GPIOA
#define ONEWIRE_TX_Pin				    GPIO_PIN_2
#define ONEWIRE_TX_Port				    GPIOA
#define ONEWIRE_RX_Pin				    GPIO_PIN_3
#define ONEWIRE_RX_Port				    GPIOA
#define DOUT_0_Pin					    GPIO_PIN_4
#define DOUT_0_Port					    GPIOA
#define DOUT_1_Pin					    GPIO_PIN_5
#define DOUT_1_Port					    GPIOA
#define DOORLOCK_PWM_Pin			    GPIO_PIN_6
#define DOORLOCK_PWM_Port			    GPIOA
#define DOUT_2_Pin					    GPIO_PIN_7
#define DOUT_2_Port					    GPIOA
#define SOUND_PWM_Pin				    GPIO_PIN_8
#define SOUND_PWM_Port				    GPIOA
#define USART_1_TX_Pin				    GPIO_PIN_9
#define USART_1_TX_Port				    GPIOA
#define USART_1_RX_Pin				    GPIO_PIN_10
#define USART_1_RX_Port				    GPIOA
#define RS485_DIR_Pin				    GPIO_PIN_11
#define RS485_DIR_Port				    GPIOA
#define SHIFT_CLK_Pin				    GPIO_PIN_12
#define SHIFT_CLK_Port				    GPIOA
#define SWDIO_Pin					    GPIO_PIN_13
#define SWDIO_Port					    GPIOA
#define SWCLK_Pin					    GPIO_PIN_14
#define SWCLK_Port					    GPIOA
#define NRF24L01_CE_Pin				    GPIO_PIN_15
#define NRF24L01_CE_Port			    GPIOA
#define CAPS_ALERT_Pin				    SWDIO_Pin
#define CAPS_ALERT_Port				    SWDIO_Port
#define NRF24L01_IRQ_Pin			    SWCLK_Pin
#define NRF24L01_IRQ_Port			    SWCLK_Port
#define DIN_0_Pin					    GPIO_PIN_0
#define DIN_0_Port					    GPIOB
#define DIN_1_Pin					    GPIO_PIN_1
#define DIN_1_Port					    GPIOB
#define DIN_2_Pin					    GPIO_PIN_2
#define DIN_2_Port					    GPIOB
#define DIN_3_Pin					    GPIO_PIN_3
#define DIN_3_Port					    GPIOB
#define DIN_4_Pin					    GPIO_PIN_4
#define DIN_4_Port					    GPIOB
#define DIN_5_Pin					    GPIO_PIN_5
#define DIN_5_Port					    GPIOB
#define DIN_6_Pin					    GPIO_PIN_6
#define DIN_6_Port					    GPIOB
#define DIN_7_Pin					    GPIO_PIN_7
#define DIN_7_Port					    GPIOB
#define I2C1_SCL_Pin				    GPIO_PIN_8
#define I2C1_SCL_Port				    GPIOB
#define I2C1_SDA_Pin				    GPIO_PIN_9
#define I2C1_SDA_Port				    GPIOB
#define DISPLAY_DC_Pin				    GPIO_PIN_10
#define DISPLAY_DC_Port				    GPIOB
#define DISPLAY_CS_Pin				    GPIO_PIN_11
#define DISPLAY_CS_Port				    GPIOB
#define FLASH_CS_Pin				    GPIO_PIN_12
#define FLASH_CS_Port				    GPIOB
#define SPI2_SCK_Pin				    GPIO_PIN_13
#define SPI2_SCK_Port				    GPIOB
#define SPI2_MISO_Pin				    GPIO_PIN_14
#define SPI2_MISO_Port				    GPIOB
#define SPI2_MOSI_Pin				    GPIO_PIN_15
#define SPI2_MOSI_Port				    GPIOB
#define NRF24L01_CSN_Pin			    GPIO_PIN_13
#define NRF24L01_CSN_Port			    GPIOC
/** ==========================================================================*/
/**		S T M 3 2 F 1 0 3 C 8		F L A S H		A D D R E S S E			  */
/** ==========================================================================*/

#define BOOTLOADER_ADDRESS              ((uint32_t)0x08000000U)
#define APPLICATION_ADDRESS 		    ((uint32_t)0x08003000U)
#define USER_FLASH_END_ADDRESS		    ((uint32_t)0x08010000U)
#define BOOTLOADER_FLASH_SIZE		    ((uint32_t)0x00003000U)
#define USER_FLASH_SIZE      		    ((uint32_t)0x0000D000U)
/* Define bitmap representing user flash area that could be write protected (check restricted to pages 16-63 = app_code). */
#define FLASH_PAGE_TO_BE_PROTECTED      (OB_WRP_PAGES16TO19 	| OB_WRP_PAGES20TO23 | OB_WRP_PAGES24TO27 | OB_WRP_PAGES28TO31 | \
                                        OB_WRP_PAGES32TO35 	| OB_WRP_PAGES36TO39 | OB_WRP_PAGES40TO43 | OB_WRP_PAGES44TO47 | \
                                        OB_WRP_PAGES48TO51 	| OB_WRP_PAGES52TO55 | OB_WRP_PAGES56TO59 | OB_WRP_PAGES60TO63)  
/** ==========================================================================*/
/**		B O O T L O A D E R		P R E D E F I N E D		C O M M A N D		  */
/** ==========================================================================*/
#define	BOOTLOADER_CMD_RUN					'W'
#define	BOOTLOADER_STATUS_UPDATE_FAIL		'O'
#define	BOOTLOADER_STATUS_UPDATE_SUCCESS	'N'


/* Exported Variable   -------------------------------------------------------*/
/* Exported Macro ------------------------------------------------------------*/
#define IS_CAP_LETTER(c)    	(((c) >= 'A') && ((c) <= 'F'))
#define IS_LC_LETTER(c)     	(((c) >= 'a') && ((c) <= 'f'))
#define IS_09(c)            	(((c) >= '0') && ((c) <= '9'))
#define ISVALIDHEX(c)       	(IS_CAP_LETTER(c) || IS_LC_LETTER(c) || IS_09(c))
#define ISVALIDDEC(c)     		IS_09(c)
#define CONVERTDEC(c)       	(c - '0')
#define CONVERTHEX_ALPHA(c) 	(IS_CAP_LETTER(c) ? ((c) - 'A'+10) : ((c) - 'a'+10))
#define CONVERTHEX(c)       	(IS_09(c) ? ((c) - '0') : CONVERTHEX_ALPHA(c))


/* Exported function  ------------------------------------------------------- */
uint8_t Bcd2Dec(uint8_t bcd);
uint8_t Dec2Bcd(uint8_t dec);
uint8_t CalcCRC(uint8_t *buff, uint8_t size);
void Int2Str(uint8_t *p_str, uint32_t intnum);
void ClearBuffer(uint8_t *buffer, uint16_t size);
uint32_t Str2Int(uint8_t *inputstr, uint32_t *intnum);
void Str2Hex(uint8_t *p_str, uint16_t lenght, uint8_t *p_hex);
void Hex2Str(uint8_t *p_hex, uint16_t lenght, uint8_t *p_str);
void Int2StrSized(uint8_t *p_str, uint32_t intnum, uint8_t size);
void CharToBin(unsigned char c, char *out);
uint32_t BaseToPower(uint8_t base, uint8_t power);

#endif  /* __COMMON_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
