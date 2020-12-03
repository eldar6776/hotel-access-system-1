/**
 ******************************************************************************
 * File Name          : display.c
 * Date               : 10.3.2018
 * Description        : GUI Display Module
 ******************************************************************************
 *
 *
 ******************************************************************************
 */
 
 
/* Include  ------------------------------------------------------------------*/
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "LCDConf.h"
#include "DIALOG.h"
#include "Resource.h"
#include "main.h"
#include "common.h"
#include "display.h"
#include "one_wire.h"
#include "thermostat.h"
#include "stm32746g.h"
#include "stm32746g_ts.h"
#include "stm32746g_qspi.h"
#include "stm32746g_sdram.h"
#include "stm32746g_eeprom.h"


/* Private Define ------------------------------------------------------------*/
#define GUI_REFRESH_TIME				            100U    // refresh gui 10 time in second
#define DATE_TIME_REFRESH_TIME			            10000U  // refresh date & time info every 10 sec. 
#define DISPLAY_SCREENSAVER_TIME                    34567U  // 30 sec display low brigntnes timeout
#define SETTINGS_MENU_ENABLE_TIME                   1234U  // press upper left corrner for 10 sec. and hope for setings menu
#define BUTTON_RESPONSE_TIME                        2000U   // button response delay till all onewire device update state

#if defined DEMO_MODE
#undef SETTINGS_MENU_ENABLE_TIME                            // clear previous definition
#define SETTINGS_MENU_ENABLE_TIME                   6543U   // faster service menu activation in demo mode
#undef BUTTON_RESPONSE_TIME                                 // clear previous definition
#define BUTTON_RESPONSE_TIME                        500U    // faster button response in demo mode
#define DEMO_MENU_DELAY                             2000U   // demo menu display image delay time
#endif

//eng   display image 1- 5
//ger   display image 11-15
//fra   display image 21-25
//arab  display image 31-35
//china display image 41-45
//jap   display image 51-55
//ita   display image 61-65
//tur   display image 71-75
//slov  display image 81-85

#define DARK_BLUE               GUI_MAKE_COLOR(0x613600)
#define LIGHT_BLUE              GUI_MAKE_COLOR(0xaa7d67)
#define BLUE                    GUI_MAKE_COLOR(0x855a41)
#define LEMON                   GUI_MAKE_COLOR(0x00d6d3)

#define BTN_INC_X0              220
#define BTN_INC_Y0              90
#define BTN_INC_X1              334
#define BTN_INC_Y1              246

#define BTN_DEC_X0              3
#define BTN_DEC_Y0              87
#define BTN_DEC_X1              106
#define BTN_DEC_Y1              247

#define BTN_DND_X0              355
#define BTN_DND_Y0              5
#define BTN_DND_X1              475
#define BTN_DND_Y1              55

#define BTN_CMD_X0              355
#define BTN_CMD_Y0              75
#define BTN_CMD_X1              475 
#define BTN_CMD_Y1              125

#define BTN_SOS_X0              355
#define BTN_SOS_Y0              145
#define BTN_SOS_X1              475
#define BTN_SOS_Y1              195

#define BTN_DOOR_X0             25
#define BTN_DOOR_Y0             215
#define BTN_DOOR_X1             180
#define BTN_DOOR_Y1             265

#define BTN_OK_X0               340
#define BTN_OK_Y0               215
#define BTN_OK_X1               473
#define BTN_OK_Y1               270

#define BTN_SETTINGS_X0         0
#define BTN_SETTINGS_Y0         0
#define BTN_SETTINGS_X1         100
#define BTN_SETTINGS_Y1         100

#define SP_TEMP_DEC_POS         125
#define SP_TEMP_UNIT_POS        165
#define SP_TEMP_V_POS           145

#define CLOCK_HOUR_DEC_POS      0
#define CLOCK_HOUR_UNIT_POS     65
#define CLOCK_COLON_1_POS       140
#define CLOCK_MINUTE_DEC_POS    165
#define CLOCK_MINUTE_UNIT_POS   235
#define CLOCK_COLON_2_POS       305
#define CLOCK_SECOND_DEC_POS    330
#define CLOCK_SECOND_UNIT_POS   400
#define CLOCK_V_POS             100
#define CLOCK_DIGIT_H_SIZE      80 
#define CLOCK_DIGIT_V_SIZE      80
#define CLOCK_DIGIT_H_SCALING   600
#define CLOCK_DIGIT_V_SCALING   600


/* Private Type --------------------------------------------------------------*/
BUTTON_Handle hBUTTON_Dnd;
BUTTON_Handle hBUTTON_SosReset;
BUTTON_Handle hBUTTON_Maid;
BUTTON_Handle hBUTTON_Increase;
BUTTON_Handle hBUTTON_Decrease;
BUTTON_Handle hBUTTON_Ok;
BUTTON_Handle hBUTTON_DoorOpen;
BUTTON_Handle hBUTTON_Next;
BUTTON_Handle hBUTTON_Forecast;

SPINBOX_Handle  hSPINBOX_FancoillNtcOffset;                         //  fancoil measured temperature value manual offset
SPINBOX_Handle  hSPINBOX_AmbientNtcOffset;                          //  ambient measured temperature value manual offset
SPINBOX_Handle  hSPINBOX_ErrorDurationTime;                         //  limit values violation time to trigger error event and time for auto restart
SPINBOX_Handle  hSPINBOX_CoolingMaxTemperature;                     //  coolant maximum temperature limit selectable as setpoint difference or absoulte value
SPINBOX_Handle  hSPINBOX_HeatingMinTemperature;                     //  heating fluid minimum temperature limit selectable as setpoint difference or absoulte value
SPINBOX_Handle  hSPINBOX_OneWireInterfaceAddress;                   //  one wire bus interface address
SPINBOX_Handle  hSPINBOX_Rs485InterfaceAddress;                     //  rs485 bus interface address
SPINBOX_Handle  hSPINBOX_FancoilFanSpeedTreshold;                   //  fancoil fan speed treshold
SPINBOX_Handle  hSPINBOX_FancoilFanLowSpeedBand;                    //  fancoil fan low speed band
SPINBOX_Handle  hSPINBOX_FancoilFanMiddleSpeedBand;                 //  fancoil fan middle speed band
SPINBOX_Handle  hSPINBOX_FreezingProtectionTriggerTemperature;      //  freezing protection function trigger temperature either ambient or fancoil
SPINBOX_Handle  hSPINBOX_FreezingProtectionSetpointTemperature;     //  freezing protection function temperature setpoint to reach
SPINBOX_Handle  hSPINBOX_ThermostatMaxSetpointTemperature;          //  set thermostat user maximum setpoint value
SPINBOX_Handle  hSPINBOX_ThermostatMinSetpointTemperature;          //  set thermostat user minimum setpoint value
SPINBOX_Handle  hSPINBOX_FancoilNightQuietModeStart;                //  fan quiet mode start from set hour till 7,00 AM
SPINBOX_Handle  hSPINBOX_FreezingProtectionTemperature;             //  freezing protection trigger temperature activ till temperature rised by 5*C

CHECKBOX_Handle hCHECKBOX_AmbientNtcErrorReport;                    //  report activ ambient temperature NTC error to controller
CHECKBOX_Handle hCHECKBOX_FancoilNtcErrorReport;                    //  report activ fancoil temperature NTC error to controller
CHECKBOX_Handle hCHECKBOX_FancoilCoolingErrorReport;                //  report activ cooling fluid temperature error to controller
CHECKBOX_Handle hCHECKBOX_FancoilHeatingErrorReport;                //  report activ heating fluid temperature error to controller
CHECKBOX_Handle hCHECKBOX_AmbientNtcErrorShutdown;                  //  disable thermostat on activ ambient temperature NTC error
CHECKBOX_Handle hCHECKBOX_FancoilNtcErrorShutdown;                  //  disable thermostat on activ fancoil temperature NTC error
CHECKBOX_Handle hCHECKBOX_FancoilCoolingErrorShutdown;              //  disable thermostat on activ cooling fluid temperature error
CHECKBOX_Handle hCHECKBOX_FancoilHeatingErrorShutdown;              //  disable thermostat on activ heating fluid temperature error
CHECKBOX_Handle hCHECKBOX_FancoilErrorAutorestart;                  //  restart previous thermostat operation disabled by error after error duration time
CHECKBOX_Handle hCHECKBOX_FancoilValveShutdownOnSetpoint;           //  shutdown cooling and heating valve when setpoint is reached
CHECKBOX_Handle hCHECKBOX_FancoilFilterDirtyMonitor;                //  enable fancoil air filter monitoring function 
CHECKBOX_Handle hCHECKBOX_FancoilFilterDirtyReport;                 //  enable fancoil air filter report when alarm trigger
CHECKBOX_Handle hCHECKBOX_FancoilFilterDirtyShutdown;               //  enable fancoil air filter to disable thermostat on alarm trigger
CHECKBOX_Handle hCHECKBOX_FreezingProtection;                       //  enable freezing protection function
CHECKBOX_Handle hCHECKBOX_FreezingProtectionReport;                 //  report freezing protection function trigger
CHECKBOX_Handle hCHECKBOX_FancoilRpmSensor;                         //  fancoil RPM hall sensor in use
CHECKBOX_Handle hCHECKBOX_FancoilRpmSensorErrorReport;              //  fancoil RPM hall sensor error report
CHECKBOX_Handle hCHECKBOX_FancoilRpmSensorErrorShutdown;            //  disable thermostat on activ  fancoil RPM hall sensor error
CHECKBOX_Handle hCHECKBOX_FancoilNightQuietMode;                    //  limit fancoil fan speed to low value from 10PM to 7AM 

ICONVIEW_Handle hICONVIEW_DemoMenu;

EDIT_Handle hEDIT_TimeSet;                                          //  enter new RTC time
EDIT_Handle hEDIT_DateSet;                                          //  enter new RTC date

RADIO_Handle hRADIO_TemperatureLimitType;                           //  select cooling / heating fluid limit temperature as absolute value or setpoint diferrence
RADIO_Handle hRADIO_ThermostatModeSelect;                           //  select thermostat operating mode as off-cooling-heating-network
RADIO_Handle hRADIO_FancoilFanControlType;                          //  select fancoil control type as on/off-three speed-triac 


/* Private Variable ----------------------------------------------------------*/
__IO uint32_t display_flags;
__IO uint8_t display_image_id, display_image_time;

static uint8_t btn_ok_state;
static uint8_t btn_opendoor_state;
static uint8_t btn_dnd_state, btn_dnd_old_state;
static uint8_t btn_sos_state, btn_sos_old_state;
static uint8_t btn_maid_state, btn_maid_old_state;
static uint8_t btn_increase_state, btn_increase_old_state;
static uint8_t btn_decrease_state, btn_decrease_old_state;
static uint8_t btn_settings_menu_state, btn_settings_menu_old_state;
static uint32_t display_screensaver_timer = 0U;


/* Private Macro -------------------------------------------------------------*/
/* Private Function Prototype ------------------------------------------------*/
static void DISPLAY_DateTime(void);
static void DISPLAY_FullBrightnes(void);
static void DISPLAY_CreateDemoMenu(void);
static void DISPLAY_DeleteDemoMenu(void);
static void DISPLAY_TemperatureSetPoint(void);
static void DISPLAY_Temperature(uint16_t value);
static void DISPLAY_CreateSettings1Screen(void);
static void DISPLAY_CreateSettings2Screen(void);
static void DISPLAY_CreateSettings3Screen(void);
static void DISPLAY_DeleteSettings1Screen(void);
static void DISPLAY_DeleteSettings2Screen(void);
static void DISPLAY_DeleteSettings3Screen(void);

static void PID_Hook(GUI_PID_STATE * pState);
static void SaveSettings(void);


/* Program Code  -------------------------------------------------------------*/
void DISPLAY_Init(void)
{
    GUI_Init();
    GUI_PID_SetHook(PID_Hook);
    WM_MULTIBUF_Enable(1);
    LCD_SetVisEx(0, 1);
    LCD_SetVisEx(1, 1);
    GUI_SelectLayer(0);
    GUI_Clear();
    GUI_DrawBitmap(&bm_display_00, 0, 0);
    GUI_SelectLayer(1);
    GUI_SetBkColor(GUI_TRANSPARENT); 
    GUI_Clear();
    BUTTON_DndActivReset();
    GUI_DrawBitmap(&bm_btn_dnd_0, BTN_DND_X0, BTN_DND_Y0);
    BUTTON_SosActivReset();
    GUI_DrawBitmap(&bm_btn_rst_sos_0, BTN_SOS_X0, BTN_SOS_Y0);
    BUTTON_CallMaidActivReset();
    GUI_DrawBitmap(&bm_btn_maid_0, BTN_CMD_X0, BTN_CMD_Y0);
    GUI_Exec();
    DISPLAY_TemperatureSetPoint();
    DISPLAY_DateTime();
}


