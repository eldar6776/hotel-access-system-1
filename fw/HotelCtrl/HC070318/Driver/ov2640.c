/**
  * @file    ov2640.c 
  * @author  WB R&D Team - openmcu666
  * @version V0.1
  * @date    2015.6.24
  * @brief   OV2640 Camera Driver
  */
#include "ov2640.h"
#include "sccb_bus.h"
#include "ov2640_table.h"
#include "stm32f429i_lcd.h"
#include "CamConfig.h"
extern uint32_t LCD_BUF[76800];

static void _delay(uint32_t dl)
{
	unsigned int i,y;
	for(i = 0; i < 1000; i++)
	{
		for(y = 0; y < dl; y++);
	}
}
/**
  * @brief  DCMI 和 DMA 初始化
  * @param  None
  * @retval None
  */
static void port_dma_init(void)
{
	DCMI_InitTypeDef DCMI_InitStructure;
	DMA_InitTypeDef  DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI, ENABLE);//DCMI 
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);//DMA2
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | 
	                       RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOE |
	                       RCC_AHB1Periph_GPIOI |RCC_AHB1Periph_GPIOH, ENABLE);                                      
	
	GPIO_PinAFConfig(GPIOH, GPIO_PinSource8, GPIO_AF_DCMI);//DCMI_HSYNC 
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_DCMI);//DCMI_PIXCLK  
  GPIO_PinAFConfig(GPIOI, GPIO_PinSource4, GPIO_AF_DCMI);//DCMI_D5 	
	GPIO_PinAFConfig(GPIOI, GPIO_PinSource5, GPIO_AF_DCMI);//DCMI_VSYNC 
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_DCMI);//DCMI_D6 
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource6, GPIO_AF_DCMI);//DCMI_D7 
  GPIO_PinAFConfig(GPIOH, GPIO_PinSource9, GPIO_AF_DCMI);//DCMI_D0		
  GPIO_PinAFConfig(GPIOH, GPIO_PinSource10, GPIO_AF_DCMI);//DCMI_D1
	GPIO_PinAFConfig(GPIOH, GPIO_PinSource11, GPIO_AF_DCMI);//DCMI_D2 	
	GPIO_PinAFConfig(GPIOH, GPIO_PinSource12, GPIO_AF_DCMI);//DCMI_D3 
	GPIO_PinAFConfig(GPIOH, GPIO_PinSource14, GPIO_AF_DCMI);//DCMI_D4 

	/* DCMI GPIO configuration ************************************************* */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9|GPIO_Pin_10 | GPIO_Pin_11|
	                                  GPIO_Pin_12 | GPIO_Pin_14;
	GPIO_Init(GPIOH, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOI, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5|GPIO_Pin_6;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	DCMI_InitStructure.DCMI_CaptureMode = DCMI_CaptureMode_Continuous;
	DCMI_InitStructure.DCMI_SynchroMode = DCMI_SynchroMode_Hardware;
	DCMI_InitStructure.DCMI_PCKPolarity = DCMI_PCKPolarity_Rising;
	DCMI_InitStructure.DCMI_VSPolarity = DCMI_VSPolarity_Low;
	DCMI_InitStructure.DCMI_HSPolarity = DCMI_HSPolarity_Low;
	DCMI_InitStructure.DCMI_CaptureRate = DCMI_CaptureRate_All_Frame;
	DCMI_InitStructure.DCMI_ExtendedDataMode = DCMI_ExtendedDataMode_8b;
	DCMI_Init(&DCMI_InitStructure); 
	/* DCMI Interrupts config ***************************************************/
	//DCMI_ITConfig(DCMI_IT_VSYNC, ENABLE);	
  DCMI_ITConfig(DCMI_IT_FRAME,ENABLE);//开启帧中断 
	
	NVIC_InitStructure.NVIC_IRQChannel = DCMI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
	NVIC_Init(&NVIC_InitStructure); 


	DMA_DeInit(DMA2_Stream1);
	DMA_InitStructure.DMA_Channel = DMA_Channel_1;  
	DMA_InitStructure.DMA_PeripheralBaseAddr = DCMI_DR_ADDRESS;	
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)LCD_BUF;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = 38400;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc =DMA_MemoryInc_Enable;//使能内存地址自增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  
	DMA_Init(DMA2_Stream1, &DMA_InitStructure);
	
	DMA_ITConfig(DMA2_Stream1, DMA_IT_TC, ENABLE);
	/* Enable the DMA Stream IRQ Channel */
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
}

/**
  * @brief  写OV2640寄存器
  * @param  reg:要写入的寄存器;data:写入的数据
  * @retval 0:success other:error
  */
