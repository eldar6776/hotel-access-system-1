/**
  * @file    mt9d111.c 
  * @author  WB R&D Team - openmcu666
  * @version V0.1
  * @date    2016.01.09
  * @brief   mt9d111 Camera Driver
  */
#include "ov7670.h"
#include "sccb_bus.h"
#include "stm32f429i_lcd.h"
#include "mt9d111_table.h"

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


	DCMI_InitStructure.DCMI_CaptureMode = DCMI_CaptureMode_Continuous;//DCMI_CaptureMode_SnapShot;//
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
  * @brief  写MT9D111寄存器
  * @param  Page:页数; regID:寄存器;regDat:数据
  * @retval 0:seccess;1:fail
  */
unsigned char mt9d111_write_reg(unsigned char Page,unsigned char regID, unsigned int regDat)
{     
	sccb_bus_start();
	if(0==sccb_bus_write_byte(0xba))
	{
		sccb_bus_stop();
		return(1);
	}
	systick_delay_us(100);
	if(0==sccb_bus_write_byte(0xf0))
	{
		sccb_bus_stop();
		return(1);
	}
	systick_delay_us(100);
	if(0==sccb_bus_write_byte((unsigned char)( Page>>8)))
	{
		sccb_bus_stop();
		return(1);
	}
	if(0==sccb_bus_write_byte((unsigned char)( Page)))
	{
		sccb_bus_stop();
		return(1);
	}
	sccb_bus_stop();
	//////////////////////////////////////////////////////////////     
	sccb_bus_start();
	if(0==sccb_bus_write_byte(0xba))
	{
		sccb_bus_stop();
		return(1);
	}
	systick_delay_us(100);
	if(0==sccb_bus_write_byte(regID))
	{
		sccb_bus_stop();
		return(1);
	}
	systick_delay_us(100);
	if(0==sccb_bus_write_byte((unsigned char)(regDat>>8)))
	{
		sccb_bus_stop();
		return(1);
	}

	if(0==sccb_bus_write_byte((unsigned char)regDat))
	{
		sccb_bus_stop();
		return(1);
	}

	sccb_bus_stop();

	return(0);
}

/**
  * @brief  读MT9D111寄存器
  * @param  Page:页数; reg:寄存器;reg:数据
  * @retval 0:seccess;
  */
uint8_t mt9d111_read_reg(uint8_t Page, uint8_t reg, uint16_t *ret_reg)
{
start:
	systick_delay_us(10);
	//通过写操作设置寄存器地址
	sccb_bus_start();
	if(sccb_bus_write_byte(0xba) == 0)//写地址
	{
		sccb_bus_stop();//发送SCCB 总线停止传输命令
		goto start;
		
	}
	systick_delay_us(10);
	if(sccb_bus_write_byte(0xf0)==0)//ID
	{
		sccb_bus_stop();//发送SCCB 总线停止传输命令
		goto start;
		
	}
	systick_delay_us(10);
	if(sccb_bus_write_byte(Page>>8)==0)//ID
	{
		sccb_bus_stop();//发送SCCB 总线停止传输命令
		goto start;
		
	}
	systick_delay_us(10);
	if(sccb_bus_write_byte(Page)==0)//ID
	{
		sccb_bus_stop();//发送SCCB 总线停止传输命令
		goto start;
		
	}
	sccb_bus_stop();//发送SCCB 总线停止传输命令	
	systick_delay_us(10);	
	
	//设置寄存器地址后，才是读
	sccb_bus_start();
	if(sccb_bus_write_byte(0xbb)==0)//读地址
	{
		sccb_bus_stop();//发送SCCB 总线停止传输命令
		goto start;
		
	}
	systick_delay_us(10);
	
	*ret_reg = sccb_bus_read_byte();//返回读到的值
	sccb_bus_send_ack();//发送ACK命令
	*ret_reg <<= 8;
	*ret_reg |= sccb_bus_read_byte();
	
	sccb_bus_send_noack();//发送NACK命令
	sccb_bus_stop();//发送SCCB 总线停止传输命令
	return 0;//成功返回
}

/**
  * @brief  复位MT9D111
  * @param  None
  * @retval None
  */
void mt9d111_reset(void)
{
	mt9d111_write_reg(0, 0x65, 0xa000);
	mt9d111_write_reg(1, 0xc3, 0x0501);
	mt9d111_write_reg(0, 0x0d, 0x0021);
	mt9d111_write_reg(0, 0x0d, 0x0000);
	_delay(1000);

}



/**
  * @brief  操作部分位
  * @param  None
  * @retval None
  */
void wrMT9D111RegBit(u8 Page, u8 regID, u16 C_Dat,u16 W_Dat)
{
	u16 temp;
	mt9d111_read_reg(Page,regID,&temp);
	temp &=~C_Dat;
	temp |= W_Dat; 
	mt9d111_write_reg(Page,regID,temp);
}

/**
  * @brief  写变量
  * @param  None
  * @retval None
  */
void wrMT9D111Variables(u8 ID,u8 offset, u8 isLogical,	u8 Size, u16 value)
{
	u16 temp;
	temp = (ID<<8)|(offset)|(isLogical<<13)|(Size<<15);
	mt9d111_write_reg(1,198,temp);
	mt9d111_write_reg(1,200,value);
}

/**
  * @brief  读变量
  * @param  None
  * @retval None
  */
void rdMT9D111Variables(u8 ID,u8 offset, u8 isLogical,	u8 Size, u16* value)
{
	u16 temp;
	temp = (ID<<8)|(offset)|(isLogical<<13)|(Size<<15);
	mt9d111_write_reg(1,198,temp);
	mt9d111_read_reg(1,200,value);
}

/**
  * @brief  初始化MT9D111
  * @param  None
  * @retval 0:seccess
  */
uint8_t mt9d111_init(void)
{
	u16  i;							 
	port_dma_init();
	sccb_bus_init();
	mt9d111_reset();
	systick_delay_ms(5);
	
	for(i=0;i<154;i++)//CHANGE_REG_NUM//(sizeof(mt9d111_QVGA))/3	154
	{
		if(mt9d111_QVGA[i][0]>2)
		{
			systick_delay_ms(5);//(mt9d111_QVGA[i][2]); // Delay 5ms();
		}
		else
		{
			if( 0==mt9d111_write_reg(mt9d111_QVGA[i][0],mt9d111_QVGA[i][1],mt9d111_QVGA[i][2]))
			{
//				return 0;
			}
		}
	//delay_us(100);
	}

	mt9d111_write_reg(0,0x66,(58<<8)|(1));
	wrMT9D111RegBit(0,0x67,0x7f,2);
	wrMT9D111RegBit(0,0x65,1<<14,0<<14);

  return 0;	
}