void DISPLAY_Service(void)
{
    static enum
    {
        DISPLAY_THERMOSTAT  = ((uint8_t)0x00U),
        DISPLAY_FORECAST    = ((uint8_t)0x01U),
        DISPLAY_MESSAGE     = ((uint8_t)0x02U),
        DISPLAY_ERROR       = ((uint8_t)0x03U),
        DISPLAY_DEMO_MENU   = ((uint8_t)0x04U),
        DISPLAY_SETTINGS_1  = ((uint8_t)0x05U),
        DISPLAY_SETTINGS_2  = ((uint8_t)0x06U),
        DISPLAY_SETTINGS_3  = ((uint8_t)0x07U)        
        
    }DisplayServiceState = DISPLAY_THERMOSTAT;
    
    static uint32_t display_timer = 0U;
	static uint32_t last_image_id = 0U;
    static uint32_t display_rtc_timer = 0U;
    static uint32_t display_image_timer = 0U;
    static uint32_t button_enable_timer = 0U;
    static uint32_t settings_menu_timer = 0U;
    static uint32_t demo_menu_timer = 0U;
	/** ==========================================================================*/
	/**    D R A W     D I S P L A Y	G U I	O N	   T I M E R    E V E N T     */
	/** ==========================================================================*/
	if((HAL_GetTick() - display_timer) >= GUI_REFRESH_TIME)
	{
		display_timer = HAL_GetTick();
		GUI_Exec();
	}
	else return;
    /** ==========================================================================*/
	/**    C H E C K    R C    D O O R B E L L     T A S T E R     S T A T E      */
	/** ==========================================================================*/
    if(IsONEWIRE_DoorBellOn() && !IsDISPLAY_DoorBellActiv())
    {
        DISPLAY_DoorBellSet();
        DISPLAY_UpdateSet();
    }
    else if(!IsONEWIRE_DoorBellOn() && IsDISPLAY_DoorBellActiv()) DISPLAY_DoorBellReset();
    /** ==========================================================================*/
	/**  U N U S E D    D I S P L A Y      B A C K L I G H T      D I M M E D     */
	/** ==========================================================================*/
    if(!IsDISPLAY_ScreensaverActive())   
    {   
        if((HAL_GetTick() - display_screensaver_timer) >= DISPLAY_SCREENSAVER_TIME)
        {
            DISPLAY_ScreensaverSet();
            display_screensaver_timer = HAL_GetTick();
            __HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_1, DISPLAY_BRIGHTNESS_LOW);  
        }
    }
    /** ==========================================================================*/
	/**         N E W       D I S P L A Y       I M A G E       R E Q U E S T     */
	/** ==========================================================================*/
    if(IsDISPLAY_UpdateActiv() && (DisplayServiceState < DISPLAY_SETTINGS_1))
    {
        DISPLAY_UpdateReset();
        DISPLAY_FullBrightnes();
        
        if(IsDISPLAY_DoorBellActiv())
        {
            display_image_id = 1U;
            display_image_time = 1U;
            buzzer_signal_id = 2U;
            buzzer_signal_time = 2U;
        }
        else if(IsDISPLAY_ConditionErrorActiv())
        {
            DISPLAY_ConditionErrorReset();
            display_image_id = 101U;
            display_image_time = 0U;
            buzzer_signal_time = 0U;
            buzzer_signal_id = 0U;
        }
        else if(IsDISPLAY_UserInterfaceActiv())
        {
            DISPLAY_UserInterfaceReset();
            display_image_id = 0U;
            display_image_time = 0U;
            buzzer_signal_time = 0U;
            buzzer_signal_id = 0U;
        }
        /************************************/
        /*    START DISPLAY IMAGE TIMER     */
        /************************************/
        if(display_image_time != 0U)            //  displayed image time is in  x 30 sec. increments
        {
            display_image_timer = HAL_GetTick();
        }
        else display_image_timer = 0U;          //  image with zero time is displayed infinite till next update
        /************************************/
        /*  DISPLAY NEW IMAGE AND SET STATE */
        /************************************/
        if((display_image_id > 89U))           //  error message with id higher than 89 
        {
            last_image_id = display_image_id;
            GUI_SelectLayer(0);
            GUI_Clear();
            GUI_MULTIBUF_BeginEx(0);
            GUI_DrawBitmap(&bm_display_error_01, 0, 0);
            GUI_MULTIBUF_EndEx(0);
            GUI_SelectLayer(1);
            GUI_SetBkColor(GUI_TRANSPARENT); 
            GUI_Clear();
            if(DisplayServiceState != DISPLAY_DEMO_MENU) DisplayServiceState = DISPLAY_ERROR;
            BUZZER_SignalOff();
        }
        else if(display_image_id == 0U)         //  thermostat user interface id is 0
        {
            last_image_id = 0U;
            GUI_SelectLayer(0);
            GUI_Clear();
            GUI_MULTIBUF_BeginEx(0);
            GUI_DrawBitmap(&bm_display_00, 0, 0);
            GUI_MULTIBUF_EndEx(0);
            GUI_SelectLayer(1);
            GUI_SetBkColor(GUI_TRANSPARENT); 
            GUI_Clear();
            if(DisplayServiceState != DISPLAY_DEMO_MENU) DisplayServiceState = DISPLAY_THERMOSTAT;
            BUTTON_StateChangedSet();
            DISPLAY_TemperatureSetPoint();
            DISPLAY_DateTime();
            BUZZER_SignalOff();
        }
        else                                    //  user information display image have id between 0 and 100
        {                                       //  sorted as 8 info image on 5 different language
            GUI_SelectLayer(0);
            GUI_Clear();
            GUI_MULTIBUF_BeginEx(0);
            if     (display_image_id == 1U) GUI_DrawBitmap(&bm_display_01, 0, 0);
            else if(display_image_id == 2U) GUI_DrawBitmap(&bm_display_02, 0, 0);
            else if(display_image_id == 3U) GUI_DrawBitmap(&bm_display_03, 0, 0);
            else if(display_image_id == 4U) GUI_DrawBitmap(&bm_display_04, 0, 0);
            else if(display_image_id == 5U) GUI_DrawBitmap(&bm_display_05, 0, 0);
            
            else if(display_image_id == 11U) GUI_DrawBitmap(&bm_display_11, 0, 0);
            else if(display_image_id == 12U) GUI_DrawBitmap(&bm_display_12, 0, 0);
            else if(display_image_id == 13U) GUI_DrawBitmap(&bm_display_13, 0, 0);
            else if(display_image_id == 14U) GUI_DrawBitmap(&bm_display_14, 0, 0);
            else if(display_image_id == 15U) GUI_DrawBitmap(&bm_display_15, 0, 0);
            
            else if(display_image_id == 21U) GUI_DrawBitmap(&bm_display_21, 0, 0);
            else if(display_image_id == 22U) GUI_DrawBitmap(&bm_display_22, 0, 0);
            else if(display_image_id == 23U) GUI_DrawBitmap(&bm_display_23, 0, 0);
            else if(display_image_id == 24U) GUI_DrawBitmap(&bm_display_24, 0, 0);
            else if(display_image_id == 25U) GUI_DrawBitmap(&bm_display_25, 0, 0);
            
            else if(display_image_id == 31U) GUI_DrawBitmap(&bm_display_31, 0, 0);
            else if(display_image_id == 32U) GUI_DrawBitmap(&bm_display_32, 0, 0);
            else if(display_image_id == 33U) GUI_DrawBitmap(&bm_display_33, 0, 0);
            else if(display_image_id == 34U) GUI_DrawBitmap(&bm_display_34, 0, 0);
            else if(display_image_id == 35U) GUI_DrawBitmap(&bm_display_35, 0, 0);
            
            else if(display_image_id == 41U) GUI_DrawBitmap(&bm_display_41, 0, 0);
            else if(display_image_id == 42U) GUI_DrawBitmap(&bm_display_42, 0, 0);
            else if(display_image_id == 43U) GUI_DrawBitmap(&bm_display_43, 0, 0);
            else if(display_image_id == 44U) GUI_DrawBitmap(&bm_display_44, 0, 0);
            else if(display_image_id == 45U) GUI_DrawBitmap(&bm_display_45, 0, 0);
            
            else if(display_image_id == 51U) GUI_DrawBitmap(&bm_display_51, 0, 0);
            else if(display_image_id == 52U) GUI_DrawBitmap(&bm_display_52, 0, 0);
            else if(display_image_id == 53U) GUI_DrawBitmap(&bm_display_53, 0, 0);
            else if(display_image_id == 54U) GUI_DrawBitmap(&bm_display_54, 0, 0);
            else if(display_image_id == 55U) GUI_DrawBitmap(&bm_display_55, 0, 0);
            
            else if(display_image_id == 61U) GUI_DrawBitmap(&bm_display_61, 0, 0);
            else if(display_image_id == 62U) GUI_DrawBitmap(&bm_display_62, 0, 0);
            else if(display_image_id == 63U) GUI_DrawBitmap(&bm_display_63, 0, 0);
            else if(display_image_id == 64U) GUI_DrawBitmap(&bm_display_64, 0, 0);
            else if(display_image_id == 65U) GUI_DrawBitmap(&bm_display_65, 0, 0);
           
            else if(display_image_id == 71U) GUI_DrawBitmap(&bm_display_71, 0, 0);
            else if(display_image_id == 72U) GUI_DrawBitmap(&bm_display_72, 0, 0);
            else if(display_image_id == 73U) GUI_DrawBitmap(&bm_display_73, 0, 0);
            else if(display_image_id == 74U) GUI_DrawBitmap(&bm_display_74, 0, 0);
            else if(display_image_id == 75U) GUI_DrawBitmap(&bm_display_75, 0, 0);
           
            else if(display_image_id == 81U) GUI_DrawBitmap(&bm_display_81, 0, 0);
            else if(display_image_id == 82U) GUI_DrawBitmap(&bm_display_82, 0, 0);
            else if(display_image_id == 83U) GUI_DrawBitmap(&bm_display_83, 0, 0);
            else if(display_image_id == 84U) GUI_DrawBitmap(&bm_display_84, 0, 0);
            else if(display_image_id == 85U) GUI_DrawBitmap(&bm_display_85, 0, 0);
            
            GUI_MULTIBUF_EndEx(0);
            GUI_SelectLayer(1);
            GUI_SetBkColor(GUI_TRANSPARENT); 
            GUI_Clear();
            GUI_MULTIBUF_BeginEx(1);
            
            if((display_image_id == 1U) || (display_image_id == 11U) || (display_image_id == 21U) 
            ||(display_image_id == 31U) || (display_image_id == 41U) || (display_image_id == 51U) 
            ||(display_image_id == 61U) || (display_image_id == 71U) || (display_image_id == 81U))
            {
               GUI_DrawBitmap(&bm_btn_door_open, BTN_DOOR_X0, BTN_DOOR_Y0); 
            }
            
            GUI_DrawBitmap(&bm_btn_ok, BTN_OK_X0, BTN_OK_Y0);
            GUI_MULTIBUF_EndEx(1);
            button_enable_timer = HAL_GetTick();
            if(DisplayServiceState != DISPLAY_DEMO_MENU) DisplayServiceState = DISPLAY_MESSAGE;
            BUZZER_SignalOn();
        }
    }
	else if(DisplayServiceState == DISPLAY_THERMOSTAT) // display thermostat user interface for room status and temperature setup
	{
        /** ==========================================================================*/
        /**     C H E C K       U S E R     B U T T O N S    N E W       E V E N T    */
        /** ==========================================================================*/
        if((HAL_GetTick() - button_enable_timer) >= BUTTON_RESPONSE_TIME)
        {
            /************************************/
            /*      DND BUTTON STATE CHANGED    */
            /************************************/
            if(btn_dnd_state && !btn_dnd_old_state)         
            {
                btn_dnd_old_state = 1U;
                
                if(!IsBUTTON_DndActiv())
                {
                    BUTTON_DndActivSet();
                    BUTTON_CallMaidActivReset();
                }
                else BUTTON_DndActivReset();
                
                BUTTON_StateChangedSet();
                ONEWIRE_SendNewButtonStateSet();
            }
            else if(!btn_dnd_state && btn_dnd_old_state)  btn_dnd_old_state = 0U;
            /************************************/
            /*  CALLMAID BUTTON STATE CHANGED   */
            /************************************/        
            if(btn_maid_state && !btn_maid_old_state)
            {
                btn_maid_old_state = 1U;
                
                if(!IsBUTTON_CallMaidActiv())
                {
                    BUTTON_CallMaidActivSet();
                    BUTTON_DndActivReset();
                }
                else BUTTON_CallMaidActivReset();
                
                BUTTON_StateChangedSet();
                ONEWIRE_SendNewButtonStateSet();
            }
            else if(!btn_maid_state && btn_maid_old_state) btn_maid_old_state = 0U;
            /************************************/
            /*      SOS BUTTON STATE CHANGED    */
            /************************************/ 
            if(IsBUTTON_SosResetActiv() && btn_sos_state && !btn_sos_old_state)
            {
                btn_sos_old_state = 1U;
                BUTTON_SosActivReset();
                BUTTON_StateChangedSet();
                ONEWIRE_SendNewButtonStateSet();
            }
            else if(!btn_sos_state && btn_sos_old_state) btn_sos_old_state = 0U;
            
#if defined DEMO_MODE
            else if(btn_sos_state && !btn_sos_old_state)
            {
                btn_sos_old_state = 1U;
                demo_menu_timer = HAL_GetTick();
            }
            else if(btn_sos_state && btn_sos_old_state)
            {
                if((HAL_GetTick() - demo_menu_timer) >= DEMO_MENU_DELAY)
                {
                    DISPLAY_CreateDemoMenu();
                    demo_menu_timer = HAL_GetTick();
                    DisplayServiceState = DISPLAY_DEMO_MENU;
                }                 
            }
#endif            
        }
        /************************************/
        /*      SETPOINT  VALUE  INCREASED  */
        /************************************/ 
        if(btn_increase_state && !btn_increase_old_state)
		{
			btn_increase_old_state = 1U;
            
            if((temperature_setpoint & 0x3FU) < thermostat_max_setpoint_temperature) 
            {
                ++temperature_setpoint;
                DISPLAY_SetpointUpdateSet();
                ONEWIRE_SendThermostatVariableSet();
                buzzer_signal_id = BUZZER_CLICK;
                BUZZER_SignalOn();
            }
		}
		else if(!btn_increase_state && btn_increase_old_state) btn_increase_old_state = 0U;
        /************************************/
        /*      SETPOINT  VALUE  DECREASED  */
        /************************************/ 
        if(btn_decrease_state && !btn_decrease_old_state)
		{
			btn_decrease_old_state = 1U;
            
            if((temperature_setpoint & 0x3FU) >= thermostat_min_setpoint_temperature) 
            {
                --temperature_setpoint;
                DISPLAY_SetpointUpdateSet();
                ONEWIRE_SendThermostatVariableSet();
                buzzer_signal_id = BUZZER_CLICK;
                BUZZER_SignalOn();
            }
		}
		else if(!btn_decrease_state && btn_decrease_old_state) btn_decrease_old_state = 0U;
        /** ==========================================================================*/
        /**  C H E C K   H I D D E N     S E T T I N G S     M E N U     B U T T O N  */
        /** ==========================================================================*/
        if(btn_settings_menu_state && !btn_settings_menu_old_state)
        {
            btn_settings_menu_old_state = 1U;
            settings_menu_timer = HAL_GetTick();
        }
        else if(btn_settings_menu_state && btn_settings_menu_old_state)
        {
            if((HAL_GetTick() - settings_menu_timer) >= SETTINGS_MENU_ENABLE_TIME)
            {
                btn_settings_menu_state = 0U;
                btn_settings_menu_old_state = 0U;
                DISPLAY_CreateSettings1Screen();
                DISPLAY_FancoilNtcSensorStateReset();
                DISPLAY_AmbientNtcSensorStateReset();
                DisplayServiceState = DISPLAY_SETTINGS_1;
                buzzer_signal_id = BUZZER_CLICK;
                BUZZER_SignalOn();
            }
        }
        else if(!btn_settings_menu_state && btn_settings_menu_old_state) btn_settings_menu_old_state = 0U;
        /** ==========================================================================*/
        /**          R E W R I T E       D A T E     &       T I M E                  */
        /** ==========================================================================*/
        if((HAL_GetTick() - display_rtc_timer) >= DATE_TIME_REFRESH_TIME) // date & time info update on minute 
        {
            display_rtc_timer = HAL_GetTick();
            DISPLAY_DateTime();
        }
        /** ==========================================================================*/
        /**   R E W R I T E   A N D   S A V E   N E W   S E T P O I N T   V A L U E   */
        /** ==========================================================================*/
        if(IsDISPLAY_SetpointUpdateActiv())    // setpoint temperature changed
        {
            DISPLAY_SetpointUpdateReset();
            DISPLAY_TemperatureSetPoint();
            BSP_EEPROM_WriteBuffer(&temperature_setpoint, EE_THERMOSTAT_SET_POINT, 1U);
            BSP_EEPROM_WaitEepromStandbyState();
        }
        /** ==========================================================================*/
        /**         R E D R A W     R O M       S T A T U S      B U T T O N S        */
        /** ==========================================================================*/
        if(IsONEWIRE_ButtonReadyForDrawingActiv() || IsBUTTON_StateChangedActiv())
        {
            BUTTON_StateChangedReset();
            ONEWIRE_ButtonReadyForDrawingReset();
            button_enable_timer = HAL_GetTick();
            buzzer_signal_id = BUZZER_CLICK;
            BUZZER_SignalOn();
           
            if(IsBUTTON_DndActiv()) GUI_DrawBitmap(&bm_btn_dnd_1, BTN_DND_X0, BTN_DND_Y0);
            else GUI_DrawBitmap(&bm_btn_dnd_0, BTN_DND_X0, BTN_DND_Y0);
            
            if(IsBUTTON_CallMaidActiv()) GUI_DrawBitmap(&bm_btn_maid_1, BTN_CMD_X0, BTN_CMD_Y0);
            else GUI_DrawBitmap(&bm_btn_maid_0, BTN_CMD_X0, BTN_CMD_Y0);
            
            if(IsBUTTON_SosResetActiv()) GUI_DrawBitmap(&bm_btn_rst_sos_1, BTN_SOS_X0, BTN_SOS_Y0);
            else GUI_DrawBitmap(&bm_btn_rst_sos_0, BTN_SOS_X0, BTN_SOS_Y0);
        }
    }
    else if(DisplayServiceState == DISPLAY_MESSAGE)    // display message to room guest with button or timeout return
	{
        if((HAL_GetTick() - button_enable_timer) >= BUTTON_RESPONSE_TIME)
        {
            /************************************/
            /*      CHECK BUTTON OK STATE       */
            /************************************/
            if(btn_ok_state != 0U)
            {
                btn_ok_state = 0U;
                if(!last_image_id) display_image_id = 0U;
                else display_image_id = last_image_id;
                last_image_id = 0U;
                DISPLAY_UpdateSet();
                BUTTON_OkActivSet();
                ONEWIRE_SendNewButtonStateSet();
                buzzer_signal_id = BUZZER_CLICK;
                BUZZER_SignalOn();
            }
            /************************************/
            /*    CHECK BUTTON OPENDOOR STATE   */\
            /************************************/
            if(btn_opendoor_state != 0U)
            {
                btn_opendoor_state = 0U;
                if(!last_image_id) display_image_id = 0U;
                else display_image_id = last_image_id;
                last_image_id = 0U;
                DISPLAY_UpdateSet();
                BUTTON_OpenDoorSet();
                ONEWIRE_SendNewButtonStateSet();
                buzzer_signal_id = BUZZER_CLICK;
                BUZZER_SignalOn();
            }
        }
        /************************************/
        /*      CHECK FOR MESSAGE TIMEOUT   */
        /************************************/
        if(display_image_time != 0U)
        {
            if((HAL_GetTick() - display_image_timer) >= (display_image_time * 30000U))
            {
                DISPLAY_UpdateSet();
                ONEWIRE_UpdateDisplayImageSet();
                if(!last_image_id) display_image_id = 0U;
                else display_image_id = last_image_id;
                display_image_time = 0U;
                last_image_id = 0U;
            }
        }
        /** ==========================================================================*/
        /**  C H E C K   H I D D E N     S E T T I N G S     M E N U     B U T T O N  */
        /** ==========================================================================*/
        if(btn_settings_menu_state && !btn_settings_menu_old_state)
        {
            btn_settings_menu_old_state = 1U;
            settings_menu_timer = HAL_GetTick(); 
        }
        else if(btn_settings_menu_state && btn_settings_menu_old_state)
        {
            if((HAL_GetTick() - settings_menu_timer) >= SETTINGS_MENU_ENABLE_TIME)
            {
                btn_settings_menu_state = 0U;
                btn_settings_menu_old_state = 0U;
                DISPLAY_CreateSettings1Screen();
                DISPLAY_FancoilNtcSensorStateReset();
                DISPLAY_AmbientNtcSensorStateReset();
                DisplayServiceState = DISPLAY_SETTINGS_1;
                buzzer_signal_id = BUZZER_CLICK;
                BUZZER_SignalOn();
            }
        }
        else if(!btn_settings_menu_state && btn_settings_menu_old_state) btn_settings_menu_old_state = 0U;
    }    
    else if(DisplayServiceState == DISPLAY_ERROR)      // display error message to inform user about misuse and conditions
	{
        if(display_image_time != 0U)
        {
            if((HAL_GetTick() - display_image_timer) >= (display_image_time * 30000U))
            {
                DISPLAY_UpdateSet();
                ONEWIRE_UpdateDisplayImageSet();
                if(!last_image_id) display_image_id = 0U;
                else display_image_id = last_image_id;
                display_image_time = 0U;
                last_image_id = 0U;
            }
        }
        /** ==========================================================================*/
        /**  C H E C K   H I D D E N     S E T T I N G S     M E N U     B U T T O N  */
        /** ==========================================================================*/
        if(btn_settings_menu_state && !btn_settings_menu_old_state)
        {
            btn_settings_menu_old_state = 1U;
            settings_menu_timer = HAL_GetTick();// + SETTINGS_MENU_ENABLE_TIME; 
        }
        else if(btn_settings_menu_state && btn_settings_menu_old_state)
        {
            if((HAL_GetTick() - settings_menu_timer) >= SETTINGS_MENU_ENABLE_TIME)
            {
                btn_settings_menu_state = 0U;
                btn_settings_menu_old_state = 0U;
                DISPLAY_CreateSettings1Screen();
                DISPLAY_FancoilNtcSensorStateReset();
                DISPLAY_AmbientNtcSensorStateReset();
                DisplayServiceState = DISPLAY_SETTINGS_1;
                buzzer_signal_id = BUZZER_CLICK;
                BUZZER_SignalOn();
            }
        }
        else if(!btn_settings_menu_state && btn_settings_menu_old_state) btn_settings_menu_old_state = 0U;
    }
    else if(DisplayServiceState == DISPLAY_SETTINGS_1) // thermostat setup menu 1
    {
        thermostat_operation_mode = (uint8_t)(RADIO_GetValue(hRADIO_ThermostatModeSelect));
        
        if(fan_control_type != (uint8_t)(RADIO_GetValue(hRADIO_FancoilFanControlType)))
        {
            fan_control_type = (uint8_t)(RADIO_GetValue(hRADIO_FancoilFanControlType));
            FAN_SetControlType(fan_control_type);
            
            GUI_ClearRect(210,150,320,270);
            
            if (fan_control_type == 3U)
            {
                GUI_GotoXY(210, 154);
                GUI_DispString("PROPORTIONAL BAND");
                GUI_GotoXY(210, 166);
                GUI_DispString("SETPOINT +/- x0.1*C");
                GUI_GotoXY(210, 194);
                GUI_DispString("FAN MINIMUM RPM");
                GUI_GotoXY(210, 206);
                GUI_DispString("OUTPUT LIMIT x1%");
                GUI_GotoXY(210, 234);
                GUI_DispString("FAN MAXIMUM RPM");
                GUI_GotoXY(210, 246);
                GUI_DispString("OUTPUT LIMIT x1%");
            }
            else 
            {
                GUI_GotoXY(210, 154);
                GUI_DispString("SETPOINT TRESHOLD");
                GUI_GotoXY(210, 166);
                GUI_DispString("TEMPERATURE x0.1*C");
                GUI_GotoXY(210, 194);
                GUI_DispString("FAN LOW SPEED");
                GUI_GotoXY(210, 206);
                GUI_DispString("SETPOINT +/- x0.1*C");
                GUI_GotoXY(210, 234);
                GUI_DispString("FAN MIDDLE SPEED");
                GUI_GotoXY(210, 246);
                GUI_DispString("SETPOINT +/- x0.1*C");
            }
        }
        
        thermostat_max_setpoint_temperature = (uint16_t)(SPINBOX_GetValue(hSPINBOX_ThermostatMaxSetpointTemperature));
        thermostat_min_setpoint_temperature = (uint16_t)(SPINBOX_GetValue(hSPINBOX_ThermostatMinSetpointTemperature));
        fan_speed_treshold = (uint8_t)(SPINBOX_GetValue(hSPINBOX_FancoilFanSpeedTreshold));
        fan_low_speed_band = (uint8_t)(SPINBOX_GetValue(hSPINBOX_FancoilFanLowSpeedBand));
        fan_middle_speed_band = (uint8_t)(SPINBOX_GetValue(hSPINBOX_FancoilFanMiddleSpeedBand));
        onewire_interface_address = (uint8_t)(SPINBOX_GetValue(hSPINBOX_OneWireInterfaceAddress));
        rs485_interface_address = (uint16_t)(SPINBOX_GetValue(hSPINBOX_Rs485InterfaceAddress));
        /************************************/
        /*      CHECK BUTTON OK STATE       */
        /************************************/
        if(BUTTON_IsPressed(hBUTTON_Ok)) 
        {
            SaveSettings();
            DISPLAY_DeleteSettings1Screen();
            if(!last_image_id) display_image_id = 0U;
            else display_image_id = last_image_id;
            last_image_id = 0U;
            DISPLAY_UpdateSet();
            DisplayServiceState = DISPLAY_THERMOSTAT;
        }
        /************************************/
        /*      CHECK BUTTON NEXT STATE     */
        /************************************/
        else if(BUTTON_IsPressed(hBUTTON_Next)) 
        {
            DISPLAY_FancoilNtcSensorStateReset();
            DISPLAY_AmbientNtcSensorStateReset();
            DISPLAY_AmbientTemperatureUpdateSet();
            DISPLAY_FancoilTemperatureUpdateSet();
            DISPLAY_DeleteSettings1Screen();
            DISPLAY_CreateSettings2Screen();
            DisplayServiceState = DISPLAY_SETTINGS_2;
        }
    }
    else if(DisplayServiceState == DISPLAY_SETTINGS_2) // thermostat setup menu 2
    {
        thermostat_max_cooling_temperature = (uint16_t) (SPINBOX_GetValue(hSPINBOX_CoolingMaxTemperature) * 10U);
        thermostat_min_heating_temperature = (uint16_t) (SPINBOX_GetValue(hSPINBOX_HeatingMinTemperature) * 10U);
        thermostat_error_duraton = (uint16_t)(SPINBOX_GetValue(hSPINBOX_ErrorDurationTime));
        thermostat_ambient_ntc_offset = SPINBOX_GetValue(hSPINBOX_AmbientNtcOffset);
        thermostat_fancoil_ntc_offset = SPINBOX_GetValue(hSPINBOX_FancoillNtcOffset);
        fancoil_temperature_limit_mode = (uint8_t)(RADIO_GetValue(hRADIO_TemperatureLimitType));
 
        if(CHECKBOX_GetState(hCHECKBOX_FancoilErrorAutorestart))    FANCOIL_ErrorAutorestartSet();  
        else FANCOIL_ErrorAutorestartReset();          
        if(CHECKBOX_GetState(hCHECKBOX_AmbientNtcErrorReport))      THERMOSTAT_AmbientNtcErrorReportSet(); 
        else THERMOSTAT_AmbientNtcErrorReportReset();
        if(CHECKBOX_GetState(hCHECKBOX_FancoilNtcErrorReport))      FANCOIL_NtcErrorReportSet();       
        else FANCOIL_NtcErrorReportReset(); 
        if(CHECKBOX_GetState(hCHECKBOX_FancoilCoolingErrorReport))  FANCOIL_CoolingErrorReportSet();  
        else FANCOIL_CoolingErrorReportReset();
        if(CHECKBOX_GetState(hCHECKBOX_FancoilHeatingErrorReport))  FANCOIL_HeatingErrorReportSet(); 
        else FANCOIL_HeatingErrorReportReset();
        if(CHECKBOX_GetState(hCHECKBOX_AmbientNtcErrorShutdown))    THERMOSTAT_AmbientNtcErrorShutdownSet(); 
        else THERMOSTAT_AmbientNtcErrorShutdownReset();
        if(CHECKBOX_GetState(hCHECKBOX_FancoilNtcErrorShutdown))    FANCOIL_FancoilNtcErrorShutdownSet();  
        else FANCOIL_FancoilNtcErrorShutdownReset(); 
        if(CHECKBOX_GetState(hCHECKBOX_FancoilCoolingErrorShutdown))FANCOIL_CoolingErrorShutdownSet();        
        else FANCOIL_CoolingErrorShutdownReset();
        if(CHECKBOX_GetState(hCHECKBOX_FancoilHeatingErrorShutdown))FANCOIL_HeatingErrorShutdownSet();
        else FANCOIL_HeatingErrorShutdownReset();
        /************************************/
        /*    ROOM TEMPERATURE NTC SENSOR   */
        /************************************/
        if(!IsAMBIENT_NTC_SensorConnected())
        {
            if(!IsDISPLAY_AmbientNtcSensorStateUpdated())
            {
                DISPLAY_AmbientNtcSensorStateSet();
                GUI_ClearRect(410, 10, 480, 50);
                GUI_GotoXY(410, 20);
                GUI_SetColor(GUI_YELLOW);
                GUI_SetFont(GUI_FONT_20_1);
                GUI_DispString("ERROR");                
            }
        }
        else if(IsDISPLAY_AmbientTemperatureUpdated())
        {
            DISPLAY_AmbientTemperatureUpdateReset();
            DISPLAY_AmbientNtcSensorStateReset();
            GUI_ClearRect(410, 10, 480, 50);
            GUI_GotoXY(410, 20);
            GUI_SetColor(GUI_YELLOW);
            GUI_SetFont(GUI_FONT_20_1);
            DISPLAY_Temperature(room_temperature);
        }
        /************************************/
        /* FANCOIL TEMPERATURE NTC SENSOR   */
        /************************************/
        if(!IsFANCOIL_NTC_SensorConnected())
        {
            if( !IsDISPLAY_FancoilNtcSensorStateUpdated())
            {
                DISPLAY_FancoilNtcSensorStateSet();
                GUI_ClearRect(410, 50, 480, 80);
                GUI_GotoXY(410, 60);
                GUI_SetColor(GUI_YELLOW);
                GUI_SetFont(GUI_FONT_20_1);
                GUI_DispString("ERROR");                
            }
        }
        else if(IsDISPLAY_FancoilTemperatureUpdated())
        {
            DISPLAY_FancoilTemperatureUpdateReset();
            DISPLAY_FancoilNtcSensorStateReset();
            GUI_ClearRect(410, 50, 480, 80);
            GUI_GotoXY(410, 60);
            GUI_SetColor(GUI_YELLOW);
            GUI_SetFont(GUI_FONT_20_1);
            DISPLAY_Temperature(fancoil_temperature);
        }
        /************************************/
        /*      CHECK BUTTON OK STATE       */
        /************************************/   
        if(BUTTON_IsPressed(hBUTTON_Ok)) 
        {
            SaveSettings();
            DISPLAY_DeleteSettings2Screen();
            if(!last_image_id) display_image_id = 0U;
            else display_image_id = last_image_id;
            last_image_id = 0U;
            DISPLAY_UpdateSet();
            DisplayServiceState = DISPLAY_THERMOSTAT;
        }
        /************************************/
        /*      CHECK BUTTON NEXT STATE     */
        /************************************/
        else if(BUTTON_IsPressed(hBUTTON_Next)) 
        {
            DISPLAY_DeleteSettings2Screen();
            DISPLAY_CreateSettings3Screen();
            DisplayServiceState = DISPLAY_SETTINGS_3;
        }
    }
    else if(DisplayServiceState == DISPLAY_SETTINGS_3) // thermostat setup menu 3
    { 
        freezing_protection_temperature = (uint8_t)(SPINBOX_GetValue(hSPINBOX_FreezingProtectionTemperature));
        fan_night_quiet_mode_start_hour = (uint8_t)(SPINBOX_GetValue(hSPINBOX_FancoilNightQuietModeStart));
        
        if(CHECKBOX_GetState(hCHECKBOX_FancoilRpmSensor))               FANCOIL_RpmSensorSet();  
        else FANCOIL_RpmSensorReset();          
        if(CHECKBOX_GetState(hCHECKBOX_FancoilRpmSensorErrorReport))    FANCOIL_RpmSensorErrorReportSet(); 
        else FANCOIL_RpmSensorErrorReportReset();
        if(CHECKBOX_GetState(hCHECKBOX_FancoilRpmSensorErrorShutdown))  FANCOIL_RpmSensorErrorShutdownSet();       
        else FANCOIL_RpmSensorErrorShutdownReset(); 
        if(CHECKBOX_GetState(hCHECKBOX_FancoilFilterDirtyMonitor))      FANCOIL_FilterDirtyMonitorSet();  
        else FANCOIL_FilterDirtyMonitorReset();
        if(CHECKBOX_GetState(hCHECKBOX_FancoilFilterDirtyReport))       FANCOIL_FilterDirtyReportSet(); 
        else FANCOIL_FilterDirtyReportReset();
        if(CHECKBOX_GetState(hCHECKBOX_FancoilFilterDirtyShutdown))     FANCOIL_FilterDirtyShutdownSet(); 
        else FANCOIL_FilterDirtyShutdownReset();
        if(CHECKBOX_GetState(hCHECKBOX_FreezingProtection))             FANCOIL_FreezingProtectionSet();  
        else FANCOIL_FreezingProtectionReset(); 
        if(CHECKBOX_GetState(hCHECKBOX_FreezingProtectionReport))       FANCOIL_FreezingProtectionReportSet();        
        else FANCOIL_FreezingProtectionReportReset();
        if(CHECKBOX_GetState(hCHECKBOX_FancoilValveShutdownOnSetpoint)) FANCOIL_ValveFollowFanSet();
        else FANCOIL_ValveFollowFanReset();
        if(CHECKBOX_GetState(hCHECKBOX_FancoilNightQuietMode))          FANCOIL_NightQuietModeSet();
        else FANCOIL_NightQuietModeReset();
        /************************************/
        /*      CHECK BUTTON OK STATE       */
        /************************************/
        if(BUTTON_IsPressed(hBUTTON_Ok)) 
        {
            SaveSettings();
            DISPLAY_DeleteSettings3Screen();
            if(!last_image_id) display_image_id = 0U;
            else display_image_id = last_image_id;
            last_image_id = 0U;
            DISPLAY_UpdateSet();
            DisplayServiceState = DISPLAY_THERMOSTAT;
        }
        /************************************/
        /*      CHECK BUTTON NEXT STATE     */
        /************************************/        
        else if(BUTTON_IsPressed(hBUTTON_Next)) 
        {
            DISPLAY_DeleteSettings3Screen();
            DISPLAY_CreateSettings1Screen();
            DisplayServiceState = DISPLAY_SETTINGS_1;
        }
    }
