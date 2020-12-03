/**
 ******************************************************************************
 * File Name          : display.c
 * Date               : 10.3.2018
 * Description        : GUI display module
 ******************************************************************************
 *
 *  display refresh 30 min
 *
 *
 *
 */
 
 
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "eeprom.h"
#include "common.h"
#include "dio_interface.h"
#include "logger.h"
#include "rc522.h"
#include "one_wire.h"
#include "rs485.h"
#include "display.h"
#include "signal.h"


/* Imported Type  ------------------------------------------------------------*/
/* Imported Variable  --------------------------------------------------------*/
/* Imported Function  --------------------------------------------------------*/
/* Private Type --------------------------------------------------------------*/
typedef struct
{
    uint8_t index;              // hold index for selected menu screen
    uint8_t state;              // hold menu state flag for upper display function
    uint8_t select;             // hold menu screen select state for taster selection
    uint8_t event;              // hold menu event to process
    uint8_t item_index;         // index of current item in focus
    uint8_t value_index;        // index of current value unit in focus
    uint8_t value_edit;         // hold flag for enabled value edit mode for faster menu shift
    uint8_t increment;          // increment value for all type of menu edit types
    uint8_t item_type[8];       // label flag, item select, value edit
    uint8_t item_label[8];      // label text from list
    uint8_t item_to_edit[8];    // value edit item type number of values from item buffer
    uint8_t item_buff[8][8];    // value edit type buffer
    
}sMenuTypeDef;


/* Private Define ------------------------------------------------------------*/
///** ==========================================================================*/
///**       I L I 9 3 4 1	  C O N T R O L L E R 		C O M M A N D	        */
///** ==========================================================================*/
#define ILI9341_NOP                                 0x00U  
#define ILI9341_RD_DISP_POWER_MODE                  0x0aU
#define ILI9341_SLEEP_OUT                           0x11U
#define ILI9341_GAMMA_SET                           0x26U
#define ILI9341_DISP_ON                             0x29U
#define ILI9341_COLUMN_ADDRESS_SET                  0x2aU
#define ILI9341_PAGE_ADDRESS_SET                    0x2bU
#define ILI9341_MEMORY_WRITE                        0x2cU
#define ILI9341_MEMORY_ACCESS_CONTROL               0x36U
#define ILI9341_PIXEL_FORMAT_SET                    0x3aU
#define ILI9341_FRAME_RATE_CTRL                     0xb1U
#define ILI9341_DISP_FUNCTION_CTRL                  0xb6U
#define ILI9341_POWER_CTRL_1                        0xc0U
#define ILI9341_POWER_CTRL_2                        0xc1U
#define ILI9341_VCOM_CTRL_1                         0xc5U
#define ILI9341_VCOM_CTRL_2                         0xc7U
#define ILI9341_POSITIVE_GAMMA_CORRECTION           0xe0U
#define ILI9341_NEGATIVE_GAMMA_CORRECTION           0xe1U
/** ==========================================================================*/
/**		  I L I 9 3 4 1		  D I S P L A Y		  O R I E N T A T I O N		  */
/** ==========================================================================*/
#ifdef DISPLAY_HORIZONTAL
#define LCD_W 320U
#define LCD_H 240U
#endif

#ifdef DISPLAY_VERTICAL
#define LCD_W 240
#define LCD_H 320
#endif

#define DISPLAY_PIXEL_CNT  (LCD_W * LCD_H)
/** ==========================================================================*/
/**		  I L I 9 3 4 1			  D I S P L A Y			C O L O R S	 	  	  */
/** ==========================================================================*/
#define WHITE				0xFFFFU
#define BLACK				0x0000U
#define BLUE				0x001FU
#define BRED				0xF81FU
#define GRED				0xFFE0U
#define GBLUE				0x07FFU
#define RED					0xF800U
#define MAGENTA				0xF81FU
#define GREEN				0x07E0U
#define CYAN				0x7FFFU
#define YELLOW				0xFFE0U
#define BROWN				0xBC40U
#define BRRED				0xFC07U
#define GRAY				0x8430U
#define DARKBLUE			0x01CFU
#define LIGHTBLUE			0x7D7CU
#define GRAYBLUE			0x5458U
#define LIGHTGREEN			0x841FU
#define LGRAY				0xC618U
#define LGRAYBLUE			0xA651U
#define LBBLUE          	0x2B12U
/** ==========================================================================*/
/**	I L I 9 3 4 1	D I S P L A Y    F O N T	A N D	 T E X T	L I N E   */
/** ==========================================================================*/
/*	smal font number of horisontal text lines	*/
#define Line0_S       		0U
#define Line1_S      		16U
#define Line2_S      		32U
#define Line3_S     		48U
#define Line4_S      		64U
#define Line5_S     		80U
#define Line6_S     		96U
#define Line7_S   			112U
#define Line8_S 			128U
#define Line9_S				144U
#define Line10_S			160U
#define Line11_S			176U
#define Line12_S			192U
#define Line13_S			208U
#define Line14_S			224U
/*	middle font number of horisontal text lines	*/
#define Line0_M       		0U
#define Line1_M      		20U
#define Line2_M      		40U
#define Line3_M     		60U
#define Line4_M      		80U
#define Line5_M     		100U
#define Line6_M     		120U
#define Line7_M   			140U
#define Line8_M 			160U
#define Line9_M				180U
#define Line10_M			200U
#define Line11_M			220U
/*	big font number of horisontal text lines	*/
#define Line0_B       		0U
#define Line1_B     		26U
#define Line2_B     		52U
#define Line3_B     		78U
#define Line4_B       		104U
#define Line5_B       		130U
#define Line6_B      		146U
#define Line7_B      		182U
#define Line8_B      		214U
/** ==========================================================================*/
/**	    D I S P L A Y    U S E R    I N T E R F A C E    C O N S T A N T S    */
/** ==========================================================================*/
#define DISPLAY_BUFFER_SIZE						2048U
#define DISPLAY_MESSAGE_SIZE					32U
#define DISPLAY_IMAGE_TIME						5432U 				    // 5 sec. image displayed time
#define DISPLAY_STATUS_TIME						2345U                   // status info display time
#define DISPLAY_UPDATE_TIME				        333U	                // update info refresh time
#define IMAGE_SIZE								153600U				    // 320x240 RGB656 formated image
#define IMAGE_DISPLAY_POSITION					0U, 0U, 319U, 239U
#define IMAGE_MINIBAR_SIZE						2048U				    // minibar image size
#define IMAGE_MINIBAR_POSITION					0U,0U,31U,31U           // minibar icon position
#define IMAGE_MINIBAR_ROTATED_POSITION			288U, 208U, 319U, 239U
#define ROOM_NUMBER_IMAGE						1U  // image index alias
#define DO_NOT_DISTURB_IMAGE					2U  // image index alias
#define BEDDING_REPLACEMENT_IMAGE				3U  // image index alias
#define CLEAN_UP_IMAGE							4U  // image index alias
#define GENERAL_CLEAN_UP_IMAGE					5U  // image index alias
#define CARD_VALID_IMAGE						6U  // image index alias
#define CARD_INVALID_IMAGE						7U  // image index alias
#define WRONG_ROOM_IMAGE						8U  // image index alias
#define TIME_EXPIRED_IMAGE						9U  // image index alias
#define FIRE_ALARM_IMAGE						10U // image index alias
#define FIRE_EXIT_IMAGE							11U // image index alias
#define MINIBAR_IMAGE							12U // image index alias
#define ROOM_OUT_OF_SERVICE_IMAGE				13U // image index alias
#define SOS_ALARM_IMAGE							14U // image index alias


/* Private Constante ---------------------------------------------------------*/
const char *label [8][1] = {"     OFF", "      ON", "INACTIVE", "DISABLED", " SET->ON", "SET->OFF", "LuxRT.x ", "DS18B.x "};
const char *screen[5][1] = {"DIGITAL   INPUT   SETTINGS",   "DIGITAL  OUTPUT   SETTINGS",   "SETUP  SYSTEM  SETTINGS  1",   "SETUP  SYSTEM  SETTINGS  2", "EXTEND  ACCESS  PERMISSION"};
const char *item [8][4] = {{"CARD STACKER     ",            "POWER CONTACTOR  ",            "RESTART CONTROLER         ",   "DISPLAY CARD DATA"},
                           {"SOS ALARM        ",            "DND & HM RESET   ",            "PREVIEW IMAGE             ",   "TAMPER PROTECTION"},
                           {"SOS RESET        ",            "WELLCOME LIGHT   ",            "UPDATE IMAGE              ",   "RADIO MODUL      "},
                           {"HANDMAID CALL    ",            "DOOR BELL        ",            "UPDATE FIRMWARE           ",   "SET RADIO ADDRESS:        "},
                           {"MINIBAR SENSOR   ",            "HVAC POWER       ",            "SCAN ONEWIRE BUS:         ",   "SET RADIO CHANEL:         "},
                           {"BALCONY DOOR     ",            "HVAC THERMOSTAT  ",            "SET RS485 ADDRESS:        ",   "SET BUZZER VOLUME:        "},
                           {"DND SWITCH       ",            "DOOR LOCK COIL   ",            "SET SYSTEM ID:            ",   "SET DOORLOCK FORCE:       "},
                           {"FLOOD SENSOR     ",            "PCB BUZZER       ",            "SET TIME:                 ",   "SET LCD BRIGHTNESS:       "}};
const char *taster[3][1] = {"ENTER              >> NEXT",   "NEXT >>             SELECT",   "SET ++            DESELECT"};

/* Private Variable ----------------------------------------------------------*/
__IO uint32_t display_menu_timeout_timer;
__IO uint32_t display_menu_timeout_time;
__IO uint32_t display_flags;
uint8_t display_status;
uint8_t journal_mode;

