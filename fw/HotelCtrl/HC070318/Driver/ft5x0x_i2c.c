/**
  * @file    ft5x0x_i2c.c 
  * @author  WB R&D Team - openmcu666
  * @version V0.1
  * @date    2015.9.7
  * @brief   Capacitive Touch
  */
#include "stm32f4xx.h"
#include "ft5x0x_i2c.h"
#include "IIC.h"
#include "delay.h"
#include "stdio.h"
struct _ts_event ts_event;

u8 Touch_Val[26]={0};

/**
  * @brief  5306相应IO的初始化 
  * @param  None
  * @retval None
  */
void ft5x0x_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
	/* Enable GPIOs clock */
	RCC_AHB1PeriphClockCmd(IIC_RCC_SCL | IIC_RCC_SDA | RCC_AHB1Periph_GPIOC 
	                        |RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOE, ENABLE);
  //SCL
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;

	GPIO_InitStructure.GPIO_Pin = IIC_PIN_SCL;
	GPIO_Init(IIC_PORT_SCL, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IIC_PIN_SDA;
	GPIO_Init(IIC_PORT_SDA, &GPIO_InitStructure);
	
  //RESET
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	
  /*暂时未加复位*/
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

  //INT
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;                //INT:PC13   
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	//将其配置成中断
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);
  EXTI_InitStructure.EXTI_Line = EXTI_Line13;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//先占优先级2级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //从优先级0级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 
	
	//GPIO_ResetBits(GPIOC,GPIO_Pin_13);//Res
//	IIC_delay( IIC_Delay_COUNT );
//	IIC_delay( IIC_Delay_COUNT );
//	IIC_delay( IIC_Delay_COUNT );

	GPIO_SetBits(GPIOE,GPIO_Pin_7);
	delay_ms(4);
	GPIO_ResetBits(GPIOE,GPIO_Pin_7);
  delay_ms(10);
	GPIO_SetBits(GPIOE,GPIO_Pin_7);     //复位后200ms内不能读数据
  delay_ms(200);
}
/**
  * @brief  中断设置	 
  * @param  1:开中断;0:关中断
  * @retval None
  */
void Pen_Int_Set(u8 en)
{
	if(en)EXTI->IMR|=1<<13;   //开启line13上的中断	  	
	else EXTI->IMR&=~(1<<13); //关闭line13上的中断	   
}	

/**
  * @brief  读取指定长度的数据 
  * @param  *PCTP_Par:指向接收缓冲区;ValFlag:读取个数
  * @retval 
  */
void ft5x0x_readCTP(u8 *PCTP_Par,u8 ValFlag)
{
	int8_t i=0;
	iic_start();
	iic_write_byte(FT5x0x_Read_ADD, 1);
	for(i=0;i<ValFlag;i++)
	{
		if(i==(ValFlag-1))  *(PCTP_Par+i)=iic_read_byte(0);
		else                *(PCTP_Par+i)=iic_read_byte(1);
	}		
	iic_stop();
}

/**
  * @brief  读产品ID
  * @param  RegIndex:寄存器地址
  * @retval ID
  */
u8 ft5x0x_readid(u8 RegIndex)
{
	unsigned char receive=0;
	iic_start();
	iic_write_byte(FT5x0x_Write_ADD,1);
	iic_write_byte(RegIndex,1);
  iic_start();
	iic_write_byte(FT5x0x_Read_ADD,1);
	receive=iic_read_byte(0);
	iic_stop();
	printf("DEVICE ID=%x\n",receive); 
	return receive;
}

/**
  * @brief  向5306写data
  * @param  *txdata:指向数据首地址;length:数据长度
  * @retval 
  */
u8 ft5x0x_i2c_txdata(u8 *txdata, u8 length)
{
	u8 ret =0;
  int i;
	iic_start();
	iic_write_byte(FT5x0x_Write_ADD,1);
  for(i=0;i<length;i++)
  { 
    iic_write_byte(txdata[i],1);		
  }                   
  
	iic_stop();
  delay_ms(5);
  return ret;
}

