/**
 ******************************************************************************
 * File Name          : dio.c
 * Date               : 28/02/2016 23:16:19
 * Description        : digital io interface software modul
 ******************************************************************************
 *
 *
 ******************************************************************************
 */
 
/* Includes ------------------------------------------------------------------*/
#include "dio.h"
#include "stepper.h"

/* Variables  ----------------------------------------------------------------*/
volatile uint8_t dout_0_7;
volatile uint8_t dout_8_15;
volatile uint8_t dout_0_7_old;
volatile uint8_t dout_8_15_old;
volatile uint16_t green_signal_timer;
volatile uint16_t red_signal_timer;

/* Defines    ----------------------------------------------------------------*/
#define I2C_EXPANDER_1_ADDRESS	0x70
#define I2C_EXPANDER_2_ADDRESS	0x72

/* Types  --------------------------------------------------------------------*/
/* Macros     ----------------------------------------------------------------*/
/* Private prototypes    -----------------------------------------------------*/
extern int16_t I2C_EEPROM_timeout(int16_t time);
extern void I2C2_Init(void);
void DIO_WriteByte(uint8_t slave_address, uint8_t data);

/* Program code   ------------------------------------------------------------*/
void DIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
//	EXTI_InitTypeDef EXTI_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;

//-----------------------------------------------------------	onboard leds	

	RCC_AHB1PeriphClockCmd(LED_GPIO_CLK , ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN | LED2_GPIO_PIN;
	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);
	GPIO_SetBits(LED_GPIO_PORT,LED1_GPIO_PIN|LED2_GPIO_PIN);

//-----------------------------------------------------------	onboard buzzer

	RCC_AHB1PeriphClockCmd(BEEP_GPIO_CLK , ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN;
	GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN);
	
//-----------------------------------------------------------	stacker step motor

//	RCC_AHB1PeriphClockCmd(STACKER_M1_CLK , ENABLE);
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//	GPIO_InitStructure.GPIO_Pin = STACKER_M1_PIN;
//	GPIO_Init(STACKER_M1_PORT, &GPIO_InitStructure);
//	GPIO_ResetBits(STACKER_M1_PORT, STACKER_M1_PIN);
//	
//	RCC_AHB1PeriphClockCmd(STACKER_M2_CLK , ENABLE);
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//	GPIO_InitStructure.GPIO_Pin = STACKER_M2_PIN | STACKER_M3_PIN;
//	GPIO_Init(STACKER_M2_PORT, &GPIO_InitStructure);
//	GPIO_ResetBits(STACKER_M2_PORT, STACKER_M2_PIN | STACKER_M3_PIN);
//	
//	RCC_AHB1PeriphClockCmd(STACKER_M4_CLK , ENABLE);
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//	GPIO_InitStructure.GPIO_Pin = STACKER_M4_PIN;
//	GPIO_Init(STACKER_M4_PORT, &GPIO_InitStructure);
//	GPIO_ResetBits(STACKER_M4_PORT, STACKER_M4_PIN);
//	
////-----------------------------------------------------------	barrier motor clock output

//	RCC_AHB1PeriphClockCmd(BARRIER_MOTOR_CLOCK_CLK , ENABLE);
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//	GPIO_InitStructure.GPIO_Pin = BARRIER_MOTOR_CLOCK_PIN;
//	GPIO_Init(BARRIER_MOTOR_CLOCK_PORT, &GPIO_InitStructure);
//	GPIO_ResetBits(BARRIER_MOTOR_CLOCK_PORT, BARRIER_MOTOR_CLOCK_PIN);
	
	
	
	RCC_AHB1PeriphClockCmd(TASTER_S2_CLK|TASTER_S3_CLK|STACKER_SENSOR_CLK|HALL_CW_DIR_CLK|HALL_CW_DIR_CLK|HALL_10_DEGREE_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	
//-----------------------------------------------------------	onboard taster 2 and 3

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = TASTER_S2_PIN;
	GPIO_Init(TASTER_S2_PORT, &GPIO_InitStructure);                   

//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_InitStructure.GPIO_Pin = TASTER_S3_PIN;
//	GPIO_Init(TASTER_S3_PORT, &GPIO_InitStructure);
	
//-----------------------------------------------------------	digital input stacker sensors
//	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	GPIO_InitStructure.GPIO_Pin = SENSOR_CARD_LOADED_PIN | SENSOR_CARD_INSERTED_PIN;
//	GPIO_Init(STACKER_SENSOR_PORT, &GPIO_InitStructure);

//-----------------------------------------------------------	barrier motor cw direction sensors

//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//	GPIO_InitStructure.GPIO_Pin = HALL_CW_DIR_PIN;
//	GPIO_Init(HALL_CW_DIR_PORT, &GPIO_InitStructure);
	
//-----------------------------------------------------------	barrier motor ccw direction sensors

//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//	GPIO_InitStructure.GPIO_Pin = HALL_CCW_DIR_PIN;
//	GPIO_Init(HALL_CCW_DIR_PORT, &GPIO_InitStructure);
	
//-----------------------------------------------------------	barrier motor position sensor

//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//	GPIO_InitStructure.GPIO_Pin = HALL_10_DEGREE_PIN;
//	GPIO_Init(HALL_10_DEGREE_PORT, &GPIO_InitStructure);
	
//-----------------------------------------------------------	taster 3 irq

//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource11);
//	EXTI_InitStructure.EXTI_Line = EXTI_Line11;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;          
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);