static sMenuTypeDef menu;
eDisplayFontSizeTypeDef font_size;

uint8_t spi_buff[8];
uint16_t lcd_brightness;
uint16_t back_color, pixel_color;


/* Private Macro -------------------------------------------------------------*/
/* Private Function Prototype ------------------------------------------------*/
static void DISPLAY_PrintNumber(uint16_t x, uint16_t y, uint32_t num, uint8_t len);
static void DISPLAY_AddressSet(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
static void DISPLAY_PrintCharacter(uint16_t x, uint16_t y, uint8_t num);
static void DISPLAY_PrintString(uint16_t x, uint16_t y, const char *p);
static void DISPLAY_ProcessMenuTasterEvent(sMenuTypeDef* menu);
static void DISPLAY_PrintMenuScreen(sMenuTypeDef* menu);
static void DISPLAY_PrintMenuValue(sMenuTypeDef* menu);
static void DISPLAY_MenuService(sMenuTypeDef* menu);
static void DISPLAY_InitMenu(sMenuTypeDef* menu);
static void DISPLAY_WriteRegister(uint8_t data);
static void DISPLAY_CardInfoText(uint8_t user);
static void DISPLAY_Image(uint8_t selected);
static void DISPLAY_WriteByte(uint8_t data);
static void DISPLAY_WriteInt(uint16_t data);
static void DISPLAY_Clear(uint16_t color);
//static void DISPLAY_CardValidDate(void);
static void DISPLAY_CardUserName(void);
static void DISPLAY_Temperature(void);
static void DISPLAY_DateTime(void);
static void DISPLAY_CheckLcd(void);

uint8_t DISPLAY_ReadCommand(uint8_t command,uint8_t index);


/* Program Code  -------------------------------------------------------------*/
/*************************************************************************/
/**     I N I T I A L I Z E     L C D       C O N T R O L L E R         **/
/*************************************************************************/
void DISPLAY_Init(void)
{
	HAL_GPIO_WritePin(DISPLAY_CS_Port, DISPLAY_CS_Pin, GPIO_PIN_SET);
    DISPLAY_RST_SetHigh();
    DIO_SetOuput();
    HAL_Delay(5U);
    DISPLAY_RST_SetLow();
    DIO_SetOuput();
    HAL_Delay(50U);
    DISPLAY_RST_SetHigh();
    DIO_SetOuput();
    HAL_Delay(200U);

    DISPLAY_WriteRegister(0xefU);
    DISPLAY_WriteByte(0x03U); 
    DISPLAY_WriteByte(0x80U); 
    DISPLAY_WriteByte(0x02U);

    DISPLAY_WriteRegister(0xcfU);
    DISPLAY_WriteByte(0x00U); 
    DISPLAY_WriteByte(0xc1U); 
    DISPLAY_WriteByte(0x30U);
    
    DISPLAY_WriteRegister(0xedU);
    DISPLAY_WriteByte(0x64U); 
    DISPLAY_WriteByte(0x03U); 
    DISPLAY_WriteByte(0x12U);
    DISPLAY_WriteByte(0x81U);
    
    DISPLAY_WriteRegister(0xe8U);
    DISPLAY_WriteByte(0x85U); 
    DISPLAY_WriteByte(0x00U); 
    DISPLAY_WriteByte(0x78U);
    
    DISPLAY_WriteRegister(0xcbU);
    DISPLAY_WriteByte(0x39U); 
    DISPLAY_WriteByte(0x2cU); 
    DISPLAY_WriteByte(0x00U);
    DISPLAY_WriteByte(0x34U); 
    DISPLAY_WriteByte(0x02U);
	    
    DISPLAY_WriteRegister(0xf7U);
    DISPLAY_WriteByte(0x20U);
        
    DISPLAY_WriteRegister(0xeaU);
    DISPLAY_WriteByte(0x00U); 
    DISPLAY_WriteByte(0x00U);

    DISPLAY_WriteRegister(ILI9341_POWER_CTRL_1);
    DISPLAY_WriteByte(0x23U);
    
    DISPLAY_WriteRegister(ILI9341_POWER_CTRL_2);
    DISPLAY_WriteByte(0x10U);   
    
    DISPLAY_WriteRegister(ILI9341_VCOM_CTRL_1);
    DISPLAY_WriteByte(0x3eU); 
    DISPLAY_WriteByte(0x28U);
        
    DISPLAY_WriteRegister(ILI9341_VCOM_CTRL_2);
#ifdef DISPLAY_HORIZONTAL
#ifdef ROTATE_DISPLAY
	DISPLAY_WriteByte(0xe8);
#else
	DISPLAY_WriteByte(0x28);        // 1-st parameter "defines read/write scanning direction = column address order, BGR color filter panel" 
#endif
#endif

#ifdef DISPLAY_VERTICAL
#ifdef ROTATE_DISPLAY
	DISPLAY_WriteByte(0xc8);
#else
	DISPLAY_WriteByte(0x48);        // 1-st parameter "defines read/write scanning direction = column address order, BGR color filter panel" 
#endif
    
#endif 
        
    DISPLAY_WriteRegister(ILI9341_MEMORY_ACCESS_CONTROL);
    DISPLAY_WriteByte(0xE8U);    
        
    DISPLAY_WriteRegister(ILI9341_PIXEL_FORMAT_SET);
    DISPLAY_WriteByte(0x55U);
    
    DISPLAY_WriteRegister(ILI9341_FRAME_RATE_CTRL);
    DISPLAY_WriteByte(0x00U); 
    DISPLAY_WriteByte(0x18U);
   
    DISPLAY_WriteRegister(ILI9341_DISP_FUNCTION_CTRL);
    DISPLAY_WriteByte(0x08U); 
    DISPLAY_WriteByte(0x82U); 
    DISPLAY_WriteByte(0x27U);
   
    DISPLAY_WriteRegister(0xf2U);
    DISPLAY_WriteByte(0x00U);
    
    DISPLAY_WriteRegister(ILI9341_GAMMA_SET);
    DISPLAY_WriteByte(0x01U);
    
    DISPLAY_WriteRegister(ILI9341_POSITIVE_GAMMA_CORRECTION);
    DISPLAY_WriteByte(0x0fU);
    DISPLAY_WriteByte(0x31U);
    DISPLAY_WriteByte(0x2bU);
    DISPLAY_WriteByte(0x0cU); 
    DISPLAY_WriteByte(0x0eU);
    DISPLAY_WriteByte(0x08U);
    DISPLAY_WriteByte(0x4eU);
    DISPLAY_WriteByte(0xf1U);
    DISPLAY_WriteByte(0x37U);
    DISPLAY_WriteByte(0x07U);
    DISPLAY_WriteByte(0x10U);
    DISPLAY_WriteByte(0x03U);
    DISPLAY_WriteByte(0x0eU);
    DISPLAY_WriteByte(0x09U);
    DISPLAY_WriteByte(0x00U);

    DISPLAY_WriteRegister(ILI9341_NEGATIVE_GAMMA_CORRECTION);
    DISPLAY_WriteByte(0x00U);
    DISPLAY_WriteByte(0x0eU);
    DISPLAY_WriteByte(0x14U);
    DISPLAY_WriteByte(0x03U);
    DISPLAY_WriteByte(0x11U);
    DISPLAY_WriteByte(0x07U);
    DISPLAY_WriteByte(0x31U);
    DISPLAY_WriteByte(0xc1U); 
    DISPLAY_WriteByte(0x48U);
    DISPLAY_WriteByte(0x08U);    
    DISPLAY_WriteByte(0x0fU);
    DISPLAY_WriteByte(0x0cU);
    DISPLAY_WriteByte(0x31U);
    DISPLAY_WriteByte(0x36U); 
    DISPLAY_WriteByte(0x0fU);  
        
    DISPLAY_WriteRegister(0xb1U);
    DISPLAY_WriteByte(0x00U); 
    DISPLAY_WriteByte(0x10U);
    
    DISPLAY_WriteRegister(ILI9341_SLEEP_OUT); 
    HAL_Delay(200U);
    DISPLAY_WriteRegister(ILI9341_DISP_ON);
    /*************************************************************************/
    /**     D I S P L A Y       S Y S T E M         S E T T I N G S         **/
    /*************************************************************************/ 
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, lcd_brightness);
    DISPLAY_Clear(BLACK);
    back_color = BLACK;
    pixel_color = WHITE;   
    font_size = SMALL_FONT;
    DISPLAY_PrintString(10U, Line0_S, "I-MEDIA GROUP d.o.o. Sarajevo - 2018.");			
    DISPLAY_PrintString(20U, Line2_M, "LUX - Hotel Room & Access Controller");
    DISPLAY_PrintString(0U, Line5_S, sys_info_hw);
    DISPLAY_PrintString(0U, Line6_S, sys_info_fw);
    DISPLAY_PrintString(0U, Line8_S,  "System ID:");
    DISPLAY_PrintNumber(140U, Line8_S, ((system_id[0] << 8U) + system_id[1]), 5U);
    DISPLAY_PrintString(0U, Line9_S,  "RS485 baudrate:");
    DISPLAY_PrintNumber(140U, Line9_S, huart1.Init.BaudRate, 6U);
    DISPLAY_PrintString(0U, Line10_S, "RS485 address:");
    DISPLAY_PrintNumber(140U, Line10_S, ((rs485_interface_address[0] << 8U) + rs485_interface_address[1]), 5U);
    DISPLAY_PrintString(0U, Line11_S, "OneWire device:");
                                                                       
    if(IsONEWIRE_DalasSensorConnected()) DISPLAY_PrintString(140U, Line11_S, "DS18B20 Temp.Sensor");
    else if (IsONEWIRE_ThermostatConnected()) 
    {
        DISPLAY_PrintNumber(140U, Line11_S, onewire_device_connected, 1U);
        DISPLAY_PrintString(160U, Line11_S, "LUX Thermostat");
        if(onewire_device_connected > 1U) DISPLAY_PrintCharacter(272U, Line11_S, 's');
    }
    else DISPLAY_PrintString(140U, Line11_S, "NOT DETECTED");
    DISPLAY_RoomNumberImage();
    HAL_Delay(1500U);
#ifndef	USE_DEBUGGER
    HAL_IWDG_Refresh(&hiwdg);
#endif
    HAL_Delay(1500U);
#ifndef	USE_DEBUGGER
    HAL_IWDG_Refresh(&hiwdg);
#endif
}

