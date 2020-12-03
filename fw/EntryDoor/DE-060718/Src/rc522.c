/**
 ******************************************************************************
 * File Name          : mfrc522.c
 * Date               : 28/02/2016 23:16:19
 * Description        : mifare RC522 software modul
 ******************************************************************************
 *
 *
 ******************************************************************************
 */


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "rc522.h"
#include "common.h"


/* Imported Type  ------------------------------------------------------------*/
extern UART_HandleTypeDef huart1;


/* Imported Variable  --------------------------------------------------------*/
/* Imported Function  --------------------------------------------------------*/
/* Private Define ------------------------------------------------------------*/
#define DEFAULT_SYSTEM_ID					0xABCDU
#define RC522_CARD_VALID_EVENT_TIME			3210U   // 3 s reader unlisten time after card read
#define RC522_CARD_INVALID_EVENT_TIME		987U    // ~1 s reader unlisten time after card read
#define RC522_POWER_ON_DELAY_TIME           8901U	// 8 s on power up reader is disbled 
#define RC522_PROCESS_TIME					123U	// 123 ms read rate
#define RC522_BUFF_LEN						16U     // buffer byte lenght
#define RC522_READ    						0x7FU	// RC522 i2c read address
#define RC522_WRITE   						0x7EU	// RC522 i2c write address
#define RC522_TIMEOUT						20U     // RC522 i2c 5 ms transfer time_mout limit
#define RC522_TRIALS						50U		// RC522 i2c 100 time_ms operation retry
#define RC522_BLOCK_SIZE              		16U 
#define CARD_PENDING						0U	    // card data status type
#define CARD_VALID							0x06U	// card data status type
#define CARD_INVALID						0x15U	// card data status type
#define CARD_DATA_FORMATED					0x7FU	// card data status type
/** ==========================================================================*/
/**			M I F A R E		C A R D		S E C T O R		A D D R E S S E		  */
/** ==========================================================================*/
#define SECTOR_0							0x00U
#define SECTOR_1							0x04U
#define SECTOR_2							0x08U
#define SECTOR_3							0x0CU
#define SECTOR_4							0x10U
#define SECTOR_5							0x14U
#define SECTOR_6							0x18U
#define SECTOR_7							0x1CU
#define SECTOR_8							0x20U
/** ==========================================================================*/
/**			R C 5 2 2			C O M M A N D			L I S T				  */
/** ==========================================================================*/
#define PCD_IDLE							0x00U   //NO action; Cancel the current command
#define PCD_AUTHENT							0x0EU   //Authentication Key
#define PCD_RECEIVE							0x08U   //Receive Data
#define PCD_TRANSMIT						0x04U   //Transmit data
#define PCD_TRANSCEIVE						0x0CU   //Transmit and receive data,
#define PCD_RESETPHASE						0x0FU   //Reset
#define PCD_CALCCRC							0x03U   //CRC Calculate
/* Mifare_One card command word */
#define PICC_REQIDL							0x26U   // find the antenna area does not enter hibernation
#define PICC_REQALL							0x52U   // find all the cards antenna area
#define PICC_ANTICOLL						0x93U   // anti-collision
#define PICC_SELECTTAG						0x93U   // election card
#define PICC_AUTHENT1A						0x60U   // authentication key A
#define PICC_AUTHENT1B						0x61U   // authentication key B
#define PICC_READ							0x30U   // Read Block
#define PICC_WRITE							0xA0U   // write block
#define PICC_DECREMENT						0xC0U   // debit
#define PICC_INCREMENT						0xC1U   // recharge
#define PICC_RESTORE						0xC2U   // transfer block data to the buffer
#define PICC_TRANSFER						0xB0U   // save the data in the buffer
#define PICC_HALT							0x50U   // Sleep
/** ==========================================================================*/
/**			R C 5 2 2			R E G I S T E R			L I S T				  */
/** ==========================================================================*/
//Page 0: Command and Status
#define RC522_REG_RESERVED00				0x00U    
#define RC522_REG_COMMAND					0x01U    
#define RC522_REG_COMM_IE_N					0x02U    
#define RC522_REG_DIV1_EN					0x03U    
#define RC522_REG_COMM_IRQ					0x04U    
#define RC522_REG_DIV_IRQ					0x05U
#define RC522_REG_ERROR						0x06U    
#define RC522_REG_STATUS1					0x07U    
#define RC522_REG_STATUS2					0x08U    
#define RC522_REG_FIFO_DATA					0x09U
#define RC522_REG_FIFO_LEVEL				0x0AU
#define RC522_REG_WATER_LEVEL				0x0BU
#define RC522_REG_CONTROL					0x0CU
#define RC522_REG_BIT_FRAMING				0x0DU
#define RC522_REG_COLL						0x0EU
#define RC522_REG_RESERVED01				0x0FU
//Page 1: Command 
#define RC522_REG_RESERVED10				0x10U
#define RC522_REG_MODE						0x11U
#define RC522_REG_TX_MODE					0x12U
#define RC522_REG_RX_MODE					0x13U
#define RC522_REG_TX_CONTROL				0x14U
#define RC522_REG_TX_AUTO					0x15U
#define RC522_REG_TX_SELL					0x16U
#define RC522_REG_RX_SELL					0x17U
#define RC522_REG_RX_THRESHOLD				0x18U
#define RC522_REG_DEMOD						0x19U
#define RC522_REG_RESERVED11				0x1AU
#define RC522_REG_RESERVED12				0x1BU
#define RC522_REG_MIFARE					0x1CU
#define RC522_REG_RESERVED13				0x1DU
#define RC522_REG_RESERVED14				0x1EU
#define RC522_REG_SERIALSPEED				0x1FU
//Page 2: CFG    
#define RC522_REG_RESERVED20				0x20U  
#define RC522_REG_CRC_RESULT_M				0x21U
#define RC522_REG_CRC_RESULT_L				0x22U
#define RC522_REG_RESERVED21				0x23U
#define RC522_REG_MOD_WIDTH					0x24U
#define RC522_REG_RESERVED22				0x25U
#define RC522_REG_RF_CFG					0x26U
#define RC522_REG_GS_N						0x27U
#define RC522_REG_CWGS_PREG					0x28U
#define RC522_REG_MODGS_PREG				0x29U
#define RC522_REG_T_MODE					0x2AU
#define RC522_REG_T_PRESCALER				0x2BU
#define RC522_REG_T_RELOAD_H				0x2CU
#define RC522_REG_T_RELOAD_L				0x2DU
#define RC522_REG_T_COUNTER_VALUE_H			0x2EU
#define RC522_REG_T_COUNTER_VALUE_L			0x2FU
//Page 3:TestRegister 
#define RC522_REG_RESERVED30				0x30U
#define RC522_REG_TEST_SEL1					0x31U
#define RC522_REG_TEST_SEL2					0x32U
#define RC522_REG_TEST_PIN_EN				0x33U
#define RC522_REG_TEST_PIN_VALUE			0x34U
#define RC522_REG_TEST_BUS					0x35U
#define RC522_REG_AUTO_TEST					0x36U
#define RC522_REG_VERSION					0x37U
#define RC522_REG_ANALOG_TEST				0x38U
#define RC522_REG_TEST_ADC1					0x39U 
#define RC522_REG_TEST_ADC2					0x3AU  
#define RC522_REG_TEST_ADC0					0x3BU  
#define RC522_REG_RESERVED31				0x3CU  
#define RC522_REG_RESERVED32				0x3DU
#define RC522_REG_RESERVED33				0x3EU  
#define RC522_REG_RESERVED34				0x3FU