//	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);

//-----------------------------------------------------------	taster 2 irq

//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
//	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);

//	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	
//-----------------------------------------------------------	sensor card loaded irq

//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);
//	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);

//	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	
//-----------------------------------------------------------	sensor card inserted irq
	
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);
//	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);

//	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	
//-----------------------------------------------------------	hall cw dir irq
	
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource15);
//	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);

//	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	
//-----------------------------------------------------------	hall ccw dir irq
	
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource5);
//	EXTI_InitStructure.EXTI_Line = EXTI_Line5;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);

//	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	
//-----------------------------------------------------------	hall 10 degree irq
	
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource6);
//	EXTI_InitStructure.EXTI_Line = EXTI_Line6;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);

//	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);

//	dout_0_7 = 0xff;
//	dout_0_7_old = 0;
//	dout_8_15 = 0xff;
//	dout_8_15_old = 0;
//	BarrierDriveVref1Release();
//	BarrierDriveEnableRelease();
//	BarrierDriveClockRelease();
//	BarrierDriveResetAssert();
//	BarrierDriveResetRelease();
//	EntryRedSignal_On();
//	ExitRedSignal_On();
//	DOUT_Service();
//	GPIO_SetBits(BARRIER_MOTOR_CLOCK_PORT, BARRIER_MOTOR_CLOCK_PIN);
}


void DOUT_Service(void)
{
	if(dout_0_7_old != dout_0_7) 
	{
		DIO_WriteByte(I2C_EXPANDER_1_ADDRESS, dout_0_7);
		dout_0_7_old = dout_0_7;
	}
	
	if(dout_8_15_old != dout_8_15)
	{
		DIO_WriteByte(I2C_EXPANDER_2_ADDRESS, dout_8_15);
		dout_8_15_old = dout_8_15;
	}
}


/**
  * @brief  翻转端口电平
  * @param  选择相应LED 1-LED1 2-LED2
  * @retval None
  */


void DIO_WriteByte(uint8_t slave_address, uint8_t data)
{
	uint32_t timeout=DIO_TIMEOUT;
	
	I2C_GenerateSTART(I2C2, ENABLE);
	timeout = DIO_TIMEOUT;
	
	while (!I2C_GetFlagStatus(I2C2, I2C_FLAG_SB)) 
	{
		if(timeout != 0) timeout--; 
		else return;
	} 

	I2C_Send7bitAddress(I2C2, slave_address, I2C_Direction_Transmitter);
	timeout = DIO_TIMEOUT;
	
	while (!I2C_GetFlagStatus(I2C2, I2C_FLAG_ADDR)) 
	{
		if(timeout != 0) timeout--;  
		else return;
	}  

	I2C2->SR2;
	timeout = DIO_TIMEOUT;
	
	while (!I2C_GetFlagStatus(I2C2, I2C_FLAG_TXE)) 
	{
		if(timeout != 0) timeout--; 
		else return;
	}

	I2C_SendData(I2C2, data);
	timeout = DIO_TIMEOUT;
	
	while ((!I2C_GetFlagStatus(I2C2, I2C_FLAG_TXE)) || (!I2C_GetFlagStatus(I2C2, I2C_FLAG_BTF))) 
	{
		if(timeout != 0) timeout--; 
		else return;
	}

	I2C_GenerateSTOP(I2C2, ENABLE);
}


/******************************   END OF FILE  **********************************/