#if defined DEMO_MODE
    else if(DisplayServiceState == DISPLAY_DEMO_MENU) // demo menu activ
    { 
        /************************************/
        /*        RUN  SELECTED  MODE       */
        /************************************/
        if(display_image_id != 0U)
        {
            if((HAL_GetTick() - demo_menu_timer) >= DEMO_MENU_DELAY)
            {
                demo_menu_timer = HAL_GetTick();
                buzzer_signal_id = BUZZER_CLICK;
                BUZZER_SignalOn();
                display_image_time = 0U;
                DISPLAY_UpdateSet();
                ++display_image_id;
                
                if(((display_image_id % 10U) >= 6U) || (display_image_id > 100U))
                {
                    display_image_id = 0U;
                    DISPLAY_CreateDemoMenu();
                }
            }
        }
        /************************************/
        /*  CHECK BUTTON DOOR OPEN STATE    */
        /************************************/
        else if(BUTTON_IsPressed(hBUTTON_DoorOpen)) 
        {
            DISPLAY_UpdateSet();
            demo_menu_timer = HAL_GetTick();
            buzzer_signal_id = BUZZER_CLICK;
            BUZZER_SignalOn();
            display_image_id = ICONVIEW_GetSel(hICONVIEW_DemoMenu) * 10U + 1U; 
            DISPLAY_DeleteDemoMenu();
        }
        /************************************/
        /*      CHECK BUTTON NEXT STATE     */
        /************************************/
        else if(BUTTON_IsPressed(hBUTTON_Next)) 
        {
            DISPLAY_UpdateSet();
            DISPLAY_DeleteDemoMenu();
            demo_menu_timer = HAL_GetTick();
            buzzer_signal_id = BUZZER_CLICK;
            BUZZER_SignalOn();
            display_image_id = 101U;
            display_image_time = 0U;
            GUI_SelectLayer(0);
            GUI_SetBkColor(GUI_GRAY); 
            GUI_Clear();
            GUI_SelectLayer(1);
            GUI_SetBkColor(GUI_TRANSPARENT); 
            GUI_Clear();
        }
        /************************************/
        /*      CHECK BUTTON OK STATE       */
        /************************************/
        else if(BUTTON_IsPressed(hBUTTON_Ok)) 
        {
            DISPLAY_UpdateSet();
            DISPLAY_DeleteDemoMenu();
            DisplayServiceState = DISPLAY_THERMOSTAT;
            buzzer_signal_id = BUZZER_CLICK;
            BUZZER_SignalOn();
            display_image_id = 0U;
            display_image_time = 0U;
            last_image_id = 0U;
        }
    }