/*************************************************************************/
/**         D I S P L A Y           U S E R          G U I              **/
/*************************************************************************/ 
void DISPLAY_Service(void) 
{
    uint8_t disp_buff[DISPLAY_MESSAGE_SIZE];
    uint32_t row_cnt, colon_cnt, space_cnt, item_cnt, char_cnt;
    uint32_t char_x_pos, char_y_pos;
    static uint32_t display_time = 0U, display_timer = 0U;
	static uint32_t preview_img_cnt = 0U;
    static uint32_t jrn_char = 0U;
    /*************************************************************************/
    /**		    E N T R Y       C O N D I T I O N S     C H E C K           **/
    /*************************************************************************/	
	if(eComState == COM_PACKET_RECEIVED) RS485_Service();
    if((HAL_GetTick() - display_timer) < display_time) return;
    display_time = 0U;
    display_timer = HAL_GetTick();
    ClearBuffer(disp_buff, DISPLAY_MESSAGE_SIZE);
    DISPLAY_CheckLcd();
    /*************************************************************************/
    /**     D I S P L A Y       U P D A T E         I N F O                 **/
    /*************************************************************************/
    if(IsRS485_UpdateActiv())
    {
        if(!IsDISPLAY_UpdateInProgressMessageActiv() )
        {
            DISPLAY_UpdateInProgressMessage();              // run only once per call
            display_time = DISPLAY_STATUS_TIME;             // set time to display update info message 
            back_color = BLACK;
            pixel_color = WHITE;
            font_size = MIDDLE_FONT;
            DISPLAY_PrintString(0U, 188U, "      ");         // clear temperature displayed area
            font_size = BIG_FONT;
            
            if(IsDISPLAY_BootloaderUpdatedActiv() || IsDISPLAY_BootloaderUpdateFailActiv())
            {
                if      (IsDISPLAY_BootloaderUpdatedActiv())            DISPLAY_PrintString(  0U, Line8_B, "Bootloader Updated  ");
                else if(IsDISPLAY_BootloaderUpdateFailActiv())          DISPLAY_PrintString(  0U, Line8_B, "Bootloader Failed   ");
                
                DISPLAY_BootloaderUpdatedDelete();
                DISPLAY_BootloaderUpdateFailDelete();					
                DISPLAY_UpdateInProgressMessageDelete();
                DISPLAY_RefreshSet();                                                          
                RS485_StopUpdate();
            }
            else
            {                                                           DISPLAY_PrintString(  0U, Line8_B, "UPDATE:             ");
                if      (activ_command == DOWNLOAD_FIRMWARE_IMAGE)      DISPLAY_PrintString(140U, Line8_B, "FIRMWARE");
                else if (activ_command == DOWNLOAD_BOOTLOADER_IMAGE)    DISPLAY_PrintString(140U, Line8_B, "BOOTLOADER");
                else if (activ_command == DOWNLOAD_SMALL_FONT)          DISPLAY_PrintString(140U, Line8_B, "SMALL FONT");
                else if (activ_command == DOWNLOAD_MIDDLE_FONT)         DISPLAY_PrintString(140U, Line8_B, "MIDDLE FONT");
                else if (activ_command == DOWNLOAD_BIG_FONT)            DISPLAY_PrintString(140U, Line8_B, "BIG FONT");
                else if ((activ_command >= DOWNLOAD_DISPLAY_IMAGE_1) && (activ_command <= DOWNLOAD_DISPLAY_IMAGE_29))
                {
                    DISPLAY_PrintString(140U, Line8_B,  "IMAGE");
                    DISPLAY_PrintNumber(260U, Line8_B, (activ_command - 0x63U), 2U);
                }
            } 
        }
        return;
    }
    /*************************************************************************/
    /**     D I S P L A Y   F I L E   T R A N S F E R   S T A T U S         **/
    /*************************************************************************/
    else if(IsDISPLAY_UpdateInProgressMessageActiv())
    {
        DISPLAY_UpdateInProgressMessageDelete();        // run only once per call
        display_time = DISPLAY_STATUS_TIME;             // set time to display update info message 
        DISPLAY_RefreshSet();                           // reload last display image after this time expire
        back_color = BLACK;
        pixel_color = WHITE;   
        font_size = MIDDLE_FONT;
        DISPLAY_PrintString(0U, 188U, "      ");         // clear temperature displayed area
        font_size = BIG_FONT;               
        if(IsSYS_FileTransferSuccessSet())              DISPLAY_PrintString(0U, Line8_B, "Transfer Successful ");
        else if(IsSYS_FileTransferFailSet())            DISPLAY_PrintString(0U, Line8_B, "Transfer Fail       ");
        return;
    }
    /*************************************************************************/
    /**     D I S P L A Y   F I R M W A R E   U P D A T E   S T A T U S     **/
    /*************************************************************************/
    else if(IsDISPLAY_FirmwareUpdatedActiv() || IsDISPLAY_FirmwareUpdateFailActiv())
    {
        back_color = BLACK;
        pixel_color = WHITE;
        font_size = MIDDLE_FONT;
        DISPLAY_PrintString(0U, 188U, "      ");         // clear temperature displayed area
        font_size = BIG_FONT;
        
        if      (IsDISPLAY_FirmwareUpdatedActiv())      DISPLAY_PrintString(0U, Line8_B, "Firmware Updated    ");
        else if (IsDISPLAY_FirmwareUpdateFailActiv())   DISPLAY_PrintString(0U, Line8_B, "Firmware Update Fail");
        
        DISPLAY_FirmwareUpdateFailDelete();             // not needed to refresh display ,
        DISPLAY_FirmwareUpdatedDelete();                // when time for this message expire,
        display_time = DISPLAY_STATUS_TIME;             // new date & time info will override mesasge text 
        return;
    }
    /*************************************************************************/
    /**     D I S P L A Y   A L L   I M A G E   F R O M     F L A S H       **/
    /*************************************************************************/
    if(IsDISPLAY_PreviewImageActiv())
    {
        ++preview_img_cnt;                      // first image index is 1
        
        if(preview_img_cnt > SOS_ALARM_IMAGE)   // last image index is 14
        {
            preview_img_cnt = 0U;               // all images,if any, are displayed from flash  memory
            DISPLAY_PreviewImageDelete();       // exit loop with index set to zero for next entrance
        }
        else 
        {
            DISPLAY_Image(preview_img_cnt);     // display all image stored in flash memory 
            display_time = DISPLAY_STATUS_TIME; // set time to display image
            return;
        }
    }
    /*************************************************************************/
    /**         E V E N T   =   U S E R     C A R D     V A L I D           **/
    /*************************************************************************/   
    else if(IsDISPLAY_CardValidImageActiv()) 
    {
        DISPLAY_CardValidImageDelete();             // first delete flag to disable reentrance if something goes wrong
        DISPLAY_Image(CARD_VALID_IMAGE);            // now show flash memory content for image index
        display_time = DISPLAY_IMAGE_TIME;          // set image diplay time
        /*************************************************************************/
        /**     D I S P L A Y   C A R D   I N F O   I F     E N A B L E D       **/
        /*************************************************************************/ 
        if(IsDISPLAY_UserCardInfoTextEnabled())
        {
            DISPLAY_CardInfoText(sCard.user_group);
            DISPLAY_CardUserName();
//            if((sCard.user_group == CARD_USER_GROUP_HANDMAID) || (sCard.user_group == CARD_USER_GROUP_SERVICE)) DISPLAY_CardValidDate();
        }
        /*************************************************************************/
        /**     D I S P L A Y       U S E R     M E N U     O P T I O N S       **/
        /*************************************************************************/ 
        if(sCard.user_group == CARD_USER_GROUP_HANDMAID)
        {
            font_size = MIDDLE_FONT;
            back_color = BLACK;
            pixel_color = YELLOW;
            DISPLAY_PrintString(0U, Line8_B, "JOURNAL");
            DISPLAY_InitMenu(&menu);
            journal_mode = 1U;
        }
        else if(sCard.user_group == CARD_USER_GROUP_GUEST)
        {
            DISPLAY_InitMenu(&menu);
            journal_mode = 0U;
        }
        else if(sCard.user_group == CARD_USER_GROUP_SERVICE)
        {
            font_size = MIDDLE_FONT;
            back_color = BLACK;
            pixel_color = YELLOW;
            DISPLAY_PrintString(0U, Line8_B, "SETTINGS");
            DISPLAY_InitMenu(&menu);
            DISPLAY_SetMenuState(PENDING);
            journal_mode = 0U;
        }
        
        RC522_ClearData();
        return;
    }
    /*************************************************************************/
    /**         E V E N T   =   U S E R     C A R D    I N V A L I D        **/
    /*************************************************************************/
    else if(IsDISPLAY_CardInvalidImageActiv()) 
    {
        DISPLAY_CardInvalidImageDelete();
        DISPLAY_Image(CARD_INVALID_IMAGE);
        display_time = DISPLAY_IMAGE_TIME;
        DISPLAY_InitMenu(&menu); 
        journal_mode = 0U;
        return;
    }
    /*************************************************************************/
    /**              D I S P L A Y      M E N U     S C R E E N             **/
    /*************************************************************************/
    else if(IsDISPLAY_MenuPrintPending())
    {
        display_menu_timeout_time = MENU_TIMEOUT;
        display_menu_timeout_timer = HAL_GetTick();
        DISPLAY_PrintMenuScreen(&menu);
        DISPLAY_PrintMenuValue(&menu);
        DISPLAY_SetMenuState(ACTIV);
        return;
    }
    /*************************************************************************/
    /**     D I S P L A Y      S C R E E N         M E N U      V A L U E   **/
    /*************************************************************************/
    else if(IsDISPLAY_MenuValueChanged())
    {
        display_menu_timeout_time = MENU_TIMEOUT;
        display_menu_timeout_timer = HAL_GetTick();
        DISPLAY_PrintMenuValue(&menu);
        DISPLAY_SetMenuState(ACTIV);
        return;
    }
    /*************************************************************************/
    /**     D I S P L A Y       M E N U     T I M E O U T       T I M E R   **/
    /*************************************************************************/
    if(journal_mode != 0U)
    { 
        if((HAL_GetTick() - display_menu_timeout_timer) >= display_menu_timeout_time)
        {
            journal_mode = 0U;
            DISPLAY_RefreshSet();    // destroy current display menu setting to null
        }
        else if((journal_mode == 2U) || (journal_mode == 4U))
        {
            font_size = MIDDLE_FONT;
            pixel_color = WHITE;
            char_cnt = 0U;
            row_cnt = 0U;
            colon_cnt = 0U;
            space_cnt = 0U;
            item_cnt = 0U;
            char_x_pos = 80U;
            char_y_pos = Line2_M;
            display_menu_timeout_time = MENU_TIMEOUT;
            display_menu_timeout_timer = HAL_GetTick();
            DISPLAY_Clear(BLACK);
            DISPLAY_PrintString( 60,  Line0_M,  "CISC. ZAMJ. GEN. ZBIR");
            DISPLAY_PrintString(  0,  Line8_M,  "HOTEL");
            DISPLAY_PrintString(  0, Line10_M,  "--------------------------");
            DISPLAY_PrintString(  0, Line11_M,  "NEXT >>               EXIT");
            
            for(uint32_t t = 0U; t < 5U; t++)
            {
                DISPLAY_PrintString(12, Line2_M + (t * Line1_M), ".SP");
                
                if(journal_mode == 2U) DISPLAY_PrintCharacter(0, Line2_M + (t * Line1_M), (t + '1'));
                else if(journal_mode == 4U) 
                {
                    DISPLAY_PrintCharacter(0, Line2_M + (t * Line1_M), (t + '6'));
                    if(t == 4U) DISPLAY_PrintString(0, Line6_M, "10.S");
                }
            }
            
            if(journal_mode == 4U) char_cnt = jrn_char;
            
            while((aJournal_1[char_cnt] != 0U) && (aJournal_1[char_cnt] != ';') && (item_cnt < (journal_mode * 12U)))
            {
                DISPLAY_PrintCharacter(char_x_pos, char_y_pos, aJournal_1[char_cnt]);
                char_x_pos += 12U;
                ++char_cnt;
                ++space_cnt;
                
                if(aJournal_1[char_cnt] == ',')
                {
                    ++char_cnt;	
                    ++item_cnt;
                    ++row_cnt;
                    
                    if(row_cnt == 4U)
                    {			
                        row_cnt = 0U;
                        char_x_pos = 80U;
                        char_y_pos += 20U;
                        ++colon_cnt;
                        
                        if(colon_cnt == 5U)
                        {
                            char_y_pos += 20U;
                        }
                    }
                    else
                    {
                        char_x_pos += (((4U - space_cnt) * 12U) + 15U);
                    }
                    
                    space_cnt = 0U;			
                }
            }
            
            if(journal_mode == 2U) jrn_char = char_cnt;
            ++journal_mode;
            return;
        }
    }
    else if(IsDISPLAY_MenuActiv())
    {
        if((HAL_GetTick() - display_menu_timeout_timer) >= display_menu_timeout_time)
        {
            DISPLAY_SetMenuState(IDLE);
            DISPLAY_RefreshSet();    // destroy current display menu setting all to null
        }
        else
        {
            DISPLAY_ProcessMenuTasterEvent(&menu);
            if(IsDISPLAY_MenuTasterEventEnd())
            {
                DISPLAY_MenuService(&menu);
                DISPLAY_SetMenuEvent(TASTER_IDLE);
            }
            return;
        }
    }
    /*************************************************************************/
    /**   E V E N T   =   R O O M   O R   S Y S T E M   I D   I N V A L I D **/
    /*************************************************************************/
    else if(IsDISPLAY_WrongRoomImageActiv()) 
    {
        DISPLAY_Image(WRONG_ROOM_IMAGE);
        DISPLAY_WrongRoomImageDelete();
        display_time = DISPLAY_IMAGE_TIME;
    }
    /*************************************************************************/
    /**         E V E N T   =   C A R D    T I M E      E X P I R E D       **/
    /*************************************************************************/
    else if(IsDISPLAY_TimeExpiredImageActiv()) 
    {
        DISPLAY_Image(TIME_EXPIRED_IMAGE);
        DISPLAY_TimeExpiredImageDelete();
        display_time = DISPLAY_IMAGE_TIME;
    }
    /*************************************************************************/
    /**         E V E N T   =   R O O M   O U T   O F   S E R V I C E       **/
    /*************************************************************************/
    else if(IsDISPLAY_RoomOutOfServiceImageActiv()) 
    {
        DISPLAY_Image(ROOM_OUT_OF_SERVICE_IMAGE);
        DISPLAY_RoomOutOfServiceImageDelete();
        display_time = DISPLAY_IMAGE_TIME;
    }
    /*************************************************************************/
    /**         D I S P L A Y       R E F R E S H       R E Q U E S T       **/
    /*************************************************************************/
    else if(IsDISPLAY_RefreshRequested())
    {
        DISPLAY_RefreshReset();
        
        if(IsDISPLAY_SosAlarmImageActiv()) 
        {
            DISPLAY_Image(SOS_ALARM_IMAGE);
            SignalBuzzer = BUZZ_SOS_ALARM;
        }
        else if(IsDISPLAY_FireExitImageActiv()) 
        {
            DISPLAY_Image(FIRE_EXIT_IMAGE);
            SignalBuzzer = BUZZ_FIRE_ALARM;
        }
        else if(IsDISPLAY_FireAlarmImageActiv()) 
        {
            DISPLAY_Image(FIRE_ALARM_IMAGE);
            SignalBuzzer = BUZZ_FIRE_ALARM;
        }
        else if(IsDISPLAY_DoNotDisturbImageActiv()) DISPLAY_Image(DO_NOT_DISTURB_IMAGE);
        else if(IsDISPLAY_BeddingReplacementImageActiv()) DISPLAY_Image(BEDDING_REPLACEMENT_IMAGE);
        else if(IsDISPLAY_CleanUpImageActiv()) DISPLAY_Image(CLEAN_UP_IMAGE);
        else if(IsDISPLAY_GeneralCleanUpImageActiv()) DISPLAY_Image(GENERAL_CLEAN_UP_IMAGE);
        else if(IsDISPLAY_RoomNumberImageActiv()) DISPLAY_Image(ROOM_NUMBER_IMAGE);
        
        if(IsDISPLAY_MinibarUsedImageActiv() && !IsDISPLAY_FireAlarmImageActiv() \
            && !IsDISPLAY_FireExitImageActiv() && !IsDISPLAY_SosAlarmImageActiv())
        {
            DISPLAY_Image(MINIBAR_IMAGE);
        }
    }
    /*************************************************************************/
    /**         D I S P L A Y       R O O M         I N F O                 **/
    /*************************************************************************/
    else
    {
        DISPLAY_DateTime();
        DISPLAY_Temperature();
    }
}