/**
  * @brief  从5306读data
  * @param  *rxdata:指向数据首地址;length:数据长度
  * @retval 
  */
u8 ft5x0x_i2c_rxdata(u8 *rxdata, u8 length)
{
	u8 ret;
  u8 *rxdatatmp =  rxdata;
	iic_start();            
  iic_write_byte(FT5x0x_Read_ADD, 1);
  for(ret=0;ret<length;ret++)
  {
		rxdatatmp[ret]=iic_read_byte(1);             
  }
	iic_stop();
  return ret;
}

/**
  * @brief  从5306读当前坐标
  * @param  None
  * @retval 
  */
u8 ft5x0x_read_xy(void)
{
	u8 buf[32] = {0};
  u8 ret = 0;
  u8 j=0;
#ifdef CONFIG_FT5X0X_MULTITOUCH
	ft5x0x_readCTP(buf, 31);
#else
  ft5x0x_readCTP(buf, 7);
#endif
  ts_event.touch_point = buf[2] & 0xf;
  if (ts_event.touch_point == 0) 
	{
		return 0;
  }

#ifdef CONFIG_FT5X0X_MULTITOUCH
	switch (ts_event.touch_point) 
	{
		case 5:
           ts_event.x5 = (s16)(buf[0x1b] & 0x0F)<<8 | (s16)buf[0x1c];
           ts_event.y5 = (s16)(buf[0x1d] & 0x0F)<<8 | (s16)buf[0x1e];
				//	 printf("point%d: (%d, %d)\r\n", ts_event.touch_point, ts_event.x5, ts_event.y5);
    case 4:
           ts_event.x4 = (s16)(buf[0x15] & 0x0F)<<8 | (s16)buf[0x16];
           ts_event.y4 = (s16)(buf[0x17] & 0x0F)<<8 | (s16)buf[0x18];
				//	 printf("point%d: (%d, %d)\r\n", ts_event.touch_point, ts_event.x4, ts_event.y4);
    case 3:
           ts_event.x3 = (s16)(buf[0x0f] & 0x0F)<<8 | (s16)buf[0x10];
           ts_event.y3 = (s16)(buf[0x11] & 0x0F)<<8 | (s16)buf[0x12];
					// printf("point%d: (%d, %d)\r\n", ts_event.touch_point, ts_event.x3, ts_event.y3);
    case 2:
           ts_event.x2 = (s16)(buf[9] & 0x0F)<<8 | (s16)buf[10];
           ts_event.y2 = (s16)(buf[11] & 0x0F)<<8 | (s16)buf[12];
					// printf("point%d: (%d, %d)\r\n", ts_event.touch_point, ts_event.x2, ts_event.y2);
    case 1:
		 
				 ts_event.x1 = (s16)(buf[3] & 0x0F)<<8 | (s16)buf[4];
          ts_event.y1 = (s16)(buf[5] & 0x0F)<<8 | (s16)buf[6];
		printf("point%d: (X:%d, Y:%d)\r\n", ts_event.touch_point, ts_event.x1, ts_event.y1);
							 
    break;
    default:
    return 0;
	}
	#else
  if (ts_event.touch_point == 1)
  {

		//读出的数据位480*272
    ts_event.x1 = (s16)(buf[3] & 0x0F)<<8 | (s16)buf[4];
    ts_event.y1 = (s16)(buf[5] & 0x0F)<<8 | (s16)buf[6];
    ret = 1;
  }
  else
  {
		ts_event.x1 = 0xFFFF;
    ts_event.y1 = 0xFFFF;
    ret = 0;
  }
#endif
//  if (ts_event.touch_point == 1) printf("%d (%d, %d)\r\n", ts_event.touch_point, ts_event.x1, ts_event.y1);
    
	return ret;
}



