/**
  * @file    ov5642.c 
  * @author  WB R&D Team - openmcu666
  * @version V0.1
  * @date    2015.11.09
  * @brief   OV5642 Camera Driver
  */
#include "stdio.h"
#include "ov5642.h"
#include "sccb_bus.h"
#include "stm32f429i_lcd.h"
#include "ov5642_table.h"
#include "CamConfig.h"
extern uint32_t LCD_BUF[76800];

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

	DCMI_InitStructure.DCMI_CaptureMode =DCMI_CaptureMode_Continuous;
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
  * @brief  写OV5642寄存器
  * @param  regID:要写入的寄存器;regDat:写入的数据
  * @retval 1:success 0:error
  */
u8 ov5642_write_reg(u16 regID, u8 regDat)
{
	sccb_bus_start();
	if(0==sccb_bus_write_byte(0x78))//
	{
		sccb_bus_stop();
		return(0);
	}
	systick_delay_us(20);
  if(0==sccb_bus_write_byte(regID>>8))
	{
		sccb_bus_stop();
		return(0);
	}
	systick_delay_us(20);
  if(0==sccb_bus_write_byte(regID))
	{
		sccb_bus_stop();
		return(0);
	}
	systick_delay_us(20);
  if(0==sccb_bus_write_byte(regDat))
	{
		sccb_bus_stop();
		return(0);
	}
  sccb_bus_stop();
	
  return(1);
}

/**
  * @brief  读OV5642寄存器
  * @param  regID:要写入的寄存器;*regDat:指向读取的数据
  * @retval 1:success 0:error
  */
uint8_t ov5642_read_reg(u16 regID, u8 *regDat)
{
	
	sccb_bus_start();                   //通过写操作设置寄存器地址
	if(0==sccb_bus_write_byte(0x78))
	{
		sccb_bus_stop();
		return(0);
	}
	systick_delay_us(20);
	systick_delay_us(20);
  if(0==sccb_bus_write_byte(regID>>8))
	{
		sccb_bus_stop();
		return(0);
	}
	systick_delay_us(20);
  if(0==sccb_bus_write_byte(regID))
	{
		sccb_bus_stop();
		return(0);
	}
	systick_delay_us(20);
	sccb_bus_stop();
	
	systick_delay_us(20);
	
	
	sccb_bus_start();                  //设置寄存器地址后，才是读
	if(0==sccb_bus_write_byte(0x79))
	{
		sccb_bus_stop();
		return(0);
	}
	systick_delay_us(20);
  *regDat=sccb_bus_read_byte();
  sccb_bus_send_noack();
  sccb_bus_stop();
  return(1);
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

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);             //Enable SYSCFG clock 
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;                          // MCO configure 
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_MCO);
  RCC_MCO1Config(RCC_MCO1Source_HSE,RCC_MCO1Div_1 );
}

#define  write_i2c ov5642_write_reg