void DISPLAY_SetMenuState(uint8_t set_state)
{
    menu.state = set_state;
}


void DISPLAY_SetMenuEvent(const eTasterEventTypeDef set_event)
{
    menu.event = set_event;
}


uint8_t IsDISPLAY_MenuActiv(void)
{
    if(menu.state == IDLE) return (0U);
    return(1U);
}


uint8_t IsDISPLAY_MenuPending(void)
{
    if(menu.state == PENDING) return (1U);
    return(0U);
}


uint8_t IsDISPLAY_MenuPrintPending(void)
{
    if(menu.state == PRINT) return (1U);
    return(0U);
}


uint8_t IsDISPLAY_MenuValueChanged(void)
{
    if(menu.state == CHANGED) return (1U);
    return(0U);
}


uint8_t IsDISPLAY_MenuTasterEventEnd(void)
{
    if(menu.event == TASTER_END) return (1U);
    return(0U);
}



uint8_t DISPLAY_ReadCommand(uint8_t command, uint8_t index) 
{
    spi_buff[0] = 0U;
    DISPLAY_WriteRegister(0xD9U);
    DISPLAY_WriteByte(0x10U + index);
    DISPLAY_WriteRegister(command);
    HAL_GPIO_WritePin(DISPLAY_DC_Port, DISPLAY_DC_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(DISPLAY_CS_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi2, spi_buff, 1U, 10U);
    HAL_SPI_Receive(&hspi2, spi_buff, 1U, 10U);
    HAL_GPIO_WritePin(DISPLAY_CS_Port, DISPLAY_CS_Pin, GPIO_PIN_SET);
    return (spi_buff[0]);
}


static void DISPLAY_MenuService(sMenuTypeDef* menu)
{
    uint32_t val;
    RTC_TimeTypeDef time_ms;
    RTC_DateTypeDef date_ms;
    /*************************************************************************/
    /**     S E T   S E L E C T E D     M E N U     I T E M     V A L U E   **/
    /*************************************************************************/    
    if(menu->select == 0U)       // exit screen selected
    {
        if(menu->index < 2U)         // force digital input or output state from service menu
        {
            /**
            *   disable dout forcing on exit from menu screen
            */
            if(dout_0_7_remote & (1U << 8))
            {
                dout_0_7_remote &= (~(1U << 8));
            }
            
            DIN_ForcingDisabled();
        }
    }
    else if(menu->select == 1U)  // screen selected or item value edit deselected
    {          
    }
    else if(menu->select == 2U)  // screen item value edit mode selected
    {
        if(menu->index < 2U)         // force digital input or output state from service menu
        {
            if(menu->increment < 4U) menu->increment = 4U;
            else if(menu->increment > 5U) menu->increment = 4U;
            menu->item_label[menu->item_index] = menu->increment;
            
            if(menu->index == 0U)        // force digital input state from service menu
            {
                DIN_ForcingEnabled();
                
                for(uint32_t t = 0U; t < 8U; t++)
                {                    
                    if(menu->item_label[t] == 4U)       // set din on
                    {
                        din_state |= (1U << t);
                        din_0_7 &= (~(1U << t));
                    }
                    else if(menu->item_label[t] == 5U)  // set din off
                    {
                        din_0_7 |= (1U << t); 
                        din_state &= (~(1U << t));
                    }
                }	
            }
            else if(menu->index  == 1U)  // force digital output state from service menu
            {
                dout_0_7_remote |= (1U << 8);
                
                for(uint32_t t = 0U; t < 8U; t++)
                {                    
                    if(menu->item_label[t] == 4U)       // set dout on
                    {
                        dout_0_7_remote |= (1U << t);
                    }
                    else if(menu->item_label[t] == 5U)  // set dout off
                    {
                        dout_0_7_remote &= (~(1U << t));
                    }
                }
            }
        }
        else if(menu->index == 2U)   // settings 1 menu state update
        {
            if(menu->item_index == 0U)      // restart controller selected
            {
                BootloaderExe();
            }
            else if(menu->item_index == 1U) // preview all display image selected
            {
                DISPLAY_SetMenuState(IDLE);
                DISPLAY_PreviewImage();
            }
            else if(menu->item_index == 2U) // update all display image request
            {
                DISPLAY_SetMenuState(IDLE);
                SYS_ImageUpdateRequestSet();
            }
            else if(menu->item_index == 3U) // update controller firmware request
            {
                DISPLAY_SetMenuState(IDLE);
                SYS_FirmwareUpdateRequestSet();
            }
            else if(menu->item_index == 4U) // scan onewire bus
            {
                ONEWIRE_ScanBus();
                if(IsONEWIRE_DalasSensorConnected()) menu->item_label[4] = 7U;
                else if(IsONEWIRE_ThermostatConnected()) 
                {
                    menu->item_label[4] = 6U;
                    EEPROM_Save(EE_ONEWIRE_ADDRESS_1, thermostat_addresse, 9U);
                }
                else menu->item_label[4] = 2U;
                menu->item_type[4] = LABEL_ITEM;
                menu->select = 1U;
            }
            else if(menu->item_index == 5U) // set rs485 interface address
            {
                if(menu->increment > 9U) menu->increment = 0U;
                menu->item_buff[menu->item_index][menu->value_index] = menu->increment + '0';
                Str2Int(&menu->item_buff[menu->item_index][0], &val);
                rs485_interface_address[0] = ((val >> 8U) & 0xFFU);
                rs485_interface_address[1] = (val & 0xFFU);
                EEPROM_Save(EE_RS485_INTERFACE_ADDRESS, rs485_interface_address, 2U);
            }
            else if(menu->item_index == 6U) // set system id
            {
                if(menu->increment > 9U) menu->increment = 0U;
                menu->item_buff[menu->item_index][menu->value_index] = menu->increment + '0';
                Str2Int(&menu->item_buff[menu->item_index][0], &val);
                system_id[0] = ((val >> 8U) & 0xFFU);
                system_id[1] = (val & 0xFFU);
                EEPROM_Save(EE_SYSTEM_ID_ADDRESS, system_id, 2U);
            }
            else if(menu->item_index == 7U) // set date & time
            {
                if(menu->value_index == 0U)       // edit hours
                {
                    if(menu->increment > 23U) menu->increment = 0U;
                }
                else if(menu->value_index == 1U)  // edit minutes
                {
                    if(menu->increment > 59U) menu->increment = 0U;
                }
                else if(menu->value_index == 2U)  // edit date
                {
                    if(menu->increment > 31U) menu->increment = 1U;
                }
                else if(menu->value_index == 3U)  // edit month
                {
                    if(menu->increment > 12U) menu->increment = 1U;
                }
                else if(menu->value_index == 4U)  // edit year
                {
                    if(menu->increment > 99U) menu->increment = 18U;
                }

                menu->item_buff[7U][menu->value_index] = Dec2Bcd(menu->increment);
                
                time_ms.Hours   = menu->item_buff[7U][0];
                time_ms.Minutes = menu->item_buff[7U][1];
                date_ms.Date    = menu->item_buff[7U][2];
                date_ms.Month   = menu->item_buff[7U][3];
                date_ms.Year    = menu->item_buff[7U][4];
                
                HAL_RTC_SetTime(&hrtc, &time_ms, RTC_FORMAT_BCD);
                HAL_RTC_SetDate(&hrtc, &date_ms, RTC_FORMAT_BCD);
                
                ONEWIRE_TimeUpdateSet();
            }    
        }
        else if(menu->index == 3U)   // settings 2 menu state update
        {
            if(menu->item_index == 0U)      // display user data from card
            {
                if(menu->increment > 1U) menu->increment = 0U;
                menu->item_label[menu->item_index] = menu->increment;
                if(menu->increment == 0U) DISPLAY_UserCardInfoTextDisable();
                else if(menu->increment == 1U) DISPLAY_UserCardInfoTextEnable();
                EEPROM_Save(EE_DISPLAY_STATUS_ADDRESS, &display_status, 1U);
            }
            else if(menu->item_index == 1U) // tamper protection switch and light sensor
            {
            }
            else if(menu->item_index == 2U) // 2,4GHz wireless radio modul
            {
            }
            else if(menu->item_index == 3U) // radio address
            {
            }
            else if(menu->item_index == 4U) // radio chanel
            {
            }
            else if(menu->item_index == 5U) // set buzzer volume
            {
                if(menu->increment > 10U) menu->increment = 0U;
                Int2StrSized(&menu->item_buff[5][0], menu->increment, 2U);
                buzzer_volume = menu->increment;
                EEPROM_Save(EE_BUZZER_VOLUME_ADDRESS, &buzzer_volume, 1U);
            }
            else if(menu->item_index == 6U) // set doorlock force
            {
                if(menu->increment > 10U) menu->increment = 0U;
                Int2StrSized(&menu->item_buff[6][0], menu->increment, 2U);
                doorlock_force = menu->increment;
                EEPROM_Save(EE_DOORLOCK_FORCE_ADDRESS, &doorlock_force, 1U);
            }
            else if(menu->item_index == 7U) // lcd brightness
            {
                if(menu->increment > 9U) menu->increment = 1U;
                else if(menu->increment == 0U) menu->increment = 1U;
                lcd_brightness = (menu->increment * 100U);
                Int2StrSized(&menu->item_buff[7][0], lcd_brightness, 3U);
                __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, lcd_brightness);
                spi_buff[0] = (lcd_brightness >> 8U);
                spi_buff[1] = (lcd_brightness & 0xFFU);
                EEPROM_Save(EE_LCD_BRIGHTNESS, spi_buff, 2U);
            }
        }
        else if(menu->index == 4U)   // extended access permission
        {
            if(menu->increment > 9U) menu->increment = 0U;
            menu->item_buff[menu->item_index][menu->value_index] = menu->increment + '0';
            Str2Int(&menu->item_buff[menu->item_index][0], &val);
            permitted_add[menu->item_index][0] = ((val >> 8U) & 0xFFU);
            permitted_add[menu->item_index][1] = (val & 0xFFU);
            EEPROM_Save(EE_PERMITTED_ADDRESS_1 + (menu->item_index * 2), &permitted_add[menu->item_index][0], 2U);
        }
    }

}