#endif
}


static void DISPLAY_DateTime(void)
{
	uint8_t disp_buff[24];
    
    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BCD);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BCD);
    GUI_MULTIBUF_BeginEx(1);
    GUI_ClearRect(240, 220, 480, 270);
    Hex2Str(&date.Date, 1U, &disp_buff[0]);
    disp_buff[2] = '.';
    Hex2Str(&date.Month, 1U, &disp_buff[3]);
    disp_buff[5] = '.';
    disp_buff[6] = '2';
    disp_buff[7] = '0';
    Hex2Str(&date.Year, 1U, &disp_buff[8]);
    disp_buff[10] = '.';
    disp_buff[11] = ' ';
    disp_buff[12] = ' ';
    Hex2Str(&time.Hours, 1U, &disp_buff[13]);
    disp_buff[15] = ':';
    Hex2Str(&time.Minutes, 1U, &disp_buff[16]);
    disp_buff[18] = 0U;
    GUI_SetFont(GUI_FONT_24_1);
    GUI_SetColor(GUI_WHITE);
    GUI_SetTextMode(GUI_TM_TRANS);
    GUI_SetTextAlign(GUI_TA_RIGHT|GUI_TA_VCENTER);
    GUI_GotoXY(470, 255);
    GUI_DispString((const char *)disp_buff);
    GUI_MULTIBUF_EndEx(1);
}


