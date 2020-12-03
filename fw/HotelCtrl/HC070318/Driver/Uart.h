/**
 ******************************************************************************
 * File Name          : hotel_room_controller.h
 * Date               : 21/08/2016 20:59:16
 * Description        : hotel room Rubicon controller data link module header
 ******************************************************************************
 *
 *
 ******************************************************************************
 */
#ifndef _UART_H_
#define _UART_H_  		100	// version 1.00

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stdio.h"

/* Exported defines    -------------------------------------------------------*/
#define RS485_RX		Bit_RESET
#define RS485_TX		Bit_SET
#define RS485_MODE(x)   GPIO_WriteBit(GPIOE,GPIO_Pin_4,x)
#define USARTx      	USART2
#define UART_IRQ     	USART2_IRQn
#define DATA_BUF_SIZE	256

/* Exported types    ---------------------------------------------------------*/

/* Exported variables  -------------------------------------------------------*/
extern uint16_t rs485_rx_cnt;
extern uint8_t rx_buffer[DATA_BUF_SIZE];
extern uint8_t tx_buffer[DATA_BUF_SIZE];
extern uint8_t *p_comm_buffer;

/* Exported macros     -------------------------------------------------------*/

/* Exported functions  -------------------------------------------------------*/
void uart_init(uint8_t baudrate);
void nvic_config(void);
void RS485_Send_Data(uint8_t *buf,uint16_t len);
void RS485ModeGpio_Init(void);

#endif
/******************************   END OF FILE  **********************************/

