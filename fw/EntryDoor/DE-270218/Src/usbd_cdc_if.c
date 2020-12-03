/**
  ******************************************************************************
  * @file           : usbd_cdc_if.c
  * @version        : v2.0_Cube
  * @brief          : Usb device for Virtual Com Port.
  ******************************************************************************
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_if.h"
#include "nrf24l01.h"

#define APP_RX_DATA_SIZE  64
#define APP_TX_DATA_SIZE  64

volatile uint16_t usb_rx_data_size;

uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];
uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

extern USBD_HandleTypeDef hUsbDeviceFS;
extern uint8_t radio_tx_buffer[RADIO_BUFFER_SIZE];

static int8_t CDC_Init_FS(void);
static int8_t CDC_DeInit_FS(void);
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Receive_FS(uint8_t* pbuf, uint32_t *Len);


USBD_CDC_ItfTypeDef USBD_Interface_fops_FS =
{
	CDC_Init_FS,
	CDC_DeInit_FS,
	CDC_Control_FS,
	CDC_Receive_FS
};

/* Private functions ---------------------------------------------------------*/
static int8_t CDC_Init_FS(void)
{
	USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, 0);
	USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);
	return (USBD_OK);
}


static int8_t CDC_DeInit_FS(void)
{
	return (USBD_OK);
}


static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
  /* USER CODE BEGIN 5 */
  switch(cmd)
  {
    case CDC_SEND_ENCAPSULATED_COMMAND:
		break;

    case CDC_GET_ENCAPSULATED_RESPONSE:
		break;

    case CDC_SET_COMM_FEATURE:
		break;

    case CDC_GET_COMM_FEATURE:
		break;

    case CDC_CLEAR_COMM_FEATURE:
		break;

  /*******************************************************************************/
  /* Line Coding Structure                                                       */
  /*-----------------------------------------------------------------------------*/
  /* Offset | Field       | Size | Value  | Description                          */
  /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
  /* 4      | bCharFormat |   1  | Number | Stop bits                            */
  /*                                        0 - 1 Stop bit                       */
  /*                                        1 - 1.5 Stop bits                    */
  /*                                        2 - 2 Stop bits                      */
  /* 5      | bParityType |  1   | Number | Parity                               */
  /*                                        0 - None                             */
  /*                                        1 - Odd                              */
  /*                                        2 - Even                             */
  /*                                        3 - Mark                             */
  /*                                        4 - Space                            */
  /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
  /*******************************************************************************/
    case CDC_SET_LINE_CODING:
		break;

    case CDC_GET_LINE_CODING:		
		break;

    case CDC_SET_CONTROL_LINE_STATE:
		break;

    case CDC_SEND_BREAK:
		break;

	default:
		break;
  }

  return (USBD_OK);
  /* USER CODE END 5 */
}


static int8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
	uint8_t sta = USBD_OK;
	USBD_CDC_SetRxBuffer(&hUsbDeviceFS, Buf);
	sta = USBD_CDC_ReceivePacket(&hUsbDeviceFS);
	usb_rx_data_size = *Len;
	memcpy(radio_tx_buffer, Buf, usb_rx_data_size);
	return (sta);
}


uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
	uint8_t sta = USBD_OK;
	USBD_CDC_SetTxBuffer(&hUsbDeviceFS, Buf, Len);
	sta = USBD_CDC_TransmitPacket(&hUsbDeviceFS);
	return sta;
}


void USB_SendData(uint8_t* buff, uint16_t size)
{
	CDC_Transmit_FS(buff, size);
}


uint8_t IsUSB_DataReady(void)
{
	return(usb_rx_data_size);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