static void DISPLAY_ProcessMenuTasterEvent(sMenuTypeDef* menu)
{
    /*************************************************************************/
    /**         M E N U          T A S T E R         E V E N T S            **/
    /*************************************************************************/
    if(menu->state == ACTIV)
    {
        /*************************************************************************/
        /**             S E T    M E N U    O N   L E F T  T A S T E R          **/
        /*************************************************************************/
        if(menu->event == LEFT_TASTER_PRESSED)
        {
            menu->event = TASTER_END;
            menu->state = CHANGED;
            
            if(menu->select == 0U)       
            {
                menu->select = 1U;
                menu->item_index = 0U;      // menu item index start from 1, index 0 is menu screen label
                menu->value_index = 0U;     // 
                menu->value_edit = 0U;
                menu->increment = 0U;
            }
            else if(menu->select == 1U)
            {
                if(menu->value_edit == 1U)
                {
                    ++menu->value_index; 
                    
                    if(menu->value_index >= menu->item_to_edit[menu->item_index])
                    {
                        ++menu->item_index;
                        menu->value_edit = 0U;
                        menu->value_index = 0U;
                    }               
                }
                else ++menu->item_index;

                if(menu->item_index >= MAX_ITEMS_IN_SCREEN)
                {
                    menu->select = 0U;
                    menu->item_index = 0U;
                }
            }
            else if(menu->select == 2U) 
            {
                ++menu->increment;
            }
        }
        /*************************************************************************/
        /**             S E T    M E N U   O N   R I G H T   T A S T E R        **/
        /*************************************************************************/
        else if(menu->event == RIGHT_TASTER_PRESSED)
        {
            menu->event = TASTER_END;
            menu->state = CHANGED;
            
            if(menu->select == 0U)
            {
                menu->state = PRINT;
                if(++menu->index >= MAX_SCREENS_IN_MENU) menu->index = 0U;
            }
            else if(menu->select == 1U)
            {
                menu->select = 2U;
                if(menu->item_to_edit[menu->item_index] != 0U)  menu->value_edit = 1U;
            }
            else if(menu->select == 2U) 
            {
                menu->select = 1U;
            }
        }
    }
}


