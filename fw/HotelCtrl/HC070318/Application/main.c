/**
  * @file    main.c 
  * @author  eldar6776@hotmail.com
  * @version V1,0
  * @date    06.03.2017
  * @brief   Main Program
  */
	
#include "main.h"
#include "eth_bsp.h"
#include "Gpio.h"
#include "usart.h"
#include "uart.h"
#include "stm32f429i_lcd.h"
#include "stm32f4x7_eth.h"
#include "netconf.h"
#include "eth_bsp.h"
#include "tftpserver.h"
#include "httpd.h"
#include "netbios.h"
#include "netconf.h"
#include "hotel_room_controller.h"
#include "rtc.h"
#include "i2c_eeprom.h"
#include "buzzer.h"
#include "W25Q16.h"
#include "Display.h"
#include "RTP_Touch.h"



/* Constante ----------------------------------------------------------------*/
const char sys_info[] = "HW: RED_DRAGON_F429, REV: 1_0,      FW: HC070318 ";

//__IO uint8_t ALARM_Occured = 0;
//__IO uint32_t RTCAlarmCount = 0;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
//static void RTC_Config(void);
//static void RTC_AlarmConfig(void);
void IWDG_Init(void);
void RAM_Init(void);

volatile uint32_t SystickCnt;
volatile uint32_t delay_tm;
uint32_t system_config;

uint32_t get_systick(void)
{
	return SystickCnt;
}

/*****************************************************************************
**   Main Function  main()
******************************************************************************/
int main(void)
{
	RTC_Config();
	I2C_EERPOM_Init();
	RAM_Init();
	W25Qxx_Init();
	uart_init(rs485_interface_baudrate);
	LCD_Init();
	LTDC_Cmd(ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE); 
	BUZZER_Init();
	nvic_config();
	LED_Init();
	Key_Init();
	FS5206_Init();
	eth_init();
	LwIP_Init();
	tftpd_init();
	httpd_init();
    netbios_init();
	DisplayInit();	
	RUBICON_Init();
	
	while (1) 
	{
		if (ETH_CheckFrameReceived()) LwIP_Pkt_Handle();
		LwIP_Periodic_Handle(SystickCnt);
		RUBICON_ProcessService();
		DisplayService();
		//IWDG_ReloadCounter();
	}
}

void RAM_Init(void)
{
	I2C_EERPOM_ReadBytes16(I2C_EE_READ_PAGE_0, EE_RS485_INTERFACE_ADDRESS, 2);
	rs485_interface_address = ((i2c_ee_buffer[0] << 8) + i2c_ee_buffer[1]);
	I2C_EERPOM_ReadBytes16(I2C_EE_READ_PAGE_0, EE_RS485_GROUP_ADDRESS, 2);
	rs485_group_address = ((i2c_ee_buffer[0] << 8) + i2c_ee_buffer[1]);
	I2C_EERPOM_ReadBytes16(I2C_EE_READ_PAGE_0, EE_RS485_BROADCAST_ADDRESS, 2);
	rs485_broadcast_address = ((i2c_ee_buffer[0] << 8) + i2c_ee_buffer[1]);
	I2C_EERPOM_ReadBytes16(I2C_EE_READ_PAGE_0, EE_RS485_INTERFACE_BAUDRATE_ADDRESS, 1);
	rs485_interface_baudrate = i2c_ee_buffer[0];
	
	if((rs485_interface_baudrate < RS485_BAUDRATE_2400) || (rs485_interface_baudrate > RS485_BAUDRATE_921600))
	{
		rs485_interface_baudrate = RS485_BAUDRATE_115200;
		I2C_EERPOM_WriteByte16(I2C_EE_WRITE_PAGE_0, EE_RS485_INTERFACE_BAUDRATE_ADDRESS, rs485_interface_baudrate);
		delay(I2C_EE_WRITE_DELAY);
		
		rs485_interface_address = RS485_INTERFACE_DEFAULT_ADDRESS;
		i2c_ee_buffer[0] = (rs485_interface_address >> 8);
		i2c_ee_buffer[1] = (rs485_interface_address & 0xff);
		I2C_EERPOM_WriteBytes16(I2C_EE_WRITE_PAGE_0, EE_RS485_INTERFACE_ADDRESS, 2);
		delay(I2C_EE_WRITE_DELAY);
		
		rs485_group_address = RUBICON_DEFFAULT_GROUP_ADDRESS;
		i2c_ee_buffer[0] = (RUBICON_DEFFAULT_GROUP_ADDRESS >> 8);
		i2c_ee_buffer[1] = (RUBICON_DEFFAULT_GROUP_ADDRESS & 0xff);
		I2C_EERPOM_WriteBytes16(I2C_EE_WRITE_PAGE_0, EE_RS485_GROUP_ADDRESS, 2);
		delay(I2C_EE_WRITE_DELAY);
		
		rs485_broadcast_address = RUBICON_DEFFAULT_BROADCAST_ADDRESS;
		i2c_ee_buffer[0] = (RUBICON_DEFFAULT_BROADCAST_ADDRESS >> 8);
		i2c_ee_buffer[1] = (RUBICON_DEFFAULT_BROADCAST_ADDRESS & 0xff);
		I2C_EERPOM_WriteBytes16(I2C_EE_WRITE_PAGE_0, EE_RS485_BROADCAST_ADDRESS, 2);
		delay(I2C_EE_WRITE_DELAY);
	}

	I2C_EERPOM_ReadBytes16(I2C_EE_READ_PAGE_0, EE_ETHERNET_IP_ADDRESS, 4);
	ip_address[0] = i2c_ee_buffer[0];
	ip_address[1] = i2c_ee_buffer[1];
	ip_address[2] = i2c_ee_buffer[2];
	ip_address[3] = i2c_ee_buffer[3];
	
	I2C_EERPOM_ReadBytes16(I2C_EE_READ_PAGE_0, EE_ETHERNET_SUBNET_ADDRESS, 4);
	subnet_mask[0] = i2c_ee_buffer[0];
	subnet_mask[1] = i2c_ee_buffer[1];
	subnet_mask[2] = i2c_ee_buffer[2];
	subnet_mask[3] = i2c_ee_buffer[3];
	
	I2C_EERPOM_ReadBytes16(I2C_EE_READ_PAGE_0, EE_ETHERNET_GATEWAY_ADDRESS, 4);
	gateway_address[0] = i2c_ee_buffer[0];
	gateway_address[1] = i2c_ee_buffer[1];
	gateway_address[2] = i2c_ee_buffer[2];
	gateway_address[3] = i2c_ee_buffer[3];
	
	I2C_EERPOM_ReadBytes16(I2C_EE_READ_PAGE_0, EE_SYSTEM_ID_ADDRESS, 2);
	system_id = ((i2c_ee_buffer[0] << 8) + i2c_ee_buffer[1]);
	
	I2C_EERPOM_ReadBytes16(I2C_EE_READ_PAGE_0, EE_SYSTEM_CONFIG_ADDRESS, 4);
	system_config = ((i2c_ee_buffer[0] << 24) + (i2c_ee_buffer[1] << 16) + (i2c_ee_buffer[2] << 8) + i2c_ee_buffer[3]);
	
}



void IWDG_Init(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //Can make at first before visit the register is written
	IWDG_SetPrescaler(IWDG_Prescaler_256); //64 frequency demultiplication A cycle 1.6ms
	IWDG_SetReload(800); //Long 12 800*1.6 most =1.28S
	/*Reload IWDG counter */
	IWDG_ReloadCounter();
	IWDG_Enable(); //Enable IWDG the LSI oscillator will be enabled by hardware
}