static void DISPLAY_FullBrightnes(void)
{    
    DISPLAY_ScreensaverReset();
    display_screensaver_timer = HAL_GetTick();
    __HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_1, DISPLAY_BRIGHTNESS_HIGH);
}


static void DISPLAY_TemperatureSetPoint(void)
{
    uint32_t dec = ((temperature_setpoint & 0x3FU) / 10U);
    uint32_t unit= ((temperature_setpoint & 0x3FU) - (dec * 10U));
    
    GUI_MULTIBUF_BeginEx(1);
    GUI_ClearRect(120, 140, 250, 220);
    
    if(dec == 1U) GUI_DrawBitmap(&bm_number_1, SP_TEMP_DEC_POS, SP_TEMP_V_POS);
    else if(dec == 2U) GUI_DrawBitmap(&bm_number_2, SP_TEMP_DEC_POS, SP_TEMP_V_POS);
    else if(dec == 3U) GUI_DrawBitmap(&bm_number_3, SP_TEMP_DEC_POS, SP_TEMP_V_POS);
    
    if(unit == 0U) GUI_DrawBitmap(&bm_number_0, SP_TEMP_UNIT_POS, SP_TEMP_V_POS);
    else if(unit == 1U) GUI_DrawBitmap(&bm_number_1, SP_TEMP_UNIT_POS, SP_TEMP_V_POS);
    else if(unit == 2U) GUI_DrawBitmap(&bm_number_2, SP_TEMP_UNIT_POS, SP_TEMP_V_POS);
    else if(unit == 3U) GUI_DrawBitmap(&bm_number_3, SP_TEMP_UNIT_POS, SP_TEMP_V_POS);
    else if(unit == 4U) GUI_DrawBitmap(&bm_number_4, SP_TEMP_UNIT_POS, SP_TEMP_V_POS);
    else if(unit == 5U) GUI_DrawBitmap(&bm_number_5, SP_TEMP_UNIT_POS, SP_TEMP_V_POS);
    else if(unit == 6U) GUI_DrawBitmap(&bm_number_6, SP_TEMP_UNIT_POS, SP_TEMP_V_POS);
    else if(unit == 7U) GUI_DrawBitmap(&bm_number_7, SP_TEMP_UNIT_POS, SP_TEMP_V_POS);
    else if(unit == 8U) GUI_DrawBitmap(&bm_number_8, SP_TEMP_UNIT_POS, SP_TEMP_V_POS);
    else if(unit == 9U) GUI_DrawBitmap(&bm_number_9, SP_TEMP_UNIT_POS, SP_TEMP_V_POS);
    GUI_MULTIBUF_EndEx(1);
}


static void DISPLAY_Temperature(uint16_t value)
{
    uint32_t temperature = value;
    if(temperature & (1U << 15)) GUI_DispString("-");
    else GUI_DispString("+");
    temperature &= 0x00000FFFU;
    GUI_DispDecMin(temperature / 10);
    GUI_DispString(".");
    GUI_DispDec(temperature % 10, 1);
    GUI_DispString("*C");
}


static void DISPLAY_CreateSettings1Screen(void)
{
    GUI_SelectLayer(0);
    GUI_Clear();
    GUI_SelectLayer(1);
    GUI_SetBkColor(GUI_TRANSPARENT); 
    GUI_Clear();
    GUI_MULTIBUF_BeginEx(1);
    
    hRADIO_ThermostatModeSelect = RADIO_CreateEx(10, 20, 150, 80, 0,WM_CF_SHOW, 0, GUI_ID_RADIO_ThermostatModeSelect, 4, 20);
    RADIO_SetTextColor(hRADIO_ThermostatModeSelect, GUI_GREEN);
    RADIO_SetText(hRADIO_ThermostatModeSelect, "OFF", 0);
    RADIO_SetText(hRADIO_ThermostatModeSelect, "COOLING", 1);
    RADIO_SetText(hRADIO_ThermostatModeSelect, "HEATING", 2);
    RADIO_SetText(hRADIO_ThermostatModeSelect, "REMOTE", 3);
    RADIO_SetValue(hRADIO_ThermostatModeSelect, thermostat_operation_mode);
    
    hRADIO_FancoilFanControlType = RADIO_CreateEx(10, 150, 150, 80, 0,WM_CF_SHOW, 0, GUI_ID_RADIO_FancoilFanControlType, 4, 20);
    RADIO_SetTextColor(hRADIO_FancoilFanControlType, GUI_GREEN);
    RADIO_SetText(hRADIO_FancoilFanControlType, "ON / OFF", 0);
    RADIO_SetText(hRADIO_FancoilFanControlType, "3 SPEED", 1);
    RADIO_SetText(hRADIO_FancoilFanControlType, "TRIAC", 2);
    RADIO_SetText(hRADIO_FancoilFanControlType, "BLDC", 3);
    RADIO_SetValue(hRADIO_FancoilFanControlType, fan_control_type);
    
    hSPINBOX_ThermostatMaxSetpointTemperature = SPINBOX_CreateEx(110, 20, 90, 30, 0, WM_CF_SHOW, GUI_ID_SPINBOX_ThermostatMaxSetpointTemperature, 15, 40);
    SPINBOX_SetEdge(hSPINBOX_ThermostatMaxSetpointTemperature, SPINBOX_EDGE_CENTER);
    SPINBOX_SetValue(hSPINBOX_ThermostatMaxSetpointTemperature, thermostat_max_setpoint_temperature);

    hSPINBOX_ThermostatMinSetpointTemperature = SPINBOX_CreateEx(110, 70, 90, 30, 0, WM_CF_SHOW, GUI_ID_SPINBOX_ThermostatMinSetpointTemperature, 15, 40);
    SPINBOX_SetEdge(hSPINBOX_ThermostatMinSetpointTemperature, SPINBOX_EDGE_CENTER);
    SPINBOX_SetValue(hSPINBOX_ThermostatMinSetpointTemperature, thermostat_min_setpoint_temperature);

    hSPINBOX_FancoilFanSpeedTreshold = SPINBOX_CreateEx(110, 150, 90, 30, 0, WM_CF_SHOW, GUI_ID_SPINBOX_FancoilFanSpeedTreshold, 0, 100);
    SPINBOX_SetEdge(hSPINBOX_FancoilFanSpeedTreshold, SPINBOX_EDGE_CENTER);
    SPINBOX_SetValue(hSPINBOX_FancoilFanSpeedTreshold, fan_speed_treshold);

    hSPINBOX_FancoilFanLowSpeedBand = SPINBOX_CreateEx(110, 190, 90, 30, 0, WM_CF_SHOW, GUI_ID_SPINBOX_FancoilFanLowSpeedBand, 0, 100);
    SPINBOX_SetEdge(hSPINBOX_FancoilFanLowSpeedBand, SPINBOX_EDGE_CENTER);
    SPINBOX_SetValue(hSPINBOX_FancoilFanLowSpeedBand, fan_low_speed_band);

    hSPINBOX_FancoilFanMiddleSpeedBand = SPINBOX_CreateEx(110, 230, 90, 30, 0, WM_CF_SHOW, GUI_ID_SPINBOX_FancoilFanMiddleSpeedBand, 0, 100);
    SPINBOX_SetEdge(hSPINBOX_FancoilFanMiddleSpeedBand, SPINBOX_EDGE_CENTER);
    SPINBOX_SetValue(hSPINBOX_FancoilFanMiddleSpeedBand, fan_middle_speed_band);

    hSPINBOX_OneWireInterfaceAddress = SPINBOX_CreateEx(340, 20, 95, 30, 0, WM_CF_SHOW, GUI_ID_SPINBOX_OneWireInterfaceAddress, 1, 9);
    SPINBOX_SetEdge(hSPINBOX_OneWireInterfaceAddress, SPINBOX_EDGE_CENTER);
    SPINBOX_SetValue(hSPINBOX_OneWireInterfaceAddress, onewire_interface_address);

    hSPINBOX_Rs485InterfaceAddress = SPINBOX_CreateEx(340, 70, 95, 30, 0, WM_CF_SHOW, GUI_ID_SPINBOX_Rs485InterfaceAddress, 100, 65000);
    SPINBOX_SetEdge(hSPINBOX_Rs485InterfaceAddress, SPINBOX_EDGE_CENTER);
    SPINBOX_SetEditMode(hSPINBOX_Rs485InterfaceAddress, SPINBOX_EM_EDIT);
    SPINBOX_SetValue(hSPINBOX_Rs485InterfaceAddress, rs485_interface_address);

    hBUTTON_Next = BUTTON_Create(390, 150, 80, 30, GUI_ID_BUTTON_Next, WM_CF_SHOW);
    BUTTON_SetText(hBUTTON_Next, "NEXT");

    hBUTTON_Ok = BUTTON_Create(390, 200, 80, 30, GUI_ID_BUTTON_Ok, WM_CF_SHOW);
    BUTTON_SetText(hBUTTON_Ok, "OK");

    GUI_SetFont(&GUI_Font13_ASCII);
    GUI_SetColor(GUI_WHITE);
    GUI_GotoXY(210, 24);
    GUI_DispString("MAX. USER SETPOINT");
    GUI_GotoXY(210, 36);
    GUI_DispString("TEMPERATURE x1*C");
    GUI_GotoXY(210, 74);
    GUI_DispString("MIN. USER SETPOINT");
    GUI_GotoXY(210, 86);
    GUI_DispString("TEMPERATURE x1*C");
    
    if (fan_control_type == 3U)
    {
        GUI_GotoXY(210, 154);
        GUI_DispString("PROPORTIONAL BAND");
        GUI_GotoXY(210, 166);
        GUI_DispString("SETPOINT +/- x0.1*C");
        GUI_GotoXY(210, 194);
        GUI_DispString("FAN MINIMUM RPM");
        GUI_GotoXY(210, 206);
        GUI_DispString("OUTPUT LIMIT x1%");
        GUI_GotoXY(210, 234);
        GUI_DispString("FAN MAXIMUM RPM");
        GUI_GotoXY(210, 246);
        GUI_DispString("OUTPUT LIMIT x1%");
    }
    else 
    {
        GUI_GotoXY(210, 154);
        GUI_DispString("SETPOINT TRESHOLD");
        GUI_GotoXY(210, 166);
        GUI_DispString("TEMPERATURE x0.1*C");
        GUI_GotoXY(210, 194);
        GUI_DispString("FAN LOW SPEED");
        GUI_GotoXY(210, 206);
        GUI_DispString("SETPOINT +/- x0.1*C");
        GUI_GotoXY(210, 234);
        GUI_DispString("FAN MIDDLE SPEED");
        GUI_GotoXY(210, 246);
        GUI_DispString("SETPOINT +/- x0.1*C");
    }
            
    GUI_GotoXY(440, 24);
    GUI_DispString("ONE");
    GUI_GotoXY(440, 36);
    GUI_DispString("WIRE");
    GUI_GotoXY(440, 80);
    GUI_DispString("RS485");
    GUI_GotoXY(10, 4);
    GUI_DispString("THERMOSTAT OPERATION MODE");
    GUI_GotoXY(10, 120);
    GUI_DispString("FANCOIL FAN SPEED CONTROL MODE");
    GUI_GotoXY(340, 4);
    GUI_DispString("INTERFACE ADDRESSE");
    GUI_DrawHLine(12, 5, 320);
    GUI_DrawHLine(12, 335, 470);
    GUI_DrawHLine(130, 5, 320);
    GUI_MULTIBUF_EndEx(1);
}