unsigned char jpeg_Preview(void)
{

  /*
    write_i2c(0x3000 ,0x00); 
		write_i2c(0x3001 ,0x00); 
		write_i2c(0x3002 ,0x00); 
		write_i2c(0x3003 ,0x00);*/ 
		write_i2c(0x3005 ,0xff); 
		write_i2c(0x3006 ,0xff); 
		write_i2c(0x3007 ,0x3f); 

		write_i2c(0x350c ,0x07); 
		write_i2c(0x350d ,0xd0); 
		write_i2c(0x3602 ,0xe4); 
		write_i2c(0x3612 ,0xac); 
		write_i2c(0x3613 ,0x44); 
		write_i2c(0x3621 ,0x27); 
		write_i2c(0x3622 ,0x08); 
		write_i2c(0x3623 ,0x22); 
		write_i2c(0x3604 ,0x60); 
		write_i2c(0x3705 ,0xda); 
		write_i2c(0x370a ,0x80);
				 
		write_i2c(0x3808 ,0x02); 
		write_i2c(0x3809 ,0x80);
		write_i2c(0x380a ,0x01);
		write_i2c(0x380b ,0xe0); 
		write_i2c(0x4602 ,0x02); 
		write_i2c(0x4603 ,0x80);
		write_i2c(0x4604 ,0x01);
		write_i2c(0x4605 ,0xe0); 
				 
		write_i2c(0x3800 ,0x1 );
		write_i2c(0x3801 ,0x8A);
		write_i2c(0x3802 ,0x0 );
		write_i2c(0x3803 ,0xA ); 
		write_i2c(0x3804 ,0xA ), 
		write_i2c(0x3805 ,0x20);
		write_i2c(0x3806 ,0x7 ); 
		write_i2c(0x3807 ,0x98); 
				
#ifdef QVGA       //qvga out
		write_i2c(0x3808 ,0x01); 
		write_i2c(0x3809 ,0x40);
		write_i2c(0x380a ,0x00);
		write_i2c(0x380b ,0xf0); 	
		  
#else        //vga
		write_i2c(0x3808 ,0x2 );
		write_i2c(0x3809 ,0x80);
		write_i2c(0x380a ,0x1 );
		write_i2c(0x380b ,0xe0);
#endif


		write_i2c(0x380c ,0xc );
		write_i2c(0x380d ,0x80);
		write_i2c(0x380e ,0x7 ); 
		write_i2c(0x380f ,0xd0); 
		write_i2c(0x5001 ,0x7f);
		write_i2c(0x5680 ,0x0 );
		write_i2c(0x5681 ,0x0 );
		write_i2c(0x5682 ,0xA );
		write_i2c(0x5683 ,0x20);
		write_i2c(0x5684 ,0x0 );
		write_i2c(0x5685 ,0x0 );
		write_i2c(0x5686 ,0x7 );
		write_i2c(0x5687 ,0x98); 

		write_i2c(0x3815 ,0x4A); 
		write_i2c(0x3818 ,0xc8); 
		write_i2c(0x3824 ,0x01); 
		write_i2c(0x3827 ,0x0a); 
		write_i2c(0x3a00 ,0x78);
		write_i2c(0x3a0d ,0x10); 
		write_i2c(0x3a0e ,0x0d); 
		write_i2c(0x3a10 ,0x32); 
		write_i2c(0x3a1b ,0x3c); 
		write_i2c(0x3a1e ,0x32); 
		write_i2c(0x3a11 ,0x80); 
		write_i2c(0x3a1f ,0x20); 
		write_i2c(0x3a00 ,0x78); 
		write_i2c(0x460b ,0x35); 
		write_i2c(0x471d ,0x00); 
		write_i2c(0x4713 ,0x03);  //压缩模式3
		write_i2c(0x471c ,0x50); 
		write_i2c(0x5682 ,0x0a);
		write_i2c(0x5683 ,0x20); 
		write_i2c(0x5686 ,0x07); 
		write_i2c(0x5687 ,0x98);
		write_i2c(0x5001 ,0x7f);
		write_i2c(0x589b ,0x00);
		write_i2c(0x589a ,0xc0); 
		write_i2c(0x4407 ,0x04);
		write_i2c(0x589b ,0x00);
		write_i2c(0x589a ,0xc0);
		write_i2c(0x3002 ,0x0c);
		write_i2c(0x3002 ,0x00); 
		write_i2c(0x3503 ,0x00);

		//Contrast +2  ??
		write_i2c(0x5001 ,0xff);  
		write_i2c(0x5580 ,0x04);  
		write_i2c(0x5587 ,0x28); 
		write_i2c(0x5588 ,0x28); 
		write_i2c(0x558a ,0x00);  
		///Saturation 1 
		write_i2c(0x5001 ,0xff);  
		write_i2c(0x5583 ,0x50); 
		write_i2c(0x5584 ,0x50); 
		write_i2c(0x5580 ,0x02);  
		//Brightness +1 
		write_i2c(0x5001 ,0xff);  
		write_i2c(0x5589 ,0x10);
		write_i2c(0x5580 ,0x04);  
		write_i2c(0x558a ,0x00); 
		//Advanced AWB  
		write_i2c(0x3406 ,0x0 );
		write_i2c(0x5192 ,0x04); 
		write_i2c(0x5191 ,0xf8); 
		write_i2c(0x518d ,0x26);
		write_i2c(0x518f ,0x42);
		write_i2c(0x518e ,0x2b); 
		write_i2c(0x5190 ,0x42); 
		write_i2c(0x518b ,0xd0);
		write_i2c(0x518c ,0xbd);                            
		write_i2c(0x5187 ,0x18);
		write_i2c(0x5188 ,0x18);
		write_i2c(0x5189 ,0x56); 
		write_i2c(0x518a ,0x5c); 
		write_i2c(0x5186 ,0x1c); 
		write_i2c(0x5181 ,0x50); 
		write_i2c(0x5184 ,0x20);
		write_i2c(0x5182 ,0x11);
		write_i2c(0x5183 ,0x0 );
		//ev1.3
		write_i2c(0x3a0f ,0x58); 
		write_i2c(0x3a10 ,0x50); 
	  write_i2c(0x3a11 ,0x91); 
		write_i2c(0x3a1b ,0x58) ;
		write_i2c(0x3a1e ,0x50); 
		write_i2c(0x3a1f ,0x20); 

		//hue +30 degree      
		write_i2c(0x5580 ,0x01); 
		write_i2c(0x5581 ,0x6f); 
		write_i2c(0x5582 ,0x20); 
		write_i2c(0x558a ,0x01);
    return 1 ;
   
}

