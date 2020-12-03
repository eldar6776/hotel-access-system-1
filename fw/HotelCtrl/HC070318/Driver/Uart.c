/**
 ******************************************************************************
 * File Name          : uart.c
 * Date               : 21/08/2016 20:59:16
 * Description        : RS485 driver
 ******************************************************************************
 *
 *
 ******************************************************************************
 */
 
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stdio.h"
#include "uart.h"
#include "hotel_room_controller.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
uint16_t rs485_rx_cnt;
uint8_t *p_comm_buffer;
uint8_t rx_buffer[DATA_BUF_SIZE];
uint8_t tx_buffer[DATA_BUF_SIZE];

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
void uart_init(uint8_t baudrate)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;	
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_6;//RX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);

	if(baudrate == RS485_BAUDRATE_2400) USART_InitStructure.USART_BaudRate = 2400;
	else if(baudrate == RS485_BAUDRATE_4800) USART_InitStructure.USART_BaudRate = 4800;
	else if(baudrate == RS485_BAUDRATE_9600) USART_InitStructure.USART_BaudRate = 9600;
	else if(baudrate == RS485_BAUDRATE_19200) USART_InitStructure.USART_BaudRate = 19200;
	else if(baudrate == RS485_BAUDRATE_38400) USART_InitStructure.USART_BaudRate = 38400;
	else if(baudrate == RS485_BAUDRATE_56700) USART_InitStructure.USART_BaudRate = 56700;
	else if(baudrate == RS485_BAUDRATE_115200) USART_InitStructure.USART_BaudRate = 115200;
	else if(baudrate == RS485_BAUDRATE_230400) USART_InitStructure.USART_BaudRate = 230400;
	else if(baudrate == RS485_BAUDRATE_460800) USART_InitStructure.USART_BaudRate = 460800;
	else if(baudrate == RS485_BAUDRATE_921600) USART_InitStructure.USART_BaudRate = 921600;
	else USART_InitStructure.USART_BaudRate = 115200;
	
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx| USART_Mode_Tx ;
	USART_Init(USARTx, &USART_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = UART_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_Cmd(USARTx, ENABLE);
	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);
}

void nvic_config(void)
{	
	/* NVIC configuration */
	/* Configure the Priority Group to 2 bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}

void RS485_Send_Data(uint8_t *buf, uint16_t len)
{
	uint16_t t;
	
	RS485_MODE(Bit_SET);	
  	for(t = 0; t < len; t++)
	{
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
		USART_SendData(USART2, buf[t]);
	}
	
	while(USART_GetFlagStatus(USARTx,USART_FLAG_TC) == RESET);	
	RS485_MODE(Bit_RESET); 
}

void RS485ModeGpio_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE , ENABLE); 

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}