static void DISPLAY_CreateSettings2Screen(void)
 {
    GUI_SelectLayer(0);
    GUI_Clear();
    GUI_SelectLayer(1);
    GUI_SetBkColor(GUI_TRANSPARENT); 
    GUI_Clear();
    GUI_MULTIBUF_BeginEx(1);
    
    hSPINBOX_CoolingMaxTemperature = SPINBOX_CreateEx(10, 10, 90, 30, 0, WM_CF_SHOW, GUI_ID_SPINBOX_CoolingMaxTemperature, 10, 50);
    SPINBOX_SetEdge(hSPINBOX_CoolingMaxTemperature, SPINBOX_EDGE_CENTER);
    SPINBOX_SetValue(hSPINBOX_CoolingMaxTemperature, (thermostat_max_cooling_temperature / 10U));

    hSPINBOX_HeatingMinTemperature = SPINBOX_CreateEx(10, 50, 90, 30, 0, WM_CF_SHOW, GUI_ID_SPINBOX_HeatingMinTemperature, 10, 50);
    SPINBOX_SetEdge(hSPINBOX_HeatingMinTemperature, SPINBOX_EDGE_CENTER);
    SPINBOX_SetValue(hSPINBOX_HeatingMinTemperature, (thermostat_min_heating_temperature / 10U));

    hSPINBOX_ErrorDurationTime = SPINBOX_CreateEx(10, 90, 90, 30, 0, WM_CF_SHOW, GUI_ID_SPINBOX_ErrorDurationTime, 1, 240);
    SPINBOX_SetEdge(hSPINBOX_ErrorDurationTime, SPINBOX_EDGE_CENTER);
    SPINBOX_SetValue(hSPINBOX_ErrorDurationTime, thermostat_error_duraton);

    hSPINBOX_AmbientNtcOffset = SPINBOX_CreateEx(230, 10, 90, 30, 0, WM_CF_SHOW, GUI_ID_SPINBOX_AmbientNtcOffset, -100, 100);
    SPINBOX_SetEdge(hSPINBOX_AmbientNtcOffset, SPINBOX_EDGE_CENTER);
    SPINBOX_SetValue(hSPINBOX_AmbientNtcOffset, thermostat_ambient_ntc_offset);

    hSPINBOX_FancoillNtcOffset = SPINBOX_CreateEx(230, 50, 90, 30, 0, WM_CF_SHOW, GUI_ID_SPINBOX_FancoillNtcOffset, -100, 100);
    SPINBOX_SetEdge(hSPINBOX_FancoillNtcOffset, SPINBOX_EDGE_CENTER);
    SPINBOX_SetValue(hSPINBOX_FancoillNtcOffset, thermostat_fancoil_ntc_offset);

    hCHECKBOX_FancoilCoolingErrorReport = CHECKBOX_Create(10, 160, 80, 20, 0, GUI_ID_CHECK_FancoilCoolingErrorReport, WM_CF_SHOW);
    CHECKBOX_SetTextColor(hCHECKBOX_FancoilCoolingErrorReport, GUI_GREEN);	
    CHECKBOX_SetText(hCHECKBOX_FancoilCoolingErrorReport, "Cooling");
    if(IsFANCOIL_CoolingErrorReportActiv()) CHECKBOX_SetState(hCHECKBOX_FancoilCoolingErrorReport, 1);
    else CHECKBOX_SetState(hCHECKBOX_FancoilCoolingErrorReport, 0);

    hCHECKBOX_FancoilHeatingErrorReport = CHECKBOX_Create(80, 160, 80, 20, 0, GUI_ID_CHECK_FancoilHeatingErrorReport, WM_CF_SHOW);
    CHECKBOX_SetTextColor(hCHECKBOX_FancoilHeatingErrorReport, GUI_GREEN);	
    CHECKBOX_SetText(hCHECKBOX_FancoilHeatingErrorReport, "Heating");
    if(IsFANCOIL_HeatingErrorReportActiv()) CHECKBOX_SetState(hCHECKBOX_FancoilHeatingErrorReport, 1);
    else CHECKBOX_SetState(hCHECKBOX_FancoilHeatingErrorReport, 0);

    hCHECKBOX_FancoilNtcErrorReport = CHECKBOX_Create(150, 160, 80, 20, 0, GUI_ID_CHECK_FancoilNtcErrorReport, WM_CF_SHOW);
    CHECKBOX_SetTextColor(hCHECKBOX_FancoilNtcErrorReport, GUI_GREEN);	
    CHECKBOX_SetText(hCHECKBOX_FancoilNtcErrorReport, "Fancoil NTC");
    if(IsFANCOIL_NtcErrorReportActiv()) CHECKBOX_SetState(hCHECKBOX_FancoilNtcErrorReport, 1);
    else CHECKBOX_SetState(hCHECKBOX_FancoilNtcErrorReport, 0);

    hCHECKBOX_AmbientNtcErrorReport = CHECKBOX_Create(240, 160, 85, 20, 0, GUI_ID_CHECK_AmbientNtcErrorReport, WM_CF_SHOW);
    CHECKBOX_SetTextColor(hCHECKBOX_AmbientNtcErrorReport, GUI_GREEN);	
    CHECKBOX_SetText(hCHECKBOX_AmbientNtcErrorReport, "Ambient NTC");
    if(IsTHERMOSTAT_AmbientNtcErrorReportActiv()) CHECKBOX_SetState(hCHECKBOX_AmbientNtcErrorReport, 1);
    else CHECKBOX_SetState(hCHECKBOX_AmbientNtcErrorReport, 0);

    hCHECKBOX_FancoilCoolingErrorShutdown = CHECKBOX_Create(10, 210, 80, 20, 0, GUI_ID_CHECK_FancoilCoolingErrorShutdown, WM_CF_SHOW);
    CHECKBOX_SetTextColor(hCHECKBOX_FancoilCoolingErrorShutdown, GUI_GREEN);	
    CHECKBOX_SetText(hCHECKBOX_FancoilCoolingErrorShutdown, "Cooling");
    if(IsFANCOIL_CoolingErrorShutdownActiv()) CHECKBOX_SetState(hCHECKBOX_FancoilCoolingErrorShutdown, 1);
    else CHECKBOX_SetState(hCHECKBOX_FancoilCoolingErrorShutdown, 0);

    hCHECKBOX_FancoilHeatingErrorShutdown = CHECKBOX_Create(80, 210, 80, 20, 0, GUI_ID_CHECK_FancoilHeatingErrorShutdown, WM_CF_SHOW);
    CHECKBOX_SetTextColor(hCHECKBOX_FancoilHeatingErrorShutdown, GUI_GREEN);	
    CHECKBOX_SetText(hCHECKBOX_FancoilHeatingErrorShutdown, "Heating");
    if(IsFANCOIL_HeatingErrorShutdownActiv()) CHECKBOX_SetState(hCHECKBOX_FancoilHeatingErrorShutdown, 1);
    else CHECKBOX_SetState(hCHECKBOX_FancoilHeatingErrorShutdown, 0);

    hCHECKBOX_FancoilNtcErrorShutdown = CHECKBOX_Create(150, 210, 80, 20, 0, GUI_ID_CHECK_FancoilNtcErrorShutdown, WM_CF_SHOW);
    CHECKBOX_SetTextColor(hCHECKBOX_FancoilNtcErrorShutdown, GUI_GREEN);	
    CHECKBOX_SetText(hCHECKBOX_FancoilNtcErrorShutdown, "Fancoil NTC");
    if(IsFANCOIL_FancoilNtcErrorShutdownActiv()) CHECKBOX_SetState(hCHECKBOX_FancoilNtcErrorShutdown, 1);
    else CHECKBOX_SetState(hCHECKBOX_FancoilNtcErrorShutdown, 0);

    hCHECKBOX_AmbientNtcErrorShutdown = CHECKBOX_Create(240, 210, 85, 20, 0, GUI_ID_CHECK_AmbientNtcErrorShutdown, WM_CF_SHOW);
    CHECKBOX_SetTextColor(hCHECKBOX_AmbientNtcErrorShutdown, GUI_GREEN);	
    CHECKBOX_SetText(hCHECKBOX_AmbientNtcErrorShutdown, "Ambient NTC");
    if(IsTHERMOSTAT_AmbientNtcErrorShutdownActiv()) CHECKBOX_SetState(hCHECKBOX_AmbientNtcErrorShutdown, 1);
    else CHECKBOX_SetState(hCHECKBOX_AmbientNtcErrorShutdown, 0);
    
    hCHECKBOX_FancoilErrorAutorestart = CHECKBOX_Create(390, 90, 90, 20, 0, GUI_ID_CHECK_FancoilErrorAutorestart, WM_CF_SHOW);
    CHECKBOX_SetTextColor(hCHECKBOX_FancoilErrorAutorestart, GUI_GREEN);	
    CHECKBOX_SetText(hCHECKBOX_FancoilErrorAutorestart, "Autorestart");
    if(IsFANCOIL_ErrorAutorestartActiv()) CHECKBOX_SetState(hCHECKBOX_FancoilErrorAutorestart, 1);
    else CHECKBOX_SetState(hCHECKBOX_FancoilErrorAutorestart, 0);

    hRADIO_TemperatureLimitType = RADIO_CreateEx(230, 90, 150, 40, 0,WM_CF_SHOW, 0, GUI_ID_RADIO_TemperatureLimitType, 2, 20);
    RADIO_SetTextColor(hRADIO_TemperatureLimitType, GUI_GREEN);
    RADIO_SetText(hRADIO_TemperatureLimitType, "min. / max. limit", 0);
    RADIO_SetText(hRADIO_TemperatureLimitType, "SP. +/- diff.", 1);
    RADIO_SetValue(hRADIO_TemperatureLimitType, fancoil_temperature_limit_mode);

    hBUTTON_Next = BUTTON_Create(390, 150, 80, 30, GUI_ID_BUTTON_Next, WM_CF_SHOW); // Settings
    BUTTON_SetText(hBUTTON_Next, "NEXT");

    hBUTTON_Ok = BUTTON_Create(390, 200, 80, 30, GUI_ID_BUTTON_Ok, WM_CF_SHOW); // Settings
    BUTTON_SetText(hBUTTON_Ok, "OK");

    GUI_SetFont(&GUI_Font13_ASCII);
    GUI_SetColor(GUI_WHITE);
    GUI_GotoXY(110, 14);
    GUI_DispString("MAX. / COOLING");
    GUI_GotoXY(110, 26);
    GUI_DispString("TEMPERATURE x1*C");
    GUI_GotoXY(110, 54);
    GUI_DispString("MIN. / HEATING");
    GUI_GotoXY(110, 66);
    GUI_DispString("TEMPERATURE x1*C");
    GUI_GotoXY(110, 94);
    GUI_DispString("ERROR DURATION");
    GUI_GotoXY(110, 106);
    GUI_DispString("TIME x1 min.");
    GUI_GotoXY(330, 14);
    GUI_DispString("AMBIENT TEMP.");
    GUI_GotoXY(330, 26);
    GUI_DispString("OFFSET x0,1*C");
    GUI_GotoXY(330, 54);
    GUI_DispString("FANCOIL TEMP.");
    GUI_GotoXY(330, 66);
    GUI_DispString("OFFSET x0,1*C");
    GUI_GotoXY(10, 145);
    GUI_DispString("REPORT SELECTED ERROR");
    GUI_GotoXY(10, 195);
    GUI_DispString("DISABLE THERMOSTAT ON SELECTED ERROR");
    GUI_DrawHLine(155, 5, 330);
    GUI_DrawHLine(205, 5, 330);
    GUI_MULTIBUF_EndEx(1);
}


