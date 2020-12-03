/**
 ******************************************************************************
 * File Name          : dio_interface.c
 * Date               : 28/02/2016 23:16:19
 * Description        :  digital in/out and capacitive sensor  modul
 ******************************************************************************
 */
 
 
/* Include  ------------------------------------------------------------------*/
#include "main.h"
#include "eeprom.h"
#include "common.h"
#include "dio.h"
#include "rc522.h"
#include "one_wire.h"
#include "rs485.h"
#include "display.h"
#include "signal.h"


/* Imported Types  -----------------------------------------------------------*/
/* Imported Variables --------------------------------------------------------*/
/* Imported Functions    -----------------------------------------------------*/
/* Private Variables  --------------------------------------------------------*/
__IO uint32_t dio_flags;
__IO uint32_t din_0_7;
__IO uint32_t din_state;
__IO uint32_t din_cap_sen;
__IO uint32_t dout_0_7;
__IO uint32_t dout_0_7_remote;
uint8_t buzzer_volume;
uint8_t doorlock_force;
uint8_t hc595_dout;


/* Private Define  -----------------------------------------------------------*/
#define INPUT_DEBOUNCE_CNT  						100U		// number of input state check 100
#define FAST_INPUT_DEBOUNCE_CNT  					50U   	    // number of input state check 50
#define SLOW_INPUT_DEBOUNCE_CNT  					10000U      // number of input state check 50
#define WATER_FLOOD_MAX_CYCLES						60U		    // check water flood sensor 60 time, then write log
#define WATER_FLOOD_CHECK_CYCLE_TIME                1111U	    // ~1000ms cycle period
#define DIO_PROCESS_TIME							5432U	    // 5s temperature regulator cycle time
#define DOOR_BELL_SW_ACTIV_TIME						2000U	    // 2 s external door bell power time
#define DOOR_BELL_SW_MENU_BUTTON_TIME				123U		// when used as service display_menu button
#define HANDMAID_SW_ACTIV_TIME						1234U       // 1 s handmaid capacitive switch activ time
#define HANDMAID_SW_MENU_BUTTON_TIME				123U		// handmaid capacitive switch activ time
#define DOOR_LOCK_COIL_PULSE_CYCLES					10U		    // 10 times on - off cycles for door lock coil
#define DOOR_LOCK_COIL_PULSE_DURATION				250U		// 250 ms door lock coil duty period
#define DOOR_LOCK_MAX_CYCLE_TIME					5112U	    // 5 sec. max door lock on time
#define DOOR_BELL_LED_ACTIV_TOGGLE_TIME             200U
#define HANDMAID_LED_ACTIV_TOGGLE_TIME              200U
#define CAP1293_PRODUCT_ID							((uint8_t)0x6fU)
#define CAP1293_VENDOR_ID							((uint8_t)0x5dU)
#define CAP1293_WRITE								((uint8_t)0x50U)
#define CAP1293_READ								((uint8_t)0x51U)
#define CAP1293_TIMEOUT								10U
#define CAP1293_TRIALS								100U
/***********************************************************************
**	 	C A P 1 2 9 3 	  	 R E G I S T E R		A D D E S S E 
***********************************************************************/
#define CAP1293_MAIN_CONTROL_REG					((uint8_t)0x00U)
#define CAP1293_GENERAL_STATUS_REG					((uint8_t)0x02U)
#define CAP1293_SENSOR_INPUT_STATUS_REG				((uint8_t)0x03U)
#define CAP1293_NOISE_FLAG_STATUS_REG				((uint8_t)0x0aU)
#define CAP1293_SENSOR_1_INPUT_DELTA_COUNT_REG		((uint8_t)0x10U)
#define CAP1293_SENSOR_2_INPUT_DELTA_COUNT_REG		((uint8_t)0x11U)
#define CAP1293_SENSOR_3_INPUT_DELTA_COUNT_REG		((uint8_t)0x12U)
#define CAP1293_SENSITIVITY_CONTROL_REG				((uint8_t)0x1fU)
#define CAP1293_CONFIGURATION_REG					((uint8_t)0x20U)
#define CAP1293_SENSOR_INPUT_ENABLE_REG				((uint8_t)0x21U)
#define CAP1293_SENSOR_INPUT_CONFIGURATION_REG		((uint8_t)0x22U)
#define CAP1293_SENSOR_INPUT_CONFIGURATION_2_REG	((uint8_t)0x23U)
#define CAP1293_AVERAGING_AND_SAMPLING_CONFIG_REG	((uint8_t)0x24U)
#define CAP1293_CALIBRATION_ACTIVATE_AND_STATUS_REG	((uint8_t)0x26U)
#define CAP1293_INTERRUPT_ENABLE_REG				((uint8_t)0x27U)
#define CAP1293_REPEAT_RATE_ENABLE_REG				((uint8_t)0x28U)
#define CAP1293_SINGLE_GUARD_ENABLE_REG				((uint8_t)0x29U)
#define CAP1293_MULTIPLE_TOUCH_CONFIGURATION_REG	((uint8_t)0x2aU)
#define CAP1293_MULTIPLE_TOUCH_PATTERN_CONFIG_REG	((uint8_t)0x2bU)
#define CAP1293_MULTIPLE_TOUCH_PATTERN_REG			((uint8_t)0x2dU)
#define CAP1293_BASE_COUNT_OF_LIMIT_REG				((uint8_t)0x2eU)
#define CAP1293_RECALIBRATION_CONFIGURATION_REG		((uint8_t)0x2fU)
#define CAP1293_SENSOR_INPUT_1_TRESHOLD_REG			((uint8_t)0x30U)
#define CAP1293_SENSOR_INPUT_2_TRESHOLD_REG			((uint8_t)0x31U)
#define CAP1293_SENSOR_INPUT_3_TRESHOLD_REG			((uint8_t)0x32U)
#define CAP1293_SENSOR_INPUT_NOISE_TRESHOLD_REG		((uint8_t)0x38U)
#define CAP1293_STANDBY_CHANNEL_REG					((uint8_t)0x40U)
#define CAP1293_STANDBY_CONFIGURATION_REG			((uint8_t)0x41U)
#define CAP1293_STANDBY_SENSITIVITY_REG				((uint8_t)0x42U)
#define CAP1293_STANDBY_TRESHOLD_REG				((uint8_t)0x43U)
#define CAP1293_CONFIGURATION_2_REG					((uint8_t)0x44U)
#define CAP1293_SENSOR_INPUT_1_BASE_COUNT_REG		((uint8_t)0x50U)
#define CAP1293_SENSOR_INPUT_2_BASE_COUNT_REG		((uint8_t)0x51U)
#define CAP1293_SENSOR_INPUT_3_BASE_COUNT_REG		((uint8_t)0x52U)
#define CAP1293_POWER_BUTTON_REG					((uint8_t)0x60U)
#define CAP1293_POWER_BUTTON_CONFIGURATION_REG		((uint8_t)0x61U)
#define CAP1293_CALIBRATION_SENSITIVITY_CONFIG_REG	((uint8_t)0x80U)
#define CAP1293_SENSOR_INPUT_1_CALIBRATION_REG		((uint8_t)0xb1U)
#define CAP1293_SENSOR_INPUT_2_CALIBRATION_REG		((uint8_t)0xb2U)
#define CAP1293_SENSOR_INPUT_3_CALIBRATION_REG		((uint8_t)0xb3U)
#define CAP1293_SENSOR_INPUT_CALIBRATION_LSB_REG	((uint8_t)0xb9U)
#define CAP1293_PRODUCT_ID_REG						((uint8_t)0xfdU)
#define CAP1293_MANUFACTURER_ID_REG					((uint8_t)0xfeU)
#define CAP1293_REVISION_REG						((uint8_t)0xffU)