static void DISPLAY_PrintMenuScreen(sMenuTypeDef* menu)
{
    uint32_t radio_address = 12345U;
    uint32_t radio_chanel = 12U;
    RTC_TimeTypeDef time_im;
    RTC_DateTypeDef date_im;
    
    font_size = MIDDLE_FONT;
    back_color = BLACK;
    pixel_color = WHITE;
    DISPLAY_Clear(BLACK);
    
    DISPLAY_PrintString(0, Line1_M, "--------------------------");
    DISPLAY_PrintString(0, Line10_M,"--------------------------");
    
    for(uint32_t t = 0; t < 8U; t++)
    {
        menu->item_label[t] = 0U;
        menu->item_to_edit[t] = 0U;
        menu->item_type[t] = LABEL_ITEM;
        
        for(uint32_t i = 0U; i < 8U; i++)
        {
            menu->item_buff[t][i] = 0U;
        }
    }
        
    if(menu->index == 0U) 
    {
        for(uint32_t t = 0; t < 8U; t++)
        {
            if (din_0_7 & (1U << t)) menu->item_label[t] = 1U;
        }
    }
    else if(menu->index == 1U) 
    {
        for(uint32_t t = 0; t < 8U; t++)
        {
            if (dout_0_7 & (1U << t)) menu->item_label[t] = 1U;
        }		
    }
    else if(menu->index == 2U) 
    {
        menu->item_type[0] = SELECT_ITEM;     // restart controller is screen value selectable value type
        menu->item_type[1] = SELECT_ITEM;     // preview image is screen value selectable value type
        menu->item_type[2] = SELECT_ITEM;     // image update is screen value selectable value type
        menu->item_type[3] = SELECT_ITEM;     // firmware update is screen value selectable value type
        menu->item_type[4] = SELECT_ITEM;     // scan onewire bus is screen value selectable value type with otput valu type: VALUE_LUX_RTH, VALUE_DS18B20 + unit numper        
        menu->item_type[5] = VALUE_IN_BUFFER; // set rs485 address is VALUE_BUFFER type with menu value stored in value buffer
        menu->item_type[6] = VALUE_IN_BUFFER; // set system id is VALUE_BUFFER type with menu value stored in value buffer        
        menu->item_type[7] = VALUE_IN_BUFFER; // set date & time is VALUE_BUFFER type with menu value stored in value buffer
        
        menu->item_to_edit[5] = 5U;
        menu->item_to_edit[6] = 5U;
        menu->item_to_edit[7] = 5U;
        
        Int2StrSized(&menu->item_buff[5][0], ((rs485_interface_address[0] << 8U) + rs485_interface_address[1]), 5U);   // 5 char rs485 interface address 00000~65535
        Int2StrSized(&menu->item_buff[6][0], ((system_id[0] << 8U) + system_id[1]), 5U);   // 5 char system id value 00000 ~ 65535
        
        HAL_RTC_GetTime(&hrtc, &time_im, RTC_FORMAT_BCD);
        HAL_RTC_GetDate(&hrtc, &date_im, RTC_FORMAT_BCD);
        
        menu->item_buff[7][0] = time_im.Hours;    // BCD hours value 0x01~0x23
        menu->item_buff[7][1] = time_im.Minutes;  // BCD  minute value 0x00~0x59
        menu->item_buff[7][2] = date_im.Date;     // BCD  date value 0x01~0x31
        menu->item_buff[7][3] = date_im.Month;    // BCD  month value 0x01~0x12
        menu->item_buff[7][4] = date_im.Year;     // BCD  year value 0x18~0x99
    }
    else if(menu->index == 3U) 
    {
        //menu->item_type[0] = LABEL_ITEM;      // disabled display	write of user data from mifare id card:(user name, sex, user type-geust, handmaid, service->->)
        //menu->item_type[1] = LABEL_ITEM;      // disabled tamper protection with unit backside pcb tacktile switch and light resistor to sense detachement from wall
        //menu->item_type[2] = LABEL_ITEM;      // 2,4GHz wireless radio modul disabled
        menu->item_type[3] = VALUE_IN_BUFFER;   // wireless radio modul address is VALUE_BUFFER type with menu value stored in value buffer
        menu->item_type[4] = VALUE_IN_BUFFER;   // wireless radio modul chanel is VALUE_BUFFER type with menu value stored in value buffer   
        menu->item_type[5] = VALUE_IN_BUFFER;   // buzzer volume is VALUE_BUFFER type with menu value stored in value buffer
        menu->item_type[6] = VALUE_IN_BUFFER;   // doorlock power is VALUE_BUFFER type with menu value stored in value buffer
        menu->item_type[7] = VALUE_IN_BUFFER;   // display backlight value 
      
        if(IsDISPLAY_UserCardInfoTextEnabled()) menu->item_label[0] = 1U;
        
        menu->item_to_edit[3] = 5U;
        menu->item_to_edit[4] = 2U;
        menu->item_to_edit[5] = 1U;
        menu->item_to_edit[6] = 1U;
        menu->item_to_edit[7] = 1U;
        
        Int2StrSized(&menu->item_buff[3][0], radio_address, 5U);      // 5 char radio modul address
        Int2StrSized(&menu->item_buff[4][0], radio_chanel, 2U);       // 5 char radio chanel
        Int2StrSized(&menu->item_buff[5][0], buzzer_volume, 2U);      // 3 char buzzer volume value 
        Int2StrSized(&menu->item_buff[6][0], doorlock_force, 2U);     // 3 char doorlock force value
        Int2StrSized(&menu->item_buff[7][0], lcd_brightness, 3U);     // 3 char doorlock force value
    }
    else if(menu->index == 4U) 
    {
        for(uint32_t t = 0U; t < 8U; t++)
        {
            menu->item_to_edit[t] = 5U;
            menu->item_type[t] = VALUE_IN_BUFFER;
            Int2StrSized(&menu->item_buff[t][0], ((permitted_add[t][0] << 8) + permitted_add[t][1]), 5);
        }
    }
}


static void DISPLAY_PrintMenuValue(sMenuTypeDef* menu)
{
    uint32_t c;
    uint8_t rtc[18];
    font_size = MIDDLE_FONT;
    back_color = BLACK;
    pixel_color = WHITE;
    
    DISPLAY_PrintString(0, Line11_M, taster[menu->select][0]);
    
    if(menu->select == 0U)
    {
        back_color = GRAY;
        pixel_color = BLACK;
    }
    else
    {
        back_color = BLACK;
        pixel_color = WHITE;    
    }
    
    DISPLAY_PrintString(0, Line0_M, screen[menu->index][0]);

    for(c = 0U; c < 8U; c ++)
    {
        if((menu->item_index == c) && (menu->select > 0U))
        {
            back_color = GRAY;
            pixel_color = BLACK;
        }
        else
        {
            back_color = BLACK;
            pixel_color = WHITE;    
        }
        
        if(menu->index == 4U)
        {
            DISPLAY_PrintString(0, Line2_M + (c * Line1_M), "PERMITTED ADDRESS  :");
            DISPLAY_PrintCharacter(210, Line2_M + (c * Line1_M), (c + '1'));
        }
        else 
        {
            DISPLAY_PrintString(0, Line2_M + (c * Line1_M), item[c][menu->index]);
        }

        if(menu->item_type[c] == VALUE_IN_BUFFER)
        {
            if((c == 7U) && (menu->index == 2U))    // date & time value
            {
                Hex2Str(&menu->item_buff[c][0], 1U, &rtc[0]);     // BCD hours value 0x01~0x23
                rtc[2] = ':';
                Hex2Str(&menu->item_buff[c][1], 1U, &rtc[3]);     // BCD  minute value 0x00~0x59
                rtc[5] = ' ';
                Hex2Str(&menu->item_buff[c][2], 1U, &rtc[6]);     // BCD  date value 0x01~0x31
                rtc[8] = '.';
                Hex2Str(&menu->item_buff[c][3], 1U, &rtc[9]);     // BCD  month value 0x01~0x12
                rtc[11] = '.';
                rtc[12] = '2';
                rtc[13] = '0';
                Hex2Str(&menu->item_buff[c][4], 1U,  &rtc[14]);   // BCD  year value 0x18~0x99
                rtc[16] = 0U;
//                DISPLAY_PrintString(120, Line2_M + (c * Line1_M), (const char*) rtc);
                DISPLAY_PrintString(120, Line9_M, (const char*) rtc);
                
                if((menu->item_index == 7U) && (menu->value_edit == 1U))
                {
                    back_color = LGRAY;
                    pixel_color = BLACK;
                    
                    if(menu->value_index == 0U)
                    {
                        DISPLAY_PrintCharacter(120U, Line9_M, rtc[0]);
                        DISPLAY_PrintCharacter(132U, Line9_M, rtc[1]); 
                    }
                    else if(menu->value_index == 1U)
                    {
                        DISPLAY_PrintCharacter(156U, Line9_M, rtc[3]);
                        DISPLAY_PrintCharacter(168U, Line9_M, rtc[4]); 
                    }
                    else if(menu->value_index == 2U)
                    {
                        DISPLAY_PrintCharacter(192U, Line9_M, rtc[6]);
                        DISPLAY_PrintCharacter(204U, Line9_M, rtc[7]); 
                    }
                    else if(menu->value_index == 3U)
                    {
                        DISPLAY_PrintCharacter(228U, Line9_M, rtc[9]);
                        DISPLAY_PrintCharacter(240U, Line9_M, rtc[10]); 
                    }
                    else if(menu->value_index == 4U)
                    {
                        DISPLAY_PrintCharacter(288U, Line9_M, rtc[14]);
                        DISPLAY_PrintCharacter(300U, Line9_M, rtc[15]); 
                    }
                } 
            }
            else                                    // 5 digit value
            {
                DISPLAY_PrintString(250, Line2_M + (c * Line1_M), (const char*) &menu->item_buff[c][0]); 
                
                if(menu->value_edit == 1U)
                {
                    back_color = LGRAY;
                    pixel_color = BLACK;
                    DISPLAY_PrintCharacter((250U + (menu->value_index * 12U)), (Line2_M + ((menu->item_index) * Line1_M)), (menu->item_buff[menu->item_index][menu->value_index]));
                }                
            }            
        }
        else if(menu->item_type[c] == LABEL_ITEM)
        {
            if((menu->item_index == c) && (menu->select > 0U))
            {
                if(menu->select == 2U) back_color = LGRAY;
                else back_color = GRAY;
                pixel_color = BLACK;
            }
            else
            {
                back_color = BLACK;
                pixel_color = WHITE;    
            }
            
            DISPLAY_PrintString(210, Line2_M + (c * Line1_M), label[menu->item_label[c]][0]);
            
            if((c == 4U) && (menu->index == 2U)) // display result of onewire bus scan
            {
                DISPLAY_PrintCharacter(300, Line2_M + (c * Line1_M), onewire_device_connected + '0');
            } 
        }
    }        
}