static void DISPLAY_CreateSettings3Screen(void)
{
    uint8_t disp_buff[10];
    
    GUI_SelectLayer(0);
    GUI_Clear();
    GUI_SelectLayer(1);
    GUI_SetBkColor(GUI_TRANSPARENT); 
    GUI_Clear();
    GUI_MULTIBUF_BeginEx(1);
    
    hCHECKBOX_FancoilRpmSensor = CHECKBOX_Create(10, 20, 150, 20, 0, GUI_ID_CHECK_FancoilRpmSensor, WM_CF_SHOW);
    CHECKBOX_SetTextColor(hCHECKBOX_FancoilRpmSensor, GUI_GREEN);	
    CHECKBOX_SetText(hCHECKBOX_FancoilRpmSensor, "Fan RPM hall sensor");
    if(IsFANCOIL_RpmSensorActiv()) CHECKBOX_SetState(hCHECKBOX_FancoilRpmSensor, 1);
    else CHECKBOX_SetState(hCHECKBOX_FancoilRpmSensor, 0);
    
    hCHECKBOX_FancoilRpmSensorErrorReport = CHECKBOX_Create(10, 50, 150, 20, 0, GUI_ID_CHECK_FancoilRpmSensorErrorReport, WM_CF_SHOW);
    CHECKBOX_SetTextColor(hCHECKBOX_FancoilRpmSensorErrorReport, GUI_GREEN);	
    CHECKBOX_SetText(hCHECKBOX_FancoilRpmSensorErrorReport, "RPM sensor error report");
    if(IsFANCOIL_RpmSensorErrorReportActiv()) CHECKBOX_SetState(hCHECKBOX_FancoilRpmSensorErrorReport, 1);
    else CHECKBOX_SetState(hCHECKBOX_FancoilRpmSensorErrorReport, 0);
    
    hCHECKBOX_FancoilRpmSensorErrorShutdown = CHECKBOX_Create(10, 80, 160, 20, 0, GUI_ID_CHECK_FancoilRpmSensorErrorShutdown, WM_CF_SHOW);
    CHECKBOX_SetTextColor(hCHECKBOX_FancoilRpmSensorErrorShutdown, GUI_GREEN);	
    CHECKBOX_SetText(hCHECKBOX_FancoilRpmSensorErrorShutdown, "RPM sensor error shutdown");
    if(IsFANCOIL_RpmSensorErrorShutdownActiv()) CHECKBOX_SetState(hCHECKBOX_FancoilRpmSensorErrorShutdown, 1);
    else CHECKBOX_SetState(hCHECKBOX_FancoilRpmSensorErrorShutdown, 0);
    
    hCHECKBOX_FancoilFilterDirtyMonitor = CHECKBOX_Create(210, 20, 150, 20, 0, GUI_ID_CHECK_FancoilFilterDirtyMonitor, WM_CF_SHOW);
    CHECKBOX_SetTextColor(hCHECKBOX_FancoilFilterDirtyMonitor, GUI_GREEN);	
    CHECKBOX_SetText(hCHECKBOX_FancoilFilterDirtyMonitor, "Filter monitoring");
    if(IsFANCOIL_FilterDirtyMonitorActiv()) CHECKBOX_SetState(hCHECKBOX_FancoilFilterDirtyMonitor, 1);
    else CHECKBOX_SetState(hCHECKBOX_FancoilFilterDirtyMonitor, 0);
    
    hCHECKBOX_FancoilFilterDirtyReport = CHECKBOX_Create(210, 50, 150, 20, 0, GUI_ID_CHECK_FancoilFilterDirtyReport, WM_CF_SHOW);
    CHECKBOX_SetTextColor(hCHECKBOX_FancoilFilterDirtyReport, GUI_GREEN);	
    CHECKBOX_SetText(hCHECKBOX_FancoilFilterDirtyReport, "Filter dirty report");
    if(IsFANCOIL_FilterDirtyReportActiv()) CHECKBOX_SetState(hCHECKBOX_FancoilFilterDirtyReport, 1);
    else CHECKBOX_SetState(hCHECKBOX_FancoilFilterDirtyReport, 0);
    
    hCHECKBOX_FancoilFilterDirtyShutdown = CHECKBOX_Create(210, 80, 150, 20, 0, GUI_ID_CHECK_FancoilFilterDirtyShutdown, WM_CF_SHOW);
    CHECKBOX_SetTextColor(hCHECKBOX_FancoilFilterDirtyShutdown, GUI_GREEN);	
    CHECKBOX_SetText(hCHECKBOX_FancoilFilterDirtyShutdown, "Filter dirty shutdown");
    if(IsFANCOIL_FilterDirtyShutdownActiv()) CHECKBOX_SetState(hCHECKBOX_FancoilFilterDirtyShutdown, 1);
    else CHECKBOX_SetState(hCHECKBOX_FancoilFilterDirtyShutdown, 0);

    hCHECKBOX_FreezingProtection = CHECKBOX_Create(10, 140, 150, 20, 0, GUI_ID_CHECK_FreezingProtection, WM_CF_SHOW);
    CHECKBOX_SetTextColor(hCHECKBOX_FreezingProtection, GUI_GREEN);	
    CHECKBOX_SetText(hCHECKBOX_FreezingProtection, "Freezing protection");
    if(IsFANCOIL_FreezingProtectionActiv()) CHECKBOX_SetState(hCHECKBOX_FreezingProtection, 1);
    else CHECKBOX_SetState(hCHECKBOX_FreezingProtection, 0);
    
    hCHECKBOX_FreezingProtectionReport = CHECKBOX_Create(10, 170, 150, 20, 0, GUI_ID_CHECK_FreezingProtectionReport, WM_CF_SHOW);
    CHECKBOX_SetTextColor(hCHECKBOX_FreezingProtectionReport, GUI_GREEN);	
    CHECKBOX_SetText(hCHECKBOX_FreezingProtectionReport, "Freezing protection report");
    if(IsFANCOIL_FreezingProtectionReportActiv()) CHECKBOX_SetState(hCHECKBOX_FreezingProtectionReport, 1);
    else CHECKBOX_SetState(hCHECKBOX_FreezingProtectionReport, 0);
    
    hCHECKBOX_FancoilValveShutdownOnSetpoint = CHECKBOX_Create(210, 140, 150, 20, 0, GUI_ID_CHECK_FancoilValveShutdownOnSetpoint, WM_CF_SHOW);
    CHECKBOX_SetTextColor(hCHECKBOX_FancoilValveShutdownOnSetpoint, GUI_GREEN);	
    CHECKBOX_SetText(hCHECKBOX_FancoilValveShutdownOnSetpoint, "Valve follow fan");
    if(IsFANCOIL_ValveFollowFanActiv()) CHECKBOX_SetState(hCHECKBOX_FancoilValveShutdownOnSetpoint, 1);
    else CHECKBOX_SetState(hCHECKBOX_FancoilValveShutdownOnSetpoint, 0);
    
    hCHECKBOX_FancoilNightQuietMode = CHECKBOX_Create(210, 170, 150, 20, 0, GUI_ID_CHECK_FancoilNightQuietMode, WM_CF_SHOW);
    CHECKBOX_SetTextColor(hCHECKBOX_FancoilNightQuietMode, GUI_GREEN);	
    CHECKBOX_SetText(hCHECKBOX_FancoilNightQuietMode, "Fan night quiet mode");
    if(IsFANCOIL_NightQuietModeActiv()) CHECKBOX_SetState(hCHECKBOX_FancoilNightQuietMode, 1);
    else CHECKBOX_SetState(hCHECKBOX_FancoilNightQuietMode, 0);
    
    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BCD);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BCD);
    
    Hex2Str(&time.Hours, 1U, &disp_buff[0]);
    disp_buff[2] = ':';
    Hex2Str(&time.Minutes, 1U, &disp_buff[3]);
    disp_buff[5] = ':';
    Hex2Str(&time.Seconds, 1U, &disp_buff[6]);
    disp_buff[8] = 0U;
    
    hEDIT_TimeSet = EDIT_CreateEx(390, 20, 80, 25, 0, WM_CF_SHOW, 0, GUI_ID_EDIT_TimeSet, 8);
    EDIT_SetFont(hEDIT_TimeSet, GUI_FONT_16_1);
    EDIT_SetText(hEDIT_TimeSet, (const char *) disp_buff);
    
    Hex2Str(&date.Date, 1U, &disp_buff[0]);
    disp_buff[2] = '.';
    Hex2Str(&date.Month, 1U, &disp_buff[3]);
    disp_buff[5] = '.';
    Hex2Str(&date.Year, 1U, &disp_buff[6]);
    disp_buff[8] = 0U;
    
    hEDIT_DateSet = EDIT_CreateEx(390, 50, 80, 25, 0, WM_CF_SHOW, 0, GUI_ID_EDIT_DateSet, 8);
    EDIT_SetFont(hEDIT_DateSet, GUI_FONT_16_1);
    EDIT_SetText(hEDIT_DateSet,  (const char *) disp_buff);
    
    hSPINBOX_FreezingProtectionTemperature = SPINBOX_CreateEx(10, 200, 90, 30, 0, WM_CF_SHOW, GUI_ID_SPINBOX_FreezingProtectionTemperature, 5, 80);
    SPINBOX_SetEdge(hSPINBOX_FreezingProtectionTemperature, SPINBOX_EDGE_CENTER);
    SPINBOX_SetValue(hSPINBOX_FreezingProtectionTemperature, freezing_protection_temperature);
    
    hSPINBOX_FancoilNightQuietModeStart = SPINBOX_CreateEx(210, 200, 90, 30, 0, WM_CF_SHOW, GUI_ID_SPINBOX_FancoilNightQuietModeStart, 0, 23);
    SPINBOX_SetEdge(hSPINBOX_FancoilNightQuietModeStart, SPINBOX_EDGE_CENTER);
    SPINBOX_SetValue(hSPINBOX_FancoilNightQuietModeStart, fan_night_quiet_mode_start_hour);
    
    hBUTTON_Next = BUTTON_Create(390, 150, 80, 30, GUI_ID_BUTTON_Next, WM_CF_SHOW);
    BUTTON_SetText(hBUTTON_Next, "NEXT");

    hBUTTON_Ok = BUTTON_Create(390, 200, 80, 30, GUI_ID_BUTTON_Ok, WM_CF_SHOW);
    BUTTON_SetText(hBUTTON_Ok, "OK");

    hBUTTON_Increase = BUTTON_Create(390, 80, 40, 30, GUI_ID_BUTTON_Increase, WM_CF_SHOW);
    BUTTON_SetText(hBUTTON_Increase, "+");

    hBUTTON_Decrease = BUTTON_Create(430, 80, 40, 30, GUI_ID_BUTTON_Decrease, WM_CF_SHOW);
    BUTTON_SetText(hBUTTON_Decrease, "--");
    
    GUI_SetFont(&GUI_Font13_ASCII);
    GUI_SetColor(GUI_WHITE);
    GUI_GotoXY(10, 5);
    GUI_DispString("RPM SENSOR OPTIONS");
    GUI_GotoXY(210, 5);
    GUI_DispString("DUST FILTER OPTIONS");
    GUI_GotoXY(390, 5);
    GUI_DispString("DATE & TIME");
    GUI_GotoXY(10, 120);
    GUI_DispString("FREEZING PROTECTION");
    GUI_GotoXY(210, 120);
    GUI_DispString("ADDITIONAL OPTIONS");
    GUI_GotoXY(110, 204);
    GUI_DispString("ACTIVATE");
    GUI_GotoXY(110, 216);
    GUI_DispString("x0,1*C");
    GUI_GotoXY(310, 204);
    GUI_DispString("QUIET");
    GUI_GotoXY(310, 216);
    GUI_DispString("HOUR");
    GUI_DrawHLine(15, 5, 170);
    GUI_DrawHLine(15, 205, 350);
    GUI_DrawHLine(15, 385, 470);
    GUI_DrawHLine(130, 5, 170);
    GUI_DrawHLine(130, 205, 350);
    GUI_MULTIBUF_EndEx(1);
}