/* Private Type --------------------------------------------------------------*/
typedef enum 
{
	MI_OK 			= 0x00U,
    MI_ERR			= 0x01U,
	MI_NOTAGERR		= 0x02U,
	MI_SKIP_OVER	= 0x03U

} RC522_StatusTypeDef;


typedef struct
{
    uint8_t block_0[RC522_BLOCK_SIZE];
    uint8_t block_1[RC522_BLOCK_SIZE];
    uint8_t block_2[RC522_BLOCK_SIZE];
    uint8_t block_3[RC522_BLOCK_SIZE];
	
} RC522_SectorTypeDef;


RC522_SectorTypeDef sector_0;
RC522_SectorTypeDef sector_1;
RC522_SectorTypeDef sector_2;
RC522_CardDataTypeDef sCard;


/* Private Variable ----------------------------------------------------------*/
__IO uint32_t mifare_process_flags;

uint8_t card_id[5];

uint8_t mifare_key_a[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 
uint8_t mifare_key_b[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 
uint8_t rc522_rx_buff[RC522_BUFF_LEN];
uint8_t rc522_tx_buff[RC522_BUFF_LEN];


/* Private Macro -------------------------------------------------------------*/
/* Private Function Prototype ------------------------------------------------*/
static RC522_StatusTypeDef RC522_Check(uint8_t* id);
static void RC522_WriteRegister(uint8_t addr, uint8_t val);
static uint8_t RC522_ReadRegister(uint8_t addr);
static void RC522_SetBitMask(uint8_t reg, uint8_t mask);
static void RC522_ClearBitMask(uint8_t reg, uint8_t mask);
static void RC522_AntennaOn(void);
static RC522_StatusTypeDef RC522_Reset(void);
static RC522_StatusTypeDef RC522_Request(uint8_t reqMode, uint8_t* TagType);
static RC522_StatusTypeDef RC522_ToCard(uint8_t command, uint8_t* sendData, uint8_t sendLen, uint8_t* backData, uint16_t* backLen);
static RC522_StatusTypeDef RC522_Anticoll(uint8_t* serNum);
static void RC522_CalculateCRC(uint8_t* pIndata, uint8_t len, uint8_t* pOutData);
static uint8_t RC522_SelectTag(uint8_t* serNum);
static RC522_StatusTypeDef RC522_Auth(uint8_t authMode, uint8_t BlockAddr, uint8_t* Sectorkey, uint8_t* serNum);
static RC522_StatusTypeDef RC522_Read(uint8_t blockAddr, uint8_t* recvData);
static void RC522_Halt(void);
static void RC522_ClearCard(void);
static RC522_StatusTypeDef RC522_ReadCard(void);
static RC522_StatusTypeDef RC522_VerifyData(void);



/* Program Code  -------------------------------------------------------------*/
void RC522_Init(void) 
{
	HAL_Delay(250);
    
    RC522_WriteRegister(RC522_REG_SERIALSPEED, 0x7A);                   //  set MFRC522 chip uart baudrate to 115200 bps

    huart1.Init.BaudRate = 115200;                                      //  switch STM32F030 usart to 115200 bps 
    
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    
    HAL_Delay(250);
	RC522_WriteRegister(RC522_REG_COMMAND, PCD_RESETPHASE);	
	RC522_WriteRegister(RC522_REG_T_MODE, 0x80U);
	RC522_WriteRegister(RC522_REG_T_PRESCALER, 0xA9);
	RC522_WriteRegister(RC522_REG_T_RELOAD_H, 0x03);
	RC522_WriteRegister(RC522_REG_T_RELOAD_L, 0xE8);		
	RC522_WriteRegister(RC522_REG_TX_AUTO, 0x40U);
	RC522_WriteRegister(RC522_REG_MODE, 0x3DU);
    RC522_WriteRegister(RC522_REG_TEST_PIN_EN, 0x00U);
	RC522_AntennaOn();
}


void RC522_Service(void)
{
    uint8_t card_serial[5];
	static uint32_t mifare_timer = 0U;                                  // start timer value
    RC522_StatusTypeDef status = MI_OK;

    if((HAL_GetTick() - mifare_timer) < RC522_PROCESS_TIME) return;     // check is timer expired
    mifare_timer = HAL_GetTick();                                       // reload start timer
    
	if (RC522_Check(card_serial) == MI_OK)                              // check is MIFARE card in field range
	{
		RC522_ClearCard();                                              // clear previous card data
		status += RC522_ReadCard();                                     // read card sectors
		status += RC522_VerifyData();                                   // verify carddat
        
        if(status == MI_OK)                                             // signal valid card
        {
            HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
            HAL_Delay(100);
            HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
            HAL_Delay(500);
            HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
            HAL_Delay(500);
        }
        else                                                            // signal invalid card
        {
            HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
            HAL_Delay(50);
            HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
            HAL_Delay(50);
            HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
            HAL_Delay(50);
            HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
            HAL_Delay(50);
            HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
            HAL_Delay(50);
            HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
            HAL_Delay(500);
            HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
            HAL_Delay(500);
        }
	}
    
    RC522_Reset();                                                      //  after reset MFRC522 uart is 9600 bps default
    huart1.Init.BaudRate = 9600;                                        //  switch STM32F030 usart to 9600 bps 

    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    RC522_WriteRegister(RC522_REG_SERIALSPEED, 0x7A);                   //  set MFRC522 chip uart baudrate to 115200 bps

    huart1.Init.BaudRate = 115200;                                      //  switch STM32F030 usart to 115200 bps 

    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    RC522_WriteRegister(RC522_REG_T_MODE, 0x80U);
    RC522_WriteRegister(RC522_REG_T_PRESCALER, 0xA9);
    RC522_WriteRegister(RC522_REG_T_RELOAD_H, 0x03);
    RC522_WriteRegister(RC522_REG_T_RELOAD_L, 0xE8);		
    RC522_WriteRegister(RC522_REG_TX_AUTO, 0x40U);
    RC522_WriteRegister(RC522_REG_MODE, 0x3DU);
    RC522_WriteRegister(RC522_REG_TEST_PIN_EN, 0x00U);
    RC522_AntennaOn();
}


static void RC522_ClearCard(void)
{
	uint32_t i;
		
    sCard.card_status = 0U;
    sCard.user_group = 0U;
    sCard.system_id = 0U;
    sCard.controller_id = 0U;

	ClearBuffer(sCard.card_id, 5U);
	ClearBuffer(sCard.expiry_time, 6U);
    
	for(i = 0U; i < 16U; i++)
	{
		sector_0.block_0[i] = 0U;
		sector_0.block_1[i] = 0U;
		sector_0.block_2[i] = 0U;
		sector_0.block_3[i] = 0U;
		
		sector_1.block_0[i] = 0U;
		sector_1.block_1[i] = 0U;
		sector_1.block_2[i] = 0U;
		sector_1.block_3[i] = 0U;
		
		sector_2.block_0[i] = 0U;
		sector_2.block_1[i] = 0U;
		sector_2.block_2[i] = 0U;
		sector_2.block_3[i] = 0U;
	}
}


static RC522_StatusTypeDef RC522_Check(uint8_t* id) 
{
	RC522_StatusTypeDef status;
	
	status = RC522_Request(PICC_REQIDL, id);            // Find cards, return card type
    
	if (status == MI_OK) 
    {                                                   // Card detected
		status = RC522_Anticoll(id);                    // Anti-collision, return card serial number 4 bytes
	}
    
	RC522_Halt();                                       // Command card into hibernation 

	return (status);
}


static void RC522_WriteRegister(uint8_t addr, uint8_t val) 
{
    uint8_t tx[2];
    
	tx[0] = addr;
    tx[1] = val;     // set value
	if(HAL_UART_Transmit(&huart1, tx, 2U, RC522_TIMEOUT) != HAL_OK) Error_Handler();
}


static uint8_t RC522_ReadRegister(uint8_t addr) 
{
    uint8_t tx = 0, rx = 0;
    
	tx = addr | 0x80;
    HAL_UART_DeInit(&huart1);
    HAL_UART_Init(&huart1);
	if(HAL_UART_Transmit(&huart1, &tx, 1U, 5) != HAL_OK)  Error_Handler();
	if(HAL_UART_Receive(&huart1, &rx, 1U, 10) != HAL_OK)  Error_Handler();
    return (rx);
}


static void RC522_SetBitMask(uint8_t reg, uint8_t mask) 
{   
	RC522_WriteRegister(reg, RC522_ReadRegister(reg) | mask);
}


static void RC522_ClearBitMask(uint8_t reg, uint8_t mask)
{  
	RC522_WriteRegister(reg, RC522_ReadRegister(reg) & (~mask));
}


static void RC522_AntennaOn(void) 
{ 
	uint8_t temp;

	temp = RC522_ReadRegister(RC522_REG_TX_CONTROL);
    
	if (!(temp & 0x03U))        
    {
		RC522_SetBitMask(RC522_REG_TX_CONTROL, 0x03U);
	}
}


static RC522_StatusTypeDef RC522_Reset(void) 
{  
    uint32_t delay;
    /**
    *   Issue the SoftReset command.
    */
	RC522_WriteRegister(RC522_REG_COMMAND, PCD_RESETPHASE);
    /**
    *   The datasheet does not mention how long the SoftRest command takes to complete.
    *   But the RC522 might have been in soft power-down mode (triggered by bit 4 of CommandReg)
    *   Section 8.8.2 in the datasheet says the oscillator start-up time_m is the start up time_m of the crystal + 37,74us. Let us be generous: 50ms.
    */
    HAL_Delay(5);

	delay = 500U;
    /**
    *   Wait for the PowerDown bit in CommandReg to be cleared
    */	
    while (RC522_ReadRegister(RC522_REG_COMMAND) & (1U << 4))
    {
		
        /**
        *   RC522 still restarting - unlikely after waiting 50ms and more
        *   mifare modul is unresponsive so return error status
        */
        --delay;
        
        if(delay == 0U)
		{			
            return (MI_ERR);
        }
		else
		{
			HAL_Delay(1);
		}
    }
    /**
    *   reset finished - return OK flag
    */
    return (MI_OK);
}


static RC522_StatusTypeDef RC522_Request(uint8_t reqMode, uint8_t* TagType) 
{
	RC522_StatusTypeDef status;  
	uint16_t backBits;                                  //The received data bits

	RC522_WriteRegister(RC522_REG_BIT_FRAMING, 0x07U);	// TxLastBits = BitFramingReg[2..0]	???

	TagType[0] = reqMode;
	status = RC522_ToCard(PCD_TRANSCEIVE, TagType, 1U, TagType, &backBits);

	if ((status != MI_OK) || (backBits != (1U << 4))) 
    {
		status = MI_ERR;
	}

	return (status);
}


static RC522_StatusTypeDef RC522_ToCard(uint8_t command, uint8_t* sendData, uint8_t sendLen, uint8_t* backData, uint16_t* backLen) 
{
    RC522_StatusTypeDef status = MI_ERR;
	uint8_t irqEn = 0U;
	uint8_t waitIRq = 0U;
	uint32_t n, i, lastBits; 

	switch (command) 
	{
		case PCD_AUTHENT:
			irqEn = 0x12U;
			waitIRq = 0x10U;            
			break;
        
		case PCD_TRANSCEIVE:            
			irqEn = 0x77U;
			waitIRq = 0x30U;            
			break;
        
		default:
			break;
	}
	
    RC522_WriteRegister(RC522_REG_COMMAND, PCD_IDLE);
    RC522_ClearBitMask(RC522_REG_COMM_IRQ, (1U << 7));
    RC522_SetBitMask(RC522_REG_FIFO_LEVEL, (1U << 7));
	RC522_WriteRegister(RC522_REG_COMM_IE_N, (irqEn | (1U << 7)));
	
	for (i = 0U; i < sendLen; i++) 
    {   
		RC522_WriteRegister(RC522_REG_FIFO_DATA, sendData[i]);   //Writing data to the FIFO
	}

	RC522_WriteRegister(RC522_REG_COMMAND, command);            //Execute the command
    
	if (command == PCD_TRANSCEIVE) 
    {    
		RC522_SetBitMask(RC522_REG_BIT_FRAMING, (1U << 7));     //StartSend=1,transmission of data starts  
    }
    /**
    *   Waiting to receive data to complete
    */
	i = 2000U;	//i according to the clock frequency adjustment, the operator M1 card maximum waiting time_m 25ms???
    
	do {
        /**
        *   CommIrqReg[7..0]
        *   Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
        */
		n = RC522_ReadRegister(RC522_REG_COMM_IRQ);
		--i;
	} 
    while ((i != 0U) && !(n & (1U << 0)) && !(n & waitIRq));          // End of do...while loop            
    /**
    *   StartSend=0
    */
	RC522_ClearBitMask(RC522_REG_BIT_FRAMING, (1U << 7));

	if (i != 0U)  
    {
		if (!(RC522_ReadRegister(RC522_REG_ERROR) & 0x13U)) 
        {
			status = MI_OK;
            
			if (n & irqEn & (1U << 0)) 
            {
				status = MI_NOTAGERR;
			}

			if (command == PCD_TRANSCEIVE) 
            {
				n = RC522_ReadRegister(RC522_REG_FIFO_LEVEL);
				lastBits = (RC522_ReadRegister(RC522_REG_CONTROL) & 0x07U);
                
				if (lastBits != 0U) *backLen = ((n - 1U) * 8U + lastBits);  
                else *backLen = (n * 8U);  

				if (n == 0U) n = 1U;
                
				if (n > RC522_BUFF_LEN) n = RC522_BUFF_LEN;   
				/**
                *   Reading the received data in FIFO
                */
				for (i = 0U; i < n; i++) 
                {
					backData[i] = RC522_ReadRegister(RC522_REG_FIFO_DATA);
				}
			}
		} 
        else 
        {
			status = MI_ERR;
		}
	}

	return (status);
}


static RC522_StatusTypeDef RC522_Anticoll(uint8_t* serNum) 
{
	RC522_StatusTypeDef status;
	uint32_t i;
	uint8_t serNumCheck = 0U;
	uint16_t unLen;

	RC522_WriteRegister(RC522_REG_BIT_FRAMING, 0U);   // TxLastBists = BitFramingReg[2..0]
	serNum[0] = PICC_ANTICOLL;
	serNum[1] = 0x20U;
	status = RC522_ToCard(PCD_TRANSCEIVE, serNum, 2U, serNum, &unLen);

	if (status == MI_OK) 
    {
		/**
        *   Check card serial number
        */
		for (i = 0U; i < 4U; i++) 
        {
			serNumCheck ^= serNum[i];
		}
        
		if (serNumCheck != serNum[i])
        {
			status = MI_ERR;
		}
	}
    
	return (status);
}


static void RC522_CalculateCRC(uint8_t*  pIndata, uint8_t len, uint8_t* pOutData)
{ 
	uint32_t i, n;

	RC522_WriteRegister(RC522_REG_COMMAND, PCD_IDLE);
	RC522_ClearBitMask(RC522_REG_DIV_IRQ, 0x04U);       // CRCIrq = 0
	RC522_SetBitMask(RC522_REG_FIFO_LEVEL, 0x80U);      // Clear the FIFO pointer
	/**
    *   Write_RC522(CommandReg, PCD_IDLE);
    *   Writing data to the FIFO
    */
	for (i = 0U; i < len; i++) 
    {
		RC522_WriteRegister(RC522_REG_FIFO_DATA, *(pIndata + i)); 
	}
    /**
	*   Start CRC calculation
    */
	RC522_WriteRegister(RC522_REG_COMMAND, PCD_CALCCRC);
    /**
	*   Wait for CRC calculation to complete
    */
	i = 0xFFU;
    
	do 
    {
        n = RC522_ReadRegister(RC522_REG_DIV_IRQ);
		--i;
	} 
    while ((i != 0U) && !(n & (1U << 2)));              // wait for CRCIrq = 1
	
	RC522_WriteRegister(RC522_REG_COMMAND, PCD_IDLE);
	/**
    *   Read CRC calculation result
    */
	pOutData[0] = RC522_ReadRegister(RC522_REG_CRC_RESULT_L);
	pOutData[1] = RC522_ReadRegister(RC522_REG_CRC_RESULT_M);
}


static uint8_t RC522_SelectTag(uint8_t* serNum) 
{
    RC522_StatusTypeDef status;
	uint32_t i;
	uint16_t recvBits;
	uint8_t buffer[9], size; 

	buffer[0] = PICC_SELECTTAG;
	buffer[1] = 0x70U;
    
	for (i = 0U; i < 5U; i++) 
    {
		buffer[i + 2U] = *(serNum + i);
	}
    
	RC522_CalculateCRC(buffer, 7U, &buffer[7]);		//??
	status = RC522_ToCard(PCD_TRANSCEIVE, buffer, 9U, buffer, &recvBits);

	if ((status == MI_OK) && (recvBits == 0x18U)) 
    {
		size = buffer[0]; 
	} 
    else 
    {
		size = 0U;
	}

	return (size);
}


static RC522_StatusTypeDef RC522_Auth(uint8_t authMode, uint8_t BlockAddr, uint8_t* Sectorkey, uint8_t* serNum) 
{
    RC522_StatusTypeDef status;
	uint16_t recvBits;
	uint8_t i;
	uint8_t buff[12]; 

	//Verify the command block address + sector + password + card serial number
	buff[0] = authMode;
	buff[1] = BlockAddr;
    
	for (i = 0U; i < 6U; i++) 
    { 
		buff[i + 2U] = *(Sectorkey + i); 
	}
    
	for (i = 0U; i < 4U; i++) 
    {
		buff[i + 8U] = *(serNum + i);
	}
    
	status = RC522_ToCard(PCD_AUTHENT, buff, 12U, buff, &recvBits);

	if ((status != MI_OK) || (!(RC522_ReadRegister(RC522_REG_STATUS2) & (1U << 3)))) 
    {
		status = MI_ERR;
	}
    
	return (status);
}


static RC522_StatusTypeDef RC522_Read(uint8_t blockAddr, uint8_t* recvData) 
{
	RC522_StatusTypeDef status;
	uint16_t unLen;

	recvData[0] = PICC_READ;
	recvData[1] = blockAddr;
	RC522_CalculateCRC(recvData, 2U, &recvData[2]);
	status = RC522_ToCard(PCD_TRANSCEIVE, recvData, 4U, recvData, &unLen);

	if ((status != MI_OK) || (unLen != 0x90U)) 
    {
		status = MI_ERR;
	}

	return (status);
}


static void RC522_Halt(void) 
{
	uint16_t unLen;
	uint8_t buff[4]; 

	buff[0] = PICC_HALT;
	buff[1] = 0U;
	RC522_CalculateCRC(buff, 2U, &buff[2]);
	RC522_ToCard(PCD_TRANSCEIVE, buff, 4U, buff, &unLen);
}


static RC522_StatusTypeDef RC522_ReadCard(void)
{
    uint32_t i;
    uint8_t str[RC522_BUFF_LEN];
    RC522_StatusTypeDef status;
	
    RC522_Request(PICC_REQIDL, str);	
    status = RC522_Anticoll(str);

    for(i = 0U; i < 5U; i++)
    {
        card_id[i] = str[i];
    }

    RC522_SelectTag(card_id);	
	
    status = RC522_Auth(PICC_AUTHENT1A, SECTOR_0, mifare_key_a, card_id);
	
    if(status != MI_OK)
	{
		RC522_Halt();
		return (status);
	}
	else
	{    
        RC522_Read(SECTOR_0, &sector_0.block_0[0]);
        RC522_Read((SECTOR_0 + 1U), &sector_0.block_1[0]);
        RC522_Read((SECTOR_0 + 2U), &sector_0.block_2[0]);
        RC522_Read((SECTOR_0 + 3U), &sector_0.block_3[0]);
	}
	
	status = RC522_Auth(PICC_AUTHENT1A, SECTOR_1, mifare_key_a, card_id);
	
    if(status != MI_OK)
	{	
		RC522_Halt();
		return (status);
	}
	else
	{      
        RC522_Read(SECTOR_1, &sector_1.block_0[0]);
        RC522_Read((SECTOR_1 + 1U), &sector_1.block_1[0]);
        RC522_Read((SECTOR_1 + 2U), &sector_1.block_2[0]);
        RC522_Read((SECTOR_1 + 3U), &sector_1.block_3[0]);    
	}
	
	status = RC522_Auth(PICC_AUTHENT1A, SECTOR_2, mifare_key_a, card_id);
	
    if(status != MI_OK)
	{		
		RC522_Halt();
		return (status);
	}
	else
	{      
        RC522_Read(SECTOR_2, &sector_2.block_0[0]);
        RC522_Read((SECTOR_2 + 1U), &sector_2.block_1[0]);
        RC522_Read((SECTOR_2 + 2U), &sector_2.block_2[0]);
        RC522_Read((SECTOR_2 + 3U), &sector_2.block_3[0]); 	
	}

    RC522_Halt();
	return (status);
}


static RC522_StatusTypeDef RC522_VerifyData(void)
{

    /**
	*			C A R D     D A T A     C H E C K
	**/	

    
	return (MI_OK);
}