static void DISPLAY_InitMenu(sMenuTypeDef* menu)
{
    menu->index = 0U;
    menu->state = 0U;
    menu->event = 0U;
    menu->select = 0U;
    menu->increment = 0U;
    menu->item_index = 0U;
    menu->value_edit = 0U;
    menu->value_index = 0U;    
    
    for(uint32_t t = 0U; t < 8U; t++)
    {
        menu->item_type[t] = 0U;
        menu->item_label[t] = 0U;
        menu->item_to_edit[0] = 0U;
        
        for(uint32_t i = 0U; i < 8U; i++)
        {
            menu->item_buff[t][i] = 0U;
        }
    }
}   
/*************************************************************************/
/**     C H E C K    I S   L C D   A C T I V    A N D     R U N         **/
/*************************************************************************/
static void DISPLAY_CheckLcd(void)
{
    static uint32_t check_delay = 0U;
    
    if((HAL_GetTick() - check_delay) < 2000U) return;
    check_delay = HAL_GetTick();
    /*************************************************************************/
    /**		D I S P L A Y		P O W E R		M O D E 	C H E C K       **/
    /*************************************************************************/
    spi_buff[0] = ILI9341_RD_DISP_POWER_MODE;
    HAL_GPIO_WritePin(DISPLAY_DC_Port, DISPLAY_DC_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DISPLAY_CS_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi2, spi_buff, 1U, SPI_FLASH_TIMEOUT) != HAL_OK) Error_Handler(DISPLAY_FUNC, SPI_DRIVER);
    if (HAL_SPI_Receive(&hspi2, &spi_buff[1], 1U, SPI_FLASH_TIMEOUT) != HAL_OK) Error_Handler(DISPLAY_FUNC, SPI_DRIVER);
    HAL_GPIO_WritePin(DISPLAY_CS_Port, DISPLAY_CS_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(DISPLAY_DC_Port, DISPLAY_DC_Pin, GPIO_PIN_SET);
    
    if((spi_buff[1] & (1U << 7)) == 0U)
    {
#ifndef USE_DEBUGGER  
        LogEvent.log_event = DISPLAY_FAIL;
        LogEvent.log_group = DISPLAY_FUNC;
        LOGGER_Write();
        while(1) {continue;}    // stay here forewer
        
#elif defined   USE_DEBUGGER
        Error_Handler(DISPLAY_FUNC, 0U);
#endif
    }
}
/*************************************************************************/
/**     D I S P L A Y       D A T E   &   T I M E     I N F O           **/
/*************************************************************************/
static void DISPLAY_DateTime(void)
{
    RTC_TimeTypeDef time_d;
    RTC_DateTypeDef date_d;
    uint8_t disp_buff[24];
    static uint32_t temp_day = 0U;
    static uint32_t delay_timer = 0U;
    
    if((HAL_GetTick() - delay_timer) < 1000U) return;
    delay_timer = HAL_GetTick(); 
    HAL_RTC_GetTime(&hrtc, &time_d, RTC_FORMAT_BCD);
    HAL_RTC_GetDate(&hrtc, &date_d, RTC_FORMAT_BCD);
    /*************************************************************************/
    /**  B A C K U P   R T C   D A T E   D A I L Y   A N D   R E F R E S H  **/  
    /*************************************************************************/
    if(temp_day != date_d.Date)
    {
        temp_day = date_d.Date;
        DISPLAY_RefreshSet();
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, date_d.Date);
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, date_d.Month);
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, date_d.WeekDay);
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, date_d.Year);
    }	

    font_size = BIG_FONT;
    pixel_color = WHITE;
    back_color = BLACK;
    Hex2Str(&date_d.Date, 1U, disp_buff);
    disp_buff[2] = '.';
    Hex2Str(&date_d.Month, 1U, &disp_buff[3]);
    disp_buff[5] = '.';
    disp_buff[6] = '2';
    disp_buff[7] = '0';
    Hex2Str(&date_d.Year, 1U, &disp_buff[8]);
    disp_buff[10] = '.';	
    disp_buff[11] = ' ';
    Hex2Str(&time_d.Hours, 1U, &disp_buff[12]);
    disp_buff[14] = ':';
    Hex2Str(&time_d.Minutes, 1U, &disp_buff[15]);
    disp_buff[17] = ':';
    Hex2Str(&time_d.Seconds, 1U, &disp_buff[18]);
    disp_buff[20] = 0U;
    DISPLAY_PrintString(0U, Line8_B, (const char*)disp_buff);
}