/* Private Macro   -----------------------------------------------------------*/
/* Private Function Prototype  -----------------------------------------------*/
static void DIO_DebounceDigitalInput(void);
static void CAP1293_Init(void);
static uint8_t CAP1293_ReadRegister(uint8_t register_address);
static void CAP1293_WriteRegister(uint8_t register_address, uint8_t register_data);


/* Program Code  -------------------------------------------------------------*/
void DIO_Init(void)
{
	EntranceDoorSensorDisabled();
	LED_DoorBellBlue_On();
	DoNotDisturbModul_On();
	PowerContactor_On();
	DIO_SetOuput();
	CAP1293_Init();
}


void DIO_Service(void) 
{
	uint8_t rd_reg;
    static uint8_t dout_0_7_mem = 0U;
    static uint32_t doorlock_timer = 0U;
    
	if(eComState == COM_PACKET_RECEIVED) RS485_Service();	
	if(IsRS485_UpdateActiv()) return;
	/** ============================================================================*/
	/**		F O R C E   D I G I T A L   O U T P U T    F R O M  	C O M M A N D 	*/
	/** ============================================================================*/
	if(dout_0_7_remote & 0x100U)
	{
		if(!(dout_0_7_remote & 0x200U))
		{
			dout_0_7_mem = dout_0_7;
			dout_0_7_remote |= 0x200U;
		}
		
		dout_0_7 = (dout_0_7_remote & 0xffU);
		
		if((dout_0_7_remote & 0x40U) && !(dout_0_7_remote & 0x1000U))
		{
			dout_0_7_remote |= 0x1000U;
            doorlock_timer = HAL_GetTick();
		}
		else if(!(dout_0_7_remote & 0x40U))
		{
			dout_0_7_remote &= 0xefffU;
		}
		else if((dout_0_7_remote & 0x40U) && (dout_0_7_remote & 0x1000U))
		{
			if((HAL_GetTick() - doorlock_timer) >= DOOR_LOCK_MAX_CYCLE_TIME) dout_0_7 &= 0xbfU;
		}
	}
	else if(dout_0_7_remote & 0x200U)
	{
		dout_0_7 = dout_0_7_mem;
		dout_0_7_remote = 0U;
	}	
	/** ============================================================================*/                                                                         
	/**		R E L O A D 	C A P A C I T I V E		S E N S O R		S T A T E    	*/                                                                           	
	/** ============================================================================*/
	if(IsCAP1293_Present())
	{
		rd_reg = CAP1293_ReadRegister(CAP1293_SENSOR_INPUT_STATUS_REG);		
		din_cap_sen &= 0xf8U;
		din_cap_sen |= (rd_reg & 0x07U);		
		rd_reg = CAP1293_ReadRegister(CAP1293_MAIN_CONTROL_REG);
		if(rd_reg & (1U << 0)) CAP1293_WriteRegister(CAP1293_MAIN_CONTROL_REG, 0U);
	}
	/** ============================================================================*/                                                                         
	/**		R E L O A D 	D I G I T A L		I N P U T 		R E G I S T E R    	*/                                                                           	
	/** ============================================================================*/
    if(!IsDIN_ForcingActivated())
    {
        din_state = 0U;	
        if(HAL_GPIO_ReadPin(DIN_0_Port, DIN_0_Pin) == GPIO_PIN_SET) (din_state |= (1U << 0));
        if(HAL_GPIO_ReadPin(DIN_1_Port, DIN_1_Pin) == GPIO_PIN_SET) (din_state |= (1U << 1));
        if(HAL_GPIO_ReadPin(DIN_2_Port, DIN_2_Pin) == GPIO_PIN_SET) (din_state |= (1U << 2));
        if(HAL_GPIO_ReadPin(DIN_3_Port, DIN_3_Pin) == GPIO_PIN_SET) (din_state |= (1U << 3));
        if(HAL_GPIO_ReadPin(DIN_4_Port, DIN_4_Pin) == GPIO_PIN_SET) (din_state |= (1U << 4));
        if(HAL_GPIO_ReadPin(DIN_5_Port, DIN_5_Pin) == GPIO_PIN_SET) (din_state |= (1U << 5));
        if(HAL_GPIO_ReadPin(DIN_6_Port, DIN_6_Pin) == GPIO_PIN_SET) (din_state |= (1U << 6));
        if(HAL_GPIO_ReadPin(DIN_7_Port, DIN_7_Pin) == GPIO_PIN_SET) (din_state |= (1U << 7));		        
    }
	DIO_DebounceDigitalInput();	
	/** ============================================================================*/                                                                         
	/**			S E T 		D I G I T A L	  O U T P U T 		D R I V E R   		*/                                                                           	
	/** ============================================================================*/
	if(dout_0_7 & (1U << 0)) HAL_GPIO_WritePin(DOUT_0_Port, DOUT_0_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(DOUT_0_Port, DOUT_0_Pin, GPIO_PIN_RESET);	
	if(dout_0_7 & (1U << 1)) HAL_GPIO_WritePin(DOUT_1_Port, DOUT_1_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(DOUT_1_Port, DOUT_1_Pin, GPIO_PIN_RESET);
	if(dout_0_7 & (1U << 2)) HAL_GPIO_WritePin(DOUT_2_Port, DOUT_2_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(DOUT_2_Port, DOUT_2_Pin, GPIO_PIN_RESET);
	if(dout_0_7 & (1U << 4)) DOUT_3_SetHigh();
	else DOUT_3_SetLow();	
	if(dout_0_7 & (1U << 5)) DOUT_4_SetHigh();
	else DOUT_4_SetLow();	
	/** ============================================================================*/                                                                         
	/**    S E T    D O O R L O C K    P W M    O R    G P I O    O U T P U T       */                                                                           	
	/** ============================================================================*/
	if(dout_0_7 & (1U << 6))  HAL_GPIO_WritePin(DOORLOCK_PWM_Port, DOORLOCK_PWM_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(DOORLOCK_PWM_Port, DOORLOCK_PWM_Pin, GPIO_PIN_RESET);
	/** ============================================================================*/                                                                         
	/**      S E T    B U Z Z E R    P W M    O R    G P I O    O U T P U T         */                                                                           	
	/** ============================================================================*/
	if(dout_0_7 & (1U << 7)) HAL_GPIO_WritePin(SOUND_PWM_Port, SOUND_PWM_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(SOUND_PWM_Port, SOUND_PWM_Pin, GPIO_PIN_RESET);

	DIO_SetOuput();
}


void DIO_SetOuput(void)
{	
	if(HAL_SPI_Transmit(&hspi2, &hc595_dout, 1U, 10U) != HAL_OK) Error_Handler();	
	HAL_GPIO_WritePin(SHIFT_CLK_Port, SHIFT_CLK_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SHIFT_CLK_Port, SHIFT_CLK_Pin, GPIO_PIN_SET);	
}


static void DIO_DebounceDigitalInput(void)
{
	static uint32_t din_0_timer = 0U;
	static uint32_t din_1_timer = 0U;
	static uint32_t din_2_timer = 0U;
	static uint32_t din_3_timer = 0U;
	static uint32_t din_4_timer = 0U;
	static uint32_t din_5_timer = 0U;
	static uint32_t din_6_timer = 0U;
	static uint32_t din_7_timer = 0U;
    static uint32_t water_flood_timer = 0U;
    static uint32_t water_flood_pcnt = 0U;
    static uint32_t handmaid_sw_time = 0U;
    static uint32_t handmaid_sw_timer = 0U;
    static uint32_t door_bell_sw_time = 0U;
    static uint32_t door_bell_sw_timer = 0U;
    static uint32_t door_bell_signal_timer = 0U;
    static uint32_t handmaid_signal_timer = 0U;
    static uint32_t old_state = 0U;
	/**
	*   debounce digital input 0 state 
	*/
	if((din_state & (1U << 0)) && (!(din_0_7 & (1U << 0))))	// CARD STACKER STATE
	{  
		if(++din_0_timer >= SLOW_INPUT_DEBOUNCE_CNT)
		{        
			din_0_timer = 0U;    // reset bit 0 debounce counter
			din_0_7 |= (1U << 0);    // change input register bit 0 state
			
			if(ROOM_Status != ROOM_HANDMAID_IN) DoNotDisturbModul_On(); 
            
			if(!IsBalconyDoorSensorActiv()) 
            {
                HVAC_Contactor_On();
            }
            
            PowerContactor_On();
			LED_RoomStatusRed_On();
		}   
	} 
	else if((!(din_state & (1U << 0))) && (din_0_7 & (1U << 0)))
	{
		if(++din_0_timer >= SLOW_INPUT_DEBOUNCE_CNT)
		{        
            din_0_timer = 0U;           // reset bit 0 debounce counter
            din_0_7 &= (~(1U << 0));    // change input register bit 0 state
			PowerContactor_Off();
            HVAC_Contactor_Off();
			LED_RoomStatusRed_Off();
		}
	} 
	else 
	{
		din_0_timer = 0U;     	// reset bit 0 debounce counter 
	}
	/**
	*   debounce digital input 1 state 
	*/
	if((din_state & (1U << 1)) && (!(din_0_7 & (1U << 1))))	// SOS ALARM TRIGGER SWITCH STATE
	{        
		if(++din_1_timer >= FAST_INPUT_DEBOUNCE_CNT)
		{        
			din_1_timer = 0U; 	// reset bit 1 debounce counter
			din_0_7 |= (1U << 1);    // change input register bit 1 state            
		}       
	} 
	else if((!(din_state & (1U << 1))) && (din_0_7 & (1U << 1)))
	{
		if(++din_1_timer >= FAST_INPUT_DEBOUNCE_CNT)
		{        
			din_1_timer = 0U; 	// reset bit 1 debounce counter
			din_0_7 &= (~(1U << 1));    // change input register bit 1 state
            
			if(!IsSosAlarmActiv())
			{
				SosAlarm_On();
				DISPLAY_SosAlarmImage();
			}			
		}        
	} 
	else 
	{
		din_1_timer = 0U;     		// reset bit 1 debounce counter
        
	}// End of check if din 1 state changed
	/**
	*   debounce digital input 2 state 
	*/
	if((din_state & (1U << 2)) && (!(din_0_7 & (1U << 2))))	// SOS ALARM RESET SWITCH STATE
	{
		if(++din_2_timer >= FAST_INPUT_DEBOUNCE_CNT)
		{
			din_2_timer = 0U; 	// reset bit 2 debounce counter
			din_0_7 |= (1U << 2);    // change input register bit 2 state
			
			if(IsSosAlarmActiv())
			{
				SosAlarm_Off();
				SignalBuzzer = BUZZ_OFF;
				DISPLAY_SosAlarmImageDelete();
			}			
		}
	} 
	else if((!(din_state & (1U << 2))) && (din_0_7 & (1U << 2)))
	{
		if(++din_2_timer >= FAST_INPUT_DEBOUNCE_CNT)
		{
			din_2_timer = 0U; 	        // reset bit 2 debounce counter
			din_0_7 &= (~(1U << 2));    // change input register bit 2 state
		}
	} 
	else 
	{
		din_2_timer = 0U;     	// reset bit 2 debounce counter

	}// End of check if din 2 state changed           
	/**
	*   debounce digital input 3 state 
	*/
	if((din_state & (1U << 3)) && (!(din_0_7 & (1U << 3))))	// HANDMAID CALL STATE
	{
		if(++din_3_timer >= INPUT_DEBOUNCE_CNT)
		{
			din_3_timer = 0U; 	// reset bit 3 debounce counter
			din_0_7 |= (1U << 3);    // change input register bit 3 state
			
			if (IsHandmaidCallActiv())
			{
				HandmaidCall_Off();
				LED_HandmaidGreen_Off();
			}			
		}  
	} 
	else if((!(din_state & (1U << 3))) && (din_0_7 & (1U << 3)))
	{
		if(++din_3_timer >= INPUT_DEBOUNCE_CNT)
		{        
			din_3_timer = 0U; 	        // reset bit 3 debounce counter
			din_0_7 &= (~(1U << 3));    // change input register bit 3 state
            
			if(IsDonNotDisturbActiv())
			{
				DoNotDisturb_Off();
				DISPLAY_DoNotDisturbImageDelete();
			}
            
			HandmaidCall_On();
			LED_HandmaidGreen_On();
		}          
	} 
	else 
	{
		din_3_timer = 0U;     	// reset bit 3 debounce counter
		  
	}// End of check if din 3 state changed
	/**
	*   debounce digital input 4 state 
	*/
	if((din_state & (1U << 4)) && (!(din_0_7 & (1U << 4))))	// MINIBAR SENSOR STATE
	{            
		if(++din_4_timer >= FAST_INPUT_DEBOUNCE_CNT)
		{        
			din_4_timer = 0U; 	// reset bit 4 debounce counter
			din_0_7 |= (1U << 4);    // change input register bit 4 state
		}        
	} 
	else if((!(din_state & (1U << 4))) && (din_0_7 & (1U << 4)))
	{
		if(++din_4_timer >= FAST_INPUT_DEBOUNCE_CNT)
		{        
			din_4_timer = 0U; 	        // reset bit 4 debounce counter
			din_0_7 &= (~(1U << 4));    // change input register bit 4 state
            DISPLAY_MinibarUsedImage();
		}            
	} 
	else 
	{
		din_4_timer = 0U;     	// reset bit 4 debounce counter
            
	}// End of check if din 4 state changed
	/**
	*   debounce digital input 5 state 
	*/
	if((din_state & (1U << 5)) && (!(din_0_7 & (1U << 5))))	// BALCONY DOOR SWITCH STATE
	{
		if(++din_5_timer >= SLOW_INPUT_DEBOUNCE_CNT)
		{
			din_5_timer = 0U; 	// reset bit 5 debounce counter
			din_0_7 |= (1U << 5);    // change input register bit 5 state
			
			if(IsIndorCardReaderActiv()) 
            {
                HVAC_Contactor_On();
            }
        }        
	} 
	else if((!(din_state & (1U << 5))) && (din_0_7 & (1U << 5)))
	{
		if(++din_5_timer >= SLOW_INPUT_DEBOUNCE_CNT)
		{        
			din_5_timer = 0U; 	// reset bit 5 debounce counter
			din_0_7 &= (~(1U << 5));    // change input register bit 5 state
			HVAC_Contactor_Off();
		}        
	} 
	else 
	{
		din_5_timer = 0U;     	// reset bit 5 debounce counter
        
	}// End of check if din 5 state changed
	/**
	*   debounce digital input 6 state 
	*/
	if((din_state & (1U << 6)) && (!(din_0_7 & (1U << 6))))	// DO NOT DISTURB SWITCH STATE
	{            
		if(++din_6_timer >= INPUT_DEBOUNCE_CNT)
		{        
			din_6_timer = 0U; 		// reset bit 6 debounce counter
			din_0_7 |= (1U << 6);    // change input register bit 6 state 
			
			if(IsDonNotDisturbActiv())
			{
				DoNotDisturb_Off();
				DISPLAY_DoNotDisturbImageDelete();
			}		
		}        
	} 
	else if((!(din_state & (1U << 6))) && (din_0_7 & (1U << 6)))
	{
 		if(++din_6_timer >= INPUT_DEBOUNCE_CNT)
		{        
			din_6_timer = 0U; 	// reset bit 6 debounce counter
			din_0_7 &= (~(1U << 6));    	// change input register bit 6 state

            if (IsHandmaidCallActiv())
			{
				HandmaidCall_Off();
				LED_HandmaidGreen_Off();
			}	
            
			DoNotDisturb_On();
			DISPLAY_DoNotDisturbImage();
        }       
	} 
	else 
	{
		din_6_timer = 0U;     // reset bit 6 debounce counter        
	}
	/**
	*   debounce digital input 7 state
	*/
	if((din_state & (1U << 7)) && (!(din_0_7 & (1U << 7))))	// ENTRY DOOR SWITCH STATE - WATER FLOOD SENSOR
	{
		if(++din_7_timer >= FAST_INPUT_DEBOUNCE_CNT)
		{
			din_7_timer = 0U; // reset bit 7 debounce counter
			din_0_7 |= (1U << 7);    // change input register bit 7 state
            WaterFloodSensorEnabled();
		}	
	} 
	else if((!(din_state & (1U << 7))) && (din_0_7 & (1U << 7)))
	{
		if(++din_7_timer >= FAST_INPUT_DEBOUNCE_CNT)
		{
			din_7_timer = 0U; // reset bit 7 debounce counter
			din_0_7 &= (~(1U << 7));    // change input register bit 7 state
		}
	} 
	else 
	{
        din_7_timer = 0U;     // reset bit 7 debounce counter

    }// End of check if din 7 state changed
	/**
	*   check door bell taster state and if pressed
	*	activate output for defined time period and
	*	wait for releaseed switch state for new cycle
	*/
	if(IsDoorBellSwitchActiv() && !IsDoorBellSwitchPressed())
	{
		DoorBellSwitchPressed();
        door_bell_sw_timer = HAL_GetTick();
        door_bell_signal_timer = HAL_GetTick();
        
		if(!IsDISPLAY_MenuActiv())
		{
            DoorBell_On();
            ONEWIRE_ButtonUpdateSet(); 	
            ONEWIRE_ThermostatDisplayDoorBell();
            SignalBuzzer = BUZZ_DOOR_BELL;
            door_bell_sw_time = DOOR_BELL_SW_ACTIV_TIME;
		}
		else
		{
            door_bell_sw_time = DOOR_BELL_SW_MENU_BUTTON_TIME;
            DISPLAY_SetMenuEvent(RIGHT_TASTER_PRESSED);
		}
	}
	else if((HAL_GetTick() - door_bell_sw_timer) >= door_bell_sw_time)
	{
		if(!(dout_0_7_remote & (1U << 3))) 
        {
            DoorBell_Off();
        }
		
		if(IsDoorBellSwitchPressed())			
		{
            LED_DoorBellBlue_On();
			DoorBellSwitchReleased();
            ONEWIRE_ButtonUpdateSet();
		}
	}
    else if(IsDoorBellSwitchPressed())
    {
        if((HAL_GetTick() - door_bell_signal_timer) >= DOOR_BELL_LED_ACTIV_TOGGLE_TIME)
        {
            door_bell_signal_timer = HAL_GetTick();
            LED_DoorBellBlue_Toggle();
        }
    }
	/**
	*	check handmaid capacitive switch
	*/
	if(IsHandmaidSwitchActiv() && !IsHandmaidSwitchPressed())
	{
		HandmaidSwitchPressed();
		handmaid_sw_timer = HAL_GetTick();
        handmaid_signal_timer = HAL_GetTick();
        
        if(IsLED_HandmaidGreen_On()) old_state = 1U;
        else old_state = 0U;
        
		if(IsDISPLAY_MenuPending())
		{
            DISPLAY_SetMenuState(PRINT);
			SignalBuzzer = BUZZ_DOOR_BELL;
            handmaid_sw_time = HANDMAID_SW_ACTIV_TIME;
		}
		else
		{
            handmaid_sw_time = HANDMAID_SW_MENU_BUTTON_TIME;
            DISPLAY_SetMenuEvent(LEFT_TASTER_PRESSED);
		}
	}
    else if((HAL_GetTick() - handmaid_sw_timer) > handmaid_sw_time) 
    {
        if(IsHandmaidSwitchPressed())
        {
            HandmaidSwitchReleased();
            if(old_state == 0U) LED_HandmaidGreen_Off();
            else LED_HandmaidGreen_On();
        }
	}
    else if(IsHandmaidSwitchPressed())
    {
        if((HAL_GetTick() - handmaid_signal_timer) >= HANDMAID_LED_ACTIV_TOGGLE_TIME)
        {
            handmaid_signal_timer = HAL_GetTick();
            LED_HandmaidGreen_Toggle();
        }
    }


    if(IsWaterFloodSensorActiv() && IsWaterFloodSensorActivated())
	{
		if((HAL_GetTick() - water_flood_timer) >= WATER_FLOOD_CHECK_CYCLE_TIME)
		{
			water_flood_timer = HAL_GetTick();
			
			if(++water_flood_pcnt >= WATER_FLOOD_MAX_CYCLES)
			{
				water_flood_pcnt = 0U;
			}
		}
	}
	else
	{
		water_flood_pcnt = 0U;
		water_flood_timer = HAL_GetTick();
	}
}

static void CAP1293_Init(void)
{
	uint8_t vendor_id, product_id, reg_wr[2];
	
	vendor_id = 0U;
	product_id = 0U;
	reg_wr[0] = CAP1293_PRODUCT_ID_REG;
	if(HAL_I2C_Master_Transmit(&hi2c1, CAP1293_WRITE, reg_wr, 1U, CAP1293_TIMEOUT) != HAL_OK) Error_Handler();
	if(HAL_I2C_Master_Receive(&hi2c1, CAP1293_READ, &product_id, 1U, CAP1293_TIMEOUT) != HAL_OK) Error_Handler();
	reg_wr[0] = CAP1293_MANUFACTURER_ID_REG;
	if(HAL_I2C_Master_Transmit(&hi2c1, CAP1293_WRITE, reg_wr, 1U, CAP1293_TIMEOUT) != HAL_OK) Error_Handler();
	if(HAL_I2C_Master_Receive(&hi2c1, CAP1293_READ, &vendor_id, 1U, CAP1293_TIMEOUT) != HAL_OK) Error_Handler();
	
	if((product_id == CAP1293_PRODUCT_ID) && (vendor_id == CAP1293_VENDOR_ID))
	{
		CAP1293_SensorPresent();
		CAP1293_WriteRegister(CAP1293_MULTIPLE_TOUCH_CONFIGURATION_REG, 0U);
		CAP1293_WriteRegister(CAP1293_SENSOR_INPUT_ENABLE_REG, 0x05U);
		CAP1293_WriteRegister(CAP1293_INTERRUPT_ENABLE_REG, 0x05U);
		CAP1293_WriteRegister(CAP1293_CONFIGURATION_2_REG, 0x40U);
		CAP1293_WriteRegister(CAP1293_REPEAT_RATE_ENABLE_REG, 0U);
		CAP1293_WriteRegister(CAP1293_SINGLE_GUARD_ENABLE_REG, 0x05U);
		CAP1293_WriteRegister(CAP1293_SENSITIVITY_CONTROL_REG, 0x0fU);
		CAP1293_WriteRegister(CAP1293_CALIBRATION_SENSITIVITY_CONFIG_REG, 0U);
		CAP1293_WriteRegister(CAP1293_MAIN_CONTROL_REG, 0U);
	}
	else CAP1293_SensorNotPresent();
}


static uint8_t CAP1293_ReadRegister(uint8_t register_address)
{
	uint8_t ret_val;
	
	if(HAL_I2C_Master_Transmit(&hi2c1, CAP1293_WRITE, &register_address, 1U, CAP1293_TIMEOUT) != HAL_OK) Error_Handler();
	if(HAL_I2C_Master_Receive(&hi2c1, CAP1293_READ, &ret_val, 1U, CAP1293_TIMEOUT) != HAL_OK) Error_Handler();
	return(ret_val);
}


static void CAP1293_WriteRegister(uint8_t register_address, uint8_t register_data)
{
	uint8_t reg_val[2];
	
	reg_val[0] = register_address;
	reg_val[1] = register_data;
	if(HAL_I2C_Master_Transmit(&hi2c1, CAP1293_WRITE, reg_val, 2U, CAP1293_TIMEOUT) != HAL_OK) Error_Handler();
}


/******************************   END OF FILE  **********************************/