uint8_t ov2640_write_reg(uint8_t reg, uint8_t data)
{
//start:
	systick_delay_us(10);
	sccb_bus_start();                          //发送SCCB 总线开始传输命令
	if(sccb_bus_write_byte(0x60) == 0)         //写地址
	{
		sccb_bus_stop();                         //发送SCCB 总线停止传输命令
	//	goto start;
		return 1;//错误返回
	}
	systick_delay_us(10);
	if(sccb_bus_write_byte(reg) == 0)//ID
	{
		sccb_bus_stop();                                 //发送SCCB 总线停止传输命令
	//	goto start;
		return 2;                                        //错误返回
	}
	systick_delay_us(10);
	if(sccb_bus_write_byte(data)==0)                    //写数据到积存器
	{
		sccb_bus_stop();                                  //发送SCCB 总线停止传输命令
		//goto start;
		return 3;//错误返回
	}
	sccb_bus_stop();                                    //发送SCCB 总线停止传输命令	
	return 0;//成功返回
}

/**
  * @brief  读OV2640寄存器
  * @param  reg:要读取的寄存器;ret_reg:指向读取的数据
  * @retval 0:success other:error
            
  */
uint8_t ov2640_read_reg(uint8_t reg, uint8_t *ret_reg)
{
//start:
	systick_delay_us(10);
	
	sccb_bus_start();
	if(sccb_bus_write_byte(0x60) == 0)                 //写地址
	{
		sccb_bus_stop();                                  //发送SCCB 总线停止传输命令
		//goto start;
		return 1;                                         //错误返回
	}
	systick_delay_us(10);
	if(sccb_bus_write_byte(reg)==0)//ID
	{
		sccb_bus_stop();                                   //发送SCCB 总线停止传输命令
		//goto start;
		return 2;                                          //错误返回
	}
	sccb_bus_stop();                                     //发送SCCB 总线停止传输命令	
	systick_delay_us(10);	
	//设置寄存器地址后，才是读
	sccb_bus_start();
	if(sccb_bus_write_byte(0x61)==0)                      //读地址
	{
		sccb_bus_stop();                                    //发送SCCB 总线停止传输命令
		//goto start;
		return 3;                                           //错误返回
	}
	systick_delay_us(10);
	*ret_reg = sccb_bus_read_byte();                      //返回读到的值
	sccb_bus_send_noack();                                //发送NACK命令
	sccb_bus_stop();                                      //发送SCCB 总线停止传输命令
	return 0;                                             //成功返回
}

/**
  * @brief  software reset
  * @param  None
  * @retval None
  */
void ov2640_reset(void)
{
	ov2640_write_reg(0xFF, 0x01);
	ov2640_write_reg(0x12, 0x80);
	_delay(1000);
}

/**
  * @brief  读取ID
  * @param  None
  * @retval temp
           -Camera器件ID
  */
uint16_t ov2640_get_id(void)
{
	uint8_t pidh=0,pidl=0;
	uint16_t temp=0;
	ov2640_write_reg(0xff, 0x01);
	ov2640_read_reg(0x0a,&pidh);
	ov2640_read_reg(0x0b,&pidl);
	temp=(uint16_t)(((uint16_t)(pidh<<8)) | pidl);

	return temp;
}

/**
  * @brief  MCO配置(PA8输出24Mhz给Camera)
  * @param  None
  * @retval None
  */
static void mco_config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
  /* MCO configure */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_MCO);
  RCC_MCO1Config(RCC_MCO1Source_HSE, RCC_MCO1Div_1);
}

/**
  * @brief  读取ID
  * @param  None
  * @retval ovid:器件ID
  */
uint16_t OV2640_Check(void)
{
	uint16_t ovid;
#ifdef USE_MCO
	mco_config();
#endif
	port_dma_init();
	sccb_bus_init();

	ov2640_reset();
	systick_delay_ms(5);
	
	ovid = ov2640_get_id();
	return ovid; 
}


/**
  * @brief  Configures the OV2640 camera brightness.
  * @param  Brightness: Brightness value, where Brightness can be: 
  *         0x40 for Brightness +2,
  *         0x30 for Brightness +1,
  *         0x20 for Brightness 0,
  *         0x10 for Brightness -1,
  *         0x00 for Brightness -2,
  * @retval None
  */
static void OV2640_BrightnessConfig(uint8_t Brightness)
{
  ov2640_write_reg(0xff, 0x00);
  ov2640_write_reg(0x7c, 0x00);
  ov2640_write_reg(0x7d, 0x04);
  ov2640_write_reg(0x7c, 0x09);
  ov2640_write_reg(0x7d, Brightness);
  ov2640_write_reg(0x7d, 0x00);
}