/*************************************************************************/
/**     D I S P L A Y       T E M P E R A T U R E       I N F O         **/
/*************************************************************************/
static void DISPLAY_Temperature(void)
{
    uint8_t disp_buff[8];
    static uint32_t temperature_timer = 0U;
    
    if((HAL_GetTick() - temperature_timer) < 1234U) return;
    temperature_timer = HAL_GetTick(); 

    if(IsONEWIRE_DalasSensorConnected() || IsONEWIRE_ThermostatConnected())
    {
        font_size = MIDDLE_FONT;
        pixel_color = YELLOW;
        back_color = BLACK;
        
        disp_buff[0] = '+';
        Int2StrSized(&disp_buff[1], (temperature_measured / 10U), 2U);
        if(disp_buff[1] == '0')
        {
            disp_buff[0] = ' ';
            disp_buff[1] = '+';
        }
        disp_buff[3] = 0x5eU;
        disp_buff[4] = 'C';
        disp_buff[5] = ' ';
        disp_buff[6] = 0U;
        DISPLAY_PrintString(5U, 188U, (const char*)disp_buff);
    } 
}
/*************************************************************************/
/**     D I S P L A Y   U S E R     C A R D     I N F O     T E X T     **/
/*************************************************************************/
static void DISPLAY_CardInfoText(uint8_t user)
{
    font_size = BIG_FONT;
    back_color = BLACK;
    pixel_color = WHITE;
    
    if(user == CARD_USER_GROUP_GUEST)
    {
        if(rc522_rx_buff[0] & (1U << 7))        DISPLAY_PrintString(130U, Line6_B, " Mr.");     // guest card user is mister
        else                                    DISPLAY_PrintString(130U, Line6_B, "Mrs ");     // guest card user is madam         
    }
    else if(user == CARD_USER_GROUP_HANDMAID)   DISPLAY_PrintString(100U, Line6_B, "SOBARICA"); // handmaid card user
    else if(user == CARD_USER_GROUP_SERVICE)    DISPLAY_PrintString(100U, Line6_B, "SERVISER"); // service card user
    
    rc522_rx_buff[0] &= (~(1U << 7));
}
/*************************************************************************/
/**     D I S P L A Y   C A R D     U S E R     N A M E     T E X T     **/
/*************************************************************************/
static void DISPLAY_CardUserName(void)
{
	uint32_t t, c, n;
    uint8_t disp_buff[64];
    
	font_size = BIG_FONT;
    back_color = BLACK;
    pixel_color = WHITE;
    
	if((rc522_rx_buff[0] == 0U) && (rc522_tx_buff[0] == 0U)) return;
	
	ClearBuffer(disp_buff, sizeof(disp_buff));
	t = 0U;
	c = 0U;
	n = 0U;
	
	while((rc522_tx_buff[t] != 0U) && (t < 17U)) 
    {
        disp_buff[c] = rc522_tx_buff[t];
        ++c;
        ++t;
    }
	
	if(t > 16U)
	{
		disp_buff[16] = '.';
		disp_buff[17] = 0U;
	}
	else
	{
		disp_buff[c++] = ' ';
		n = t;
        t = 0U;

		while((rc522_rx_buff[t] != 0U) && (t < 17U)) 
        {
            disp_buff[c] = rc522_rx_buff[t];
            ++c;
            ++t;
        }

		if((n + t) > 18U)
		{
			c = (n + 1U);							
			disp_buff[c++] = rc522_rx_buff[0];
			disp_buff[c++] = '.';
            disp_buff[c] = 0U;
			n += 3U;
		}
		else n += t;	
		
		n = (n / 2U);
		n = (10U - n);
		n = (n * 10U);
	}
						
	DISPLAY_PrintString(n, Line7_B, (const char*)disp_buff);
}
/*************************************************************************/
/**     D I S P L A Y       C A R D     V A L I D       T I M E         **/
/*************************************************************************/
//static void DISPLAY_CardValidDate(void)
//{
//    uint8_t disp_buff[16];
//    
//    font_size = BIG_FONT;
//    back_color = BLACK;
//    pixel_color = WHITE;
//    
//    Hex2Str(&sCard.expiry_time[0], 1U, &disp_buff[0]);
//    disp_buff[2] = '.';
//    Hex2Str(&sCard.expiry_time[1], 1U, &disp_buff[3]);
//    disp_buff[5] = '.';
//    disp_buff[6] = '2';
//    disp_buff[7] = '0';
//    Hex2Str(&sCard.expiry_time[2], 1U, &disp_buff[8]);
//    disp_buff[10] = '.';
//    disp_buff[11] = 0U;
//    DISPLAY_PrintString(  0U, Line7_B, "VAZI DO:");
//    DISPLAY_PrintString(140U, Line7_B, (const char*)disp_buff);  
//}
/*************************************************************************/
/**     D I S P L A Y       R G B 5 6 5         I M A G E               **/
/*************************************************************************/
static void DISPLAY_Image(uint8_t selected)
{
	uint32_t image_size;
	uint32_t char_cnt = 0U;
    uint32_t flash_address;
    uint8_t dsp_buff[DISPLAY_BUFFER_SIZE];
	
	flash_address = ((selected - 1U) * 0x00030000U);
	
	if(selected == MINIBAR_IMAGE) 
	{
		image_size = IMAGE_MINIBAR_SIZE;
		DISPLAY_AddressSet(IMAGE_MINIBAR_POSITION);
	}
	else 
    {
        image_size = IMAGE_SIZE;
        DISPLAY_AddressSet(IMAGE_DISPLAY_POSITION);
    }
	
	DISPLAY_WriteRegister(ILI9341_MEMORY_WRITE);
	
    while(image_size)
	{
        if(image_size >= DISPLAY_BUFFER_SIZE) 
        {
            char_cnt = DISPLAY_BUFFER_SIZE;
            image_size -= DISPLAY_BUFFER_SIZE;
        }
		else 
        {
            char_cnt = image_size;
            image_size = 0U;
        }
        
		SPI_FLASH_ReadPage(flash_address, dsp_buff, char_cnt);
		HAL_GPIO_WritePin(DISPLAY_DC_Port, DISPLAY_DC_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(DISPLAY_CS_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET);
		if (HAL_SPI_Transmit(&hspi2, dsp_buff, char_cnt, LCD_DISPLAY_TIMEOUT) != HAL_OK) Error_Handler(DISPLAY_FUNC, SPI_DRIVER);
		HAL_GPIO_WritePin(DISPLAY_CS_Port, DISPLAY_CS_Pin, GPIO_PIN_SET);
		flash_address += char_cnt;
	}
}





/*************************************************************************/
/**      F I L L     D I S P L A Y    W I T H     C O L O R             **/
/*************************************************************************/
static void DISPLAY_Clear(uint16_t color)
{
	uint32_t i = 0U;
    uint8_t dsp_buff[640];

    DISPLAY_AddressSet(0U, 0U, LCD_W - 1U, LCD_H - 1U);
	DISPLAY_WriteRegister(ILI9341_MEMORY_WRITE);
    HAL_GPIO_WritePin(DISPLAY_DC_Port, DISPLAY_DC_Pin, GPIO_PIN_SET);
    
    do
    {
        dsp_buff[i] = (color >> 8U);
        ++i;
        dsp_buff[i] = (color & 0xffU);
        ++i;
    } 
    while (i < 640U);
    
    HAL_GPIO_WritePin(DISPLAY_CS_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET);
    i = LCD_H;
    
    do
    {
        if (HAL_SPI_Transmit(&hspi2, dsp_buff, 640U, SPI_FLASH_TIMEOUT) != HAL_OK) Error_Handler(EEPROM_FUNC, SPI_DRIVER);
        --i;
    }
    while(i != 0U);
    
    HAL_GPIO_WritePin(DISPLAY_CS_Port, DISPLAY_CS_Pin, GPIO_PIN_SET);
    DISPLAY_WriteRegister(ILI9341_NOP);
}


static void DISPLAY_AddressSet(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    DISPLAY_WriteRegister(ILI9341_COLUMN_ADDRESS_SET);
    DISPLAY_WriteInt(x1);
    DISPLAY_WriteInt(x2);
    DISPLAY_WriteRegister(ILI9341_PAGE_ADDRESS_SET);
    DISPLAY_WriteInt(y1);
    DISPLAY_WriteInt(y2);
}


static void DISPLAY_WriteRegister(uint8_t data)
{
    HAL_GPIO_WritePin(DISPLAY_DC_Port, DISPLAY_DC_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DISPLAY_CS_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi2, &data, 1U, SPI_FLASH_TIMEOUT) != HAL_OK) Error_Handler(EEPROM_FUNC, SPI_DRIVER);
    HAL_GPIO_WritePin(DISPLAY_CS_Port, DISPLAY_CS_Pin, GPIO_PIN_SET); 
}


static void DISPLAY_PrintCharacter(uint16_t x, uint16_t y, uint8_t num)
{
    uint32_t temp, pos, n, t;
    uint16_t colortemp = pixel_color;
    uint32_t inc_y;
    uint32_t font_address;
    uint32_t inc_x =  (4U + (font_size * 4U));
    if(font_size == SMALL_FONT) inc_y = 16U, font_address = EE_SMALL_FONT_ADDRESS;
    else if(font_size == MIDDLE_FONT) inc_y = 20U, font_address = EE_MIDDLE_FONT_ADDRESS;
    else if(font_size == BIG_FONT) inc_y = 26U, font_address = EE_BIG_FONT_ADDRESS;
    
    n = num - ' ';
    
    DISPLAY_AddressSet(x, y, (x + inc_x - 1U), (y  + inc_y - 1U)); 
    DISPLAY_WriteRegister(ILI9341_MEMORY_WRITE);
    
    for(pos = 0U; pos < inc_y; pos++)
    {
        if(font_size == SMALL_FONT) temp = SPI_FLASH_ReadByte((n * inc_y + pos) + font_address);  
        else temp = SPI_FLASH_ReadInt(((n * inc_y + pos)* 2) + font_address);
        
        for(t = 0U; t < inc_x; t++)
        {
            pixel_color = back_color;
            
            if(font_size == SMALL_FONT) 
            {
                if (temp & (1U << t))  pixel_color = colortemp;
            }
            else if(temp & (1U << 15U)) pixel_color = colortemp;
            
            DISPLAY_WriteInt(pixel_color);    
            if(font_size != SMALL_FONT) temp <<= 1U;                 
        }
    }		
   
    pixel_color = colortemp;	
}



static void DISPLAY_PrintNumber(uint16_t x, uint16_t y, uint32_t num, uint8_t len)
{
    uint32_t t, temp, enshow = 0U;
    uint32_t inc_x =  (4U + (font_size * 4U));
    
	for(t = 0U; t < len; t++)
	{
		temp =( num / BaseToPower(10U, len - t - 1U)) %10U;
        
		if((enshow == 0U) && (t < (len - 1)))
		{
			if(temp == 0U)
			{
				DISPLAY_PrintCharacter(( x + inc_x * t), y, ' ');
				continue;
			}
			else 
			{
                enshow = 1U;
            }
		}
        
        DISPLAY_PrintCharacter((x + inc_x * t), y, (temp + '0'));
	}
}


static void DISPLAY_PrintString(uint16_t x, uint16_t y, const char *p)
{            
    uint32_t inc_y;
    uint32_t inc_x =  (4U + (font_size * 4U));
    if(font_size == SMALL_FONT) inc_y = 16U;
    else if(font_size == MIDDLE_FONT) inc_y = 20U;
    else if(font_size == BIG_FONT) inc_y = 26U;
    
    while(*p != '\0')
	{        
		
        if(x > (LCD_W - inc_x))
        {
            x = 0U;
            y += inc_y;
        }

        if(y > (LCD_H - inc_y)) 
        {
            x = 0U;
            y = 0U;
        }
        
        DISPLAY_PrintCharacter(x, y, *p);
        x += inc_x;
        ++p;
    }
}


static void DISPLAY_WriteByte(uint8_t data)
{
	HAL_GPIO_WritePin(DISPLAY_DC_Port, DISPLAY_DC_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(DISPLAY_CS_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi2, &data, 1U, SPI_FLASH_TIMEOUT) != HAL_OK) Error_Handler(EEPROM_FUNC, SPI_DRIVER);
    HAL_GPIO_WritePin(DISPLAY_CS_Port, DISPLAY_CS_Pin, GPIO_PIN_SET);
}


static void DISPLAY_WriteInt(uint16_t data)
{
	HAL_GPIO_WritePin(DISPLAY_DC_Port, DISPLAY_DC_Pin, GPIO_PIN_SET);
    spi_buff[0] = data >> 8U;
    spi_buff[1] = data & 0xFFU;
    HAL_GPIO_WritePin(DISPLAY_CS_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET);
    if (HAL_SPI_Transmit(&hspi2, spi_buff, 2U, SPI_FLASH_TIMEOUT) != HAL_OK) Error_Handler(EEPROM_FUNC, SPI_DRIVER);
    HAL_GPIO_WritePin(DISPLAY_CS_Port, DISPLAY_CS_Pin, GPIO_PIN_SET);
}


/******************************   END OF FILE  **********************************/

