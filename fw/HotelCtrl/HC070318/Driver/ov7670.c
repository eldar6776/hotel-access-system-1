/**
  * @file    ov7670.c 
  * @author  WB R&D Team - openmcu666
  * @version V0.1
  * @date    2015.6.23
  * @brief   OV7670 Camera Driver
  */
#include "ov7670.h"
#include "sccb_bus.h"
#include "CamConfig.h"
#include "ov7670_table.h"
#include "stm32f429i_lcd.h"
uint32_t LCD_BUF[76800] __attribute__((at(0xD0000000)));

 //uint32_t LCD_BUF[20000];
/**
  * @brief  
  * @param  None
  * @retval None
  */
static void _delay(uint32_t dl)
{
	unsigned int i,y;
	for(i = 0; i < 1000; i++)
	{
		for(y = 0; y < dl; y++);
	}
}
/**
  * @brief  DMA set
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
	RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOI|
	RCC_AHB1Periph_GPIOH, ENABLE);//使能DCMI的GPIO时钟
	

	
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
	DCMI_InitStructure.DCMI_PCKPolarity = DCMI_PCKPolarity_Falling;
	DCMI_InitStructure.DCMI_VSPolarity = DCMI_VSPolarity_High;
	DCMI_InitStructure.DCMI_HSPolarity = DCMI_HSPolarity_High;
	DCMI_InitStructure.DCMI_CaptureRate = DCMI_CaptureRate_All_Frame;//全帧捕获
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
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;          //失能外设地址自增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                   //使能内存地址自增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;       //半字传输
	DMA_InitStructure.DMA_Mode =DMA_Mode_Circular;                            //循环采集模式 
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;                       //设置中断优先级为高
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  
	DMA_Init(DMA2_Stream1, &DMA_InitStructure);

	DMA_ITConfig(DMA2_Stream1, DMA_IT_TC, ENABLE);                             //传输完成中断
	/* Enable the DMA Stream IRQ Channel */
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
}

/**
  * @brief  写OV7670寄存器
  * @param  reg:要写入的寄存器;data:写入的数据
  * @retval 0:success other:error
  */
uint8_t ov7670_write_reg(uint8_t reg, uint8_t data)
{
	sccb_bus_start();                                    //发送SCCB 总线开始传输命令
	if(sccb_bus_write_byte(0x42) == 0)                   //写地址
	{
		sccb_bus_stop();                                   //发送SCCB 总线停止传输命令
		return 1;                                         //错误返回
	}
	systick_delay_us(10);
	if(sccb_bus_write_byte(reg) == 0)//ID
	{
		sccb_bus_stop();                                    //发送SCCB 总线停止传输命令
		return 2;                                          //错误返回
	}
	systick_delay_us(10);
	if(sccb_bus_write_byte(data)==0)                     //写数据到积存器
	{
		sccb_bus_stop();                                   //发送SCCB 总线停止传输命令
		return 3;                                          //错误返回
	}
	sccb_bus_stop();                                     //发送SCCB 总线停止传输命令	
	return 0;                                            //成功返回
}
/**
  * @brief  读OV7670寄存器
  * @param  reg:要读取的寄存器;ret_reg:指向读取的数据
  * @retval 0:success other:error          
  */
uint8_t ov7660_read_reg(uint8_t reg, uint8_t *ret_reg)
{
	//通过写操作设置寄存器地址
	sccb_bus_start();
	if(sccb_bus_write_byte(0x42) == 0)              //写地址
	{
		sccb_bus_stop();                              //发送SCCB 总线停止传输命令
		return 1;                                     //错误返回
	}
	systick_delay_us(10);
	if(sccb_bus_write_byte(reg)==0)//ID
	{
		sccb_bus_stop();                             //发送SCCB 总线停止传输命令
		return 2;                                    //错误返回
	}
	sccb_bus_stop();                               //发送SCCB 总线停止传输命令	
	systick_delay_us(10);	
	                                                //设置寄存器地址后，才是读
	sccb_bus_start();
	if(sccb_bus_write_byte(0x43)==0)                //读地址
	{
		sccb_bus_stop();                              //发送SCCB 总线停止传输命令
		return 3;                                     //错误返回
	}
	systick_delay_us(10);
	*ret_reg = sccb_bus_read_byte();                //返回读到的值
	sccb_bus_send_noack();                          //发送NACK命令
	sccb_bus_stop();                                //发送SCCB 总线停止传输命令
	return 0;                                       //成功返回
}
/**
  * @brief  software reset
  * @param  None
  * @retval None
  */
void ov7670_reset(void)
{
	ov7670_write_reg(0xFF, 0x01);
	ov7670_write_reg(0x12,0x80);
	_delay(1000);
}

/**
  * @brief  读取ID
  * @param  None
  * @retval temp
           -Camera器件ID
  */
uint16_t ov7660_get_id(void)
{
	
	uint8_t pidh=0,pidl=0;
	uint16_t temp=0;
	ov7670_write_reg(0xff, 0x01);
	ov7660_read_reg(0x0a,&pidh);
	ov7660_read_reg(0x0b,&pidl);
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
  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);       //Enable SYSCFG clock 

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;                    // MCO configure 
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_MCO);
    RCC_MCO1Config(RCC_MCO1Source_HSE, RCC_MCO1Div_1);
}

/**
  * @brief  读取ID
  * @param  None
  * @retval ovid:器件ID
  */
uint16_t OV7670_Check(void)
{
	uint16_t ovid;
#ifdef USE_MCO
	mco_config();                                           //output 24MHz
#endif                               
	port_dma_init();                                        //DCMI to LCD_BUF
	sccb_bus_init();
	ov7670_reset();
	systick_delay_ms(5);
	ovid = ov7660_get_id();
	
	return ovid; 
}

/**
  * @brief  开始传输
  * @param  None
  * @retval None
  */
void CameraStart(void)
{	
	DMA_Cmd(DMA2_Stream1, ENABLE); 
	DCMI_Cmd(ENABLE); 
	DCMI_CaptureCmd(ENABLE); 

}

/**
  * @brief  停止传输
  * @param  None
  * @retval None
  */
void cam_stop(void)
{	
	DMA_Cmd(DMA2_Stream1, DISABLE); 
	DCMI_Cmd(DISABLE); 
	DCMI_CaptureCmd(DISABLE); 

}

/**
  * @brief  初始化7670寄存器
  * @param  None
  * @retval 0-success;  1-error
  */
uint8_t OV7670_Init(void)
{
	uint32_t i;

	for(i = 0; i < OV7670_REG_NUM; i++)
	{
		if(ov7670_write_reg(OV7670_reg[i][0],OV7670_reg[i][1]))
		{
			return 1;
		}
	}
	return 0;	
}