/**
  * @brief  Configures the OV2640 camera Black and white mode.
  * @param  BlackWhite: BlackWhite value, where BlackWhite can be: 
  *         0x18 for B&W,
  *         0x40 for Negative,
  *         0x58 for B&W negative,
  *         0x00 for Normal,
  * @retval None
  */
void OV2640_BandWConfig(uint8_t BlackWhite)
{
  ov2640_write_reg(0xff, 0x00);
  ov2640_write_reg(0x7c, 0x00);
  ov2640_write_reg(0x7d, BlackWhite);
  ov2640_write_reg(0x7c, 0x05);
  ov2640_write_reg(0x7d, 0x80);
  ov2640_write_reg(0x7d, 0x80);
}

/**
  * @brief  Configures the OV2640 camera color effects.
  * @param  value1: Color effects value1
  * @param  value2: Color effects value2
  *         where value1 and value2 can be: 
  *         value1 = 0x40, value2 = 0xa6 for Antique,
  *         value1 = 0xa0, value2 = 0x40 for Bluish,
  *         value1 = 0x40, value2 = 0x40 for Greenish,
  *         value1 = 0x40, value2 = 0xc0 for Reddish,
  * @retval None
  */
void OV2640_ColorEffectsConfig(uint8_t value1, uint8_t value2)
{
  ov2640_write_reg(0xff, 0x00);
  ov2640_write_reg(0x7c, 0x00);
  ov2640_write_reg(0x7d, 0x18);
	ov2640_write_reg(0x7c, 0x05);
	ov2640_write_reg(0x7d, value1);
  ov2640_write_reg(0x7d, value2);
}

/**
  * @brief  Configures the OV2640 camera contrast.
  * @param  value1: Contrast value1
  * @param  value2: Contrast value2
  *         where value1 and value2 can be: 
  *         value1 = 0x28, value2 = 0x0c for Contrast +2,
  *         value1 = 0x24, value2 = 0x16 for Contrast +1,
  *         value1 = 0x20, value2 = 0x20 for Contrast 0,
  *         value1 = 0x1c, value2 = 0x2a for Contrast -1,
  *         value1 = 0x18, value2 = 0x34 for Contrast -2,
  * @retval None
  */
void OV2640_ContrastConfig(uint8_t value1, uint8_t value2)
{
  ov2640_write_reg(0xff, 0x00);
  ov2640_write_reg(0x7c, 0x00);
  ov2640_write_reg(0x7d, 0x04);
  ov2640_write_reg(0x7c, 0x07);
  ov2640_write_reg(0x7d, 0x20);
  ov2640_write_reg(0x7d, value1);
  ov2640_write_reg(0x7d, value2);
  ov2640_write_reg(0x7d, 0x06);
}

/**
  * @brief  Configures the OV2640 camera color saturation.
  * @param  value1: Contrast value1
  * @param  value2: Contrast value2
  *         where value1 and value2 can be: 
  *         value1 = 0x68, value2 = 0x68 for saturation +2,
  *         value1 = 0x58, value2 = 0x58 for saturation +1,
  *         value1 = 0x48, value2 = 0x48 for saturation 0,
  *         value1 = 0x38, value2 = 0x38 for saturation -1,
  *         value1 = 0x28, value2 = 0x28 for saturation -2,
  * @retval None
  */
void OV2640_SaturationConfig(uint8_t value1, uint8_t value2)
{
	ov2640_write_reg(0xff, 0x00);
  ov2640_write_reg(0x7c, 0x00);
	ov2640_write_reg(0x7d, 0x02);
  ov2640_write_reg(0x7c, 0x03);
	ov2640_write_reg(0x7d, value1);
  ov2640_write_reg(0x7d, value2);
}

uint8_t OV2640_Init(void)
{
	u32 i;
	ov2640_write_reg(0xff, 0x01);
  ov2640_write_reg(0x12, 0x80);
	systick_delay_us(400);

	for(i=0;i<(sizeof(OV2640_QVGA))/2;i++)
	{
		ov2640_write_reg(OV2640_QVGA[i][0], OV2640_QVGA[i][1]);
		systick_delay_us(20);
	}

	OV2640_BrightnessConfig(0x20);         //白平衡

	OV2640_SaturationConfig(0x48,0x48);	   //颜色饱和度
	
	ov2640_write_reg(0xff, 0x00);          //拍照环境设置
  ov2640_write_reg(0xc7, 0x40);
	ov2640_write_reg(0xcc, 0x52);
  ov2640_write_reg(0xcd, 0x41);
  ov2640_write_reg(0xce, 0x66);
	
	OV2640_ContrastConfig(0x20, 0x20);     //对比度设置
	
	OV2640_BandWConfig(0x00);
	
	return 0;	
}