unsigned char XGA_jpeg_Preview(void)
{
  
		write_i2c(0x3000 ,0x00); 
		write_i2c(0x3001 ,0x00); 
		write_i2c(0x3002 ,0x00); 
		write_i2c(0x3003 ,0x00); 
		write_i2c(0x3005 ,0xff); 
		write_i2c(0x3006 ,0xff); 
		write_i2c(0x3007 ,0x3f); 

		write_i2c(0x350c ,0x07); 
		write_i2c(0x350d ,0xd0); 
		write_i2c(0x3602 ,0xe4); 
		write_i2c(0x3612 ,0xac); 
		write_i2c(0x3613 ,0x44); 
		write_i2c(0x3621 ,0x27); 
		write_i2c(0x3622 ,0x08); 
		write_i2c(0x3623 ,0x22); 
		write_i2c(0x3604 ,0x60); 
		write_i2c(0x3705 ,0xda); 
		write_i2c(0x370a ,0x80);  

		write_i2c(0x3808 ,0x04); 
		write_i2c(0x3809 ,0x00);
		write_i2c(0x380a ,0x03);
		write_i2c(0x380b ,0x00); 
		write_i2c(0x4602 ,0x04); 
		write_i2c(0x4603 ,0x00);
		write_i2c(0x4604 ,0x03);
		write_i2c(0x4605 ,0x00); 

		write_i2c(0x3800 ,0x1 ); 
		write_i2c(0x3801 ,0x8A);
		write_i2c(0x3802 ,0x0 ); 
		write_i2c(0x3803 ,0xA ); 
		write_i2c(0x3804 ,0xA ); 
		write_i2c(0x3805 ,0x20); 
		write_i2c(0x3806 ,0x7 ); 
		write_i2c(0x3807 ,0x98); 
		write_i2c(0x3808 ,0x4 ); 
		write_i2c(0x3809 ,0x0 ); 
		write_i2c(0x380a ,0x3 ); 
		write_i2c(0x380b ,0x0 ); 
		write_i2c(0x380c ,0xc ); 
		write_i2c(0x380d ,0x80);
		write_i2c(0x380e ,0x7 ); 
		write_i2c(0x380f ,0xd0); 
		write_i2c(0x5001 ,0x7f); 
		write_i2c(0x5680 ,0x0 ); 
		write_i2c(0x5681 ,0x0 ); 
		write_i2c(0x5682 ,0xA ); 
		write_i2c(0x5683 ,0x20); 
		write_i2c(0x5684 ,0x0 ); 
		write_i2c(0x5685 ,0x0 ); 
		write_i2c(0x5686 ,0x7 ); 
		write_i2c(0x5687 ,0x98); 

		write_i2c(0x3815 ,0x4A); 
		write_i2c(0x3818 ,0xc8); 
		write_i2c(0x3824 ,0x01); 
		write_i2c(0x3827 ,0x0a); 
		write_i2c(0x3a00 ,0x78);
		write_i2c(0x3a0d ,0x10); 
		write_i2c(0x3a0e ,0x0d); 
		write_i2c(0x3a10 ,0x32); 
		write_i2c(0x3a1b ,0x3c); 
		write_i2c(0x3a1e ,0x32); 
		write_i2c(0x3a11 ,0x80); 
		write_i2c(0x3a1f ,0x20); 
		write_i2c(0x3a00 ,0x78); 
		write_i2c(0x460b ,0x35); 
		write_i2c(0x471d ,0x00); 
		write_i2c(0x4713 ,0x03);
		write_i2c(0x471c ,0x50); 
		write_i2c(0x5682 ,0x0a);
		write_i2c(0x5683 ,0x20); 
		write_i2c(0x5686 ,0x07); 
		write_i2c(0x5687 ,0x98);
		write_i2c(0x5001 ,0x7f);
		write_i2c(0x589b ,0x00);
		write_i2c(0x589a ,0xc0); 
		write_i2c(0x4407 ,0x04);
		write_i2c(0x589b ,0x00);
		write_i2c(0x589a ,0xc0);
		write_i2c(0x3002 ,0x0c);
		write_i2c(0x3002 ,0x00); 
		write_i2c(0x3503 ,0x00);
    return 1 ;
}
/**
  * @brief  OV5642初始化
  * @param  None
  * @retval 0:success 1:error
  */
uint8_t ov5642_init(void)
{	
	uint16_t i;
	
#ifdef USE_MCO
	mco_config();
#endif									 	
	port_dma_init();
	sccb_bus_init();
	systick_delay_ms(5);


// printf("\r\n%d\r\n",(sizeof(OV5642_reg)/4));

  for(i = 0; i <sizeof(OV5642_reg)/4; i++)
	{
		if(ov5642_write_reg(OV5642_reg[i][0],OV5642_reg[i][1])==0)
		{
			return 1;
		}
	}

	//jpeg_Preview();
	//XGA_jpeg_Preview();

  return 0;	
}