static void DISPLAY_CreateDemoMenu(void)
{
    GUI_SelectLayer(0);
    GUI_SetBkColor(GUI_GRAY); 
    GUI_Clear();
    GUI_SelectLayer(1);
    GUI_SetBkColor(GUI_TRANSPARENT); 
    GUI_Clear();
    GUI_MULTIBUF_BeginEx(1);
    hICONVIEW_DemoMenu = ICONVIEW_CreateEx(0, 0, 480, 272, 0, WM_CF_SHOW, 0, GUI_ID_ICONVIEW_DemoMenu, 80, 80);
    ICONVIEW_SetBkColor(hICONVIEW_DemoMenu, ICONVIEW_CI_BK, GUI_TRANSPARENT);
    ICONVIEW_SetBkColor(hICONVIEW_DemoMenu, ICONVIEW_CI_SEL, GUI_DARKGRAY);
    ICONVIEW_SetTextAlign(hICONVIEW_DemoMenu, GUI_TA_HCENTER|GUI_TA_VCENTER);
    ICONVIEW_SetFont(hICONVIEW_DemoMenu, GUI_FONT_32B_1);
    ICONVIEW_SetSpace(hICONVIEW_DemoMenu, GUI_COORD_X, 40);
    ICONVIEW_SetSpace(hICONVIEW_DemoMenu, GUI_COORD_Y, 5);
    ICONVIEW_SetTextColor(hICONVIEW_DemoMenu, ICONVIEW_CI_UNSEL, GUI_BLACK);
    ICONVIEW_SetTextColor(hICONVIEW_DemoMenu, ICONVIEW_CI_SEL, GUI_BLACK);
    ICONVIEW_AddBitmapItem(hICONVIEW_DemoMenu, &bm_eng_flag,    "GB");
    ICONVIEW_AddBitmapItem(hICONVIEW_DemoMenu, &bm_de_flag,     "DE" );
    ICONVIEW_AddBitmapItem(hICONVIEW_DemoMenu, &bm_fr_flag,     "FR" );
    ICONVIEW_AddBitmapItem(hICONVIEW_DemoMenu, &bm_ksa_flag,    "SA" );
    ICONVIEW_AddBitmapItem(hICONVIEW_DemoMenu, &bm_chn_flag,    "CN" );
    ICONVIEW_AddBitmapItem(hICONVIEW_DemoMenu, &bm_jap_flag,    "JP" );
    ICONVIEW_AddBitmapItem(hICONVIEW_DemoMenu, &bm_ita_flag,    "IT" );
    ICONVIEW_AddBitmapItem(hICONVIEW_DemoMenu, &bm_trk_flag,    "TR" );
    ICONVIEW_AddBitmapItem(hICONVIEW_DemoMenu, &bm_slo_flag,    "SI" );
    hBUTTON_DoorOpen = BUTTON_Create(120, 200, 100, 50, GUI_ID_BUTTON_DoorOpen, WM_CF_SHOW);
    BUTTON_SetText(hBUTTON_DoorOpen, "VIEW");
    hBUTTON_Next = BUTTON_Create(240, 200, 100, 50, GUI_ID_BUTTON_Next, WM_CF_SHOW);
    BUTTON_SetText(hBUTTON_Next, "ERROR");
    hBUTTON_Ok = BUTTON_Create(360, 200, 100, 50, GUI_ID_BUTTON_Ok, WM_CF_SHOW);
    BUTTON_SetText(hBUTTON_Ok, "EXIT");
    GUI_MULTIBUF_EndEx(1);
}


static void DISPLAY_DeleteDemoMenu(void)
{
    WM_DeleteWindow(hICONVIEW_DemoMenu);
    WM_DeleteWindow(hBUTTON_Ok);
    WM_DeleteWindow(hBUTTON_Next);
    WM_DeleteWindow(hBUTTON_DoorOpen);
}


static void DISPLAY_DeleteSettings1Screen(void)
{
    WM_DeleteWindow(hBUTTON_Ok);
    WM_DeleteWindow(hBUTTON_Next);
    WM_DeleteWindow(hRADIO_ThermostatModeSelect);
    WM_DeleteWindow(hRADIO_FancoilFanControlType);
    WM_DeleteWindow(hSPINBOX_ThermostatMaxSetpointTemperature);
    WM_DeleteWindow(hSPINBOX_ThermostatMinSetpointTemperature);
    WM_DeleteWindow(hSPINBOX_FancoilFanSpeedTreshold);
    WM_DeleteWindow(hSPINBOX_FancoilFanLowSpeedBand);
    WM_DeleteWindow(hSPINBOX_FancoilFanMiddleSpeedBand);
    WM_DeleteWindow(hSPINBOX_OneWireInterfaceAddress);
    WM_DeleteWindow(hSPINBOX_Rs485InterfaceAddress);
}


static void DISPLAY_DeleteSettings2Screen(void)
{
    WM_DeleteWindow(hBUTTON_Ok);
    WM_DeleteWindow(hBUTTON_Next);
    WM_DeleteWindow(hRADIO_TemperatureLimitType);
    WM_DeleteWindow(hSPINBOX_HeatingMinTemperature);
    WM_DeleteWindow(hSPINBOX_CoolingMaxTemperature);
    WM_DeleteWindow(hSPINBOX_ErrorDurationTime);
    WM_DeleteWindow(hSPINBOX_AmbientNtcOffset);
    WM_DeleteWindow(hSPINBOX_FancoillNtcOffset);
    WM_DeleteWindow(hCHECKBOX_AmbientNtcErrorReport);
    WM_DeleteWindow(hCHECKBOX_FancoilNtcErrorReport);
    WM_DeleteWindow(hCHECKBOX_FancoilCoolingErrorReport);
    WM_DeleteWindow(hCHECKBOX_FancoilHeatingErrorReport);
    WM_DeleteWindow(hCHECKBOX_AmbientNtcErrorShutdown);
    WM_DeleteWindow(hCHECKBOX_FancoilNtcErrorShutdown);
    WM_DeleteWindow(hCHECKBOX_FancoilCoolingErrorShutdown);
    WM_DeleteWindow(hCHECKBOX_FancoilHeatingErrorShutdown);
    WM_DeleteWindow(hCHECKBOX_FancoilErrorAutorestart);
}


static void DISPLAY_DeleteSettings3Screen(void)
{
    WM_DeleteWindow(hBUTTON_Ok);
    WM_DeleteWindow(hBUTTON_Next);
    WM_DeleteWindow(hBUTTON_Increase);
    WM_DeleteWindow(hBUTTON_Decrease);
    WM_DeleteWindow(hCHECKBOX_FancoilValveShutdownOnSetpoint);
    WM_DeleteWindow(hCHECKBOX_FancoilFilterDirtyMonitor);
    WM_DeleteWindow(hCHECKBOX_FancoilFilterDirtyReport);
    WM_DeleteWindow(hCHECKBOX_FancoilFilterDirtyShutdown);
    WM_DeleteWindow(hCHECKBOX_FreezingProtection);
    WM_DeleteWindow(hCHECKBOX_FreezingProtectionReport);
    WM_DeleteWindow(hCHECKBOX_FancoilRpmSensor);
    WM_DeleteWindow(hCHECKBOX_FancoilRpmSensorErrorReport);
    WM_DeleteWindow(hCHECKBOX_FancoilRpmSensorErrorShutdown);
    WM_DeleteWindow(hCHECKBOX_FancoilNightQuietMode);
    WM_DeleteWindow(hSPINBOX_FreezingProtectionTemperature);
    WM_DeleteWindow(hSPINBOX_FancoilNightQuietModeStart);
    WM_DeleteWindow(hEDIT_TimeSet);
    WM_DeleteWindow(hEDIT_DateSet);
}


static void PID_Hook(GUI_PID_STATE * pState) 
{
    if(pState->Pressed  == 1U)
    {
        pState->Layer = 1U;
        DISPLAY_FullBrightnes();
        
        if ((pState->x >= BTN_INC_X0) && 
            (pState->y >= BTN_INC_Y0) && 
            (pState->x < BTN_INC_X1) && 
            (pState->y < BTN_INC_Y1)) 
        {	
            btn_increase_state = 1U;   
        }
        
        if ((pState->x >= BTN_DEC_X0) && 
            (pState->y >= BTN_DEC_Y0) && 
            (pState->x < BTN_DEC_X1) && 
            (pState->y < BTN_DEC_Y1 - 20)) 
        {	
            btn_decrease_state = 1U;
        }  

        if ((pState->x >= BTN_DND_X0) && 
            (pState->y >= BTN_DND_Y0) && 
            (pState->x < BTN_DND_X1) && 
            (pState->y < BTN_DND_Y1)) 
        {
            btn_dnd_state = 1U;
        }
           
        if ((pState->x >= BTN_CMD_X0) && 
            (pState->y >= BTN_CMD_Y0) && 
            (pState->x < BTN_CMD_X1) && 
            (pState->y < BTN_CMD_Y1)) 
        {
            btn_maid_state = 1U;
        } 
        
        if ((pState->x >= BTN_SOS_X0) && 
            (pState->y >= BTN_SOS_Y0) && 
            (pState->x < BTN_SOS_X1) && 
            (pState->y < BTN_SOS_Y1)) 
        {
            btn_sos_state = 1U;
        } 
        
        if ((pState->x >= BTN_SETTINGS_X0) && 
            (pState->y >= BTN_SETTINGS_Y0) && 
            (pState->x < BTN_SETTINGS_X1) && 
            (pState->y < BTN_SETTINGS_Y1)) 
        {
            btn_settings_menu_state = 1U;
        }
        
        if ((pState->x >= BTN_DOOR_X0) && 
            (pState->y >= BTN_DOOR_Y0 + 20) && 
            (pState->x < BTN_DOOR_X1) && 
            (pState->y < BTN_DOOR_Y1)) 
        {
            btn_opendoor_state = 1U;
        }
           
        if ((pState->x >= BTN_OK_X0) && 
            (pState->y >= BTN_OK_Y0) && 
            (pState->x < BTN_OK_X1) && 
            (pState->y < BTN_OK_Y1)) 
        {
            btn_ok_state = 1U;
        } 
    }
    else
    {
        btn_settings_menu_state = 0U;
        btn_opendoor_state = 0U; 
        btn_decrease_state = 0U;   
        btn_increase_state = 0U;
        btn_maid_state = 0U;
        btn_sos_state = 0U; 
        btn_dnd_state = 0U;
        btn_ok_state = 0U;      
    }
}


static void SaveSettings(void)
 {
    uint8_t ee_buff[16];

    ee_buff[0] = (thermostat_max_cooling_temperature >> 8U);
    ee_buff[1] = (thermostat_max_cooling_temperature & 0xFFU);
    ee_buff[2] = (thermostat_min_heating_temperature >> 8U);
    ee_buff[3] = (thermostat_min_heating_temperature & 0xFFU);
    ee_buff[4] = (thermostat_error_duraton >> 8U);
    ee_buff[5] = (thermostat_error_duraton & 0xFFU);
    ee_buff[6] = (thermostat_ambient_ntc_offset >> 8U);
    ee_buff[7] = (thermostat_ambient_ntc_offset & 0xFFU);
    ee_buff[8] = (thermostat_fancoil_ntc_offset >> 8U);
    ee_buff[9] = (thermostat_fancoil_ntc_offset & 0xFFU);
    ee_buff[10] = fancoil_temperature_limit_mode;
    ee_buff[11] = (thermostat_flags >> 24U);
    ee_buff[12] = (thermostat_flags >> 16U);
    ee_buff[13] = (thermostat_flags >> 8U);
    ee_buff[14] = (thermostat_flags & 0xFFU);
    ee_buff[15] = 0U;
    BSP_EEPROM_WriteBuffer(ee_buff, EE_THERMOSTAT_MAX_COOLING_TEMP, 16U);
    BSP_EEPROM_WaitEepromStandbyState();
    
    ee_buff[0] = thermostat_operation_mode;
    ee_buff[1] = thermostat_max_setpoint_temperature;
    ee_buff[2] = thermostat_min_setpoint_temperature;
    ee_buff[3] = fan_speed_treshold;
    ee_buff[4] = fan_low_speed_band;
    ee_buff[5] = fan_middle_speed_band;
    ee_buff[6] = onewire_interface_address;
    ee_buff[7] = (rs485_interface_address >> 8U);
    ee_buff[8] = (rs485_interface_address & 0xFFU);
    ee_buff[9] = fan_control_type;
    ee_buff[10] = freezing_protection_temperature;
    ee_buff[11] = fan_night_quiet_mode_start_hour;
    ee_buff[13] = temperature_setpoint;
    ee_buff[13] = 0U;
    ee_buff[14] = 0U;
    ee_buff[15] = 0U;
    BSP_EEPROM_WriteBuffer(ee_buff, EE_THERMOSTAT_OPERATION_MODE, 16U);
    BSP_EEPROM_WaitEepromStandbyState();
}


/*************************** End of file ****************************/
