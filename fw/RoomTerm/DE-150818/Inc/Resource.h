/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2017  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.42 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only  be used  in accordance  with  a license  and should  not be  re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : Resource.h
Content     : Main resource header file of weather forecast demo
---------------------------END-OF-HEADER------------------------------
*/

#ifndef RESOURCE_H
#define RESOURCE_H

#include "GUI.h"

#ifndef GUI_CONST_STORAGE
  #define GUI_CONST_STORAGE const
#endif
//
//	Sound
//

//
// 	Font
//

//
// 	Image
#define QSPI_FLASH_DISPLAY_IMAGE_00_ADDRESS         ".ARM.__at_0x90000000"      // display image size  0x0003fc00 
#define QSPI_FLASH_DISPLAY_IMAGE_01_ADDRESS         ".ARM.__at_0x9003fc00"      // display image size  0x0003fc00 
#define QSPI_FLASH_DISPLAY_IMAGE_02_ADDRESS         ".ARM.__at_0x9007f800"      // display image size  0x0003fc00 
#define QSPI_FLASH_DISPLAY_IMAGE_03_ADDRESS         ".ARM.__at_0x900bf400"      // display image size  0x0003fc00 
#define QSPI_FLASH_DISPLAY_IMAGE_04_ADDRESS         ".ARM.__at_0x900ff000"      // display image size  0x0003fc00 
#define QSPI_FLASH_DISPLAY_IMAGE_05_ADDRESS         ".ARM.__at_0x9013ec00"      // display image size  0x0003fc00 
#define QSPI_FLASH_DISPLAY_IMAGE_11_ADDRESS         ".ARM.__at_0x9017e800"      // display image size  0x0003fc00 
#define QSPI_FLASH_DISPLAY_IMAGE_12_ADDRESS         ".ARM.__at_0x901be400"      // display image size  0x0003fc00
#define QSPI_FLASH_DISPLAY_IMAGE_13_ADDRESS         ".ARM.__at_0x901fe000"      // display image size  0x0003fc00
#define QSPI_FLASH_DISPLAY_IMAGE_14_ADDRESS         ".ARM.__at_0x9023dc00"      // display image size  0x0003fc00
#define QSPI_FLASH_DISPLAY_IMAGE_15_ADDRESS         ".ARM.__at_0x9027d800"      // display image size  0x0003fc00
#define QSPI_FLASH_DISPLAY_IMAGE_21_ADDRESS         ".ARM.__at_0x902bd400"      // display image size  0x0003fc00
#define QSPI_FLASH_DISPLAY_IMAGE_22_ADDRESS         ".ARM.__at_0x902fd000"      // display image size  0x0003fc00 
#define QSPI_FLASH_DISPLAY_IMAGE_23_ADDRESS         ".ARM.__at_0x9033cc00"      // display image size  0x0003fc00 
#define QSPI_FLASH_DISPLAY_IMAGE_24_ADDRESS         ".ARM.__at_0x9037c800"      // display image size  0x0003fc00 
#define QSPI_FLASH_DISPLAY_IMAGE_25_ADDRESS         ".ARM.__at_0x903bc400"      // display image size  0x0003fc00 
#define QSPI_FLASH_DISPLAY_IMAGE_31_ADDRESS         ".ARM.__at_0x903fc000"      // display image size  0x0003fc00 
#define QSPI_FLASH_DISPLAY_IMAGE_32_ADDRESS         ".ARM.__at_0x9043bc00"      // display image size  0x0003fc00 
#define QSPI_FLASH_DISPLAY_IMAGE_33_ADDRESS         ".ARM.__at_0x9047b800"      // display image size  0x0003fc00 
#define QSPI_FLASH_DISPLAY_IMAGE_34_ADDRESS         ".ARM.__at_0x904bb400"      // display image size  0x0003fc00 
#define QSPI_FLASH_DISPLAY_IMAGE_35_ADDRESS         ".ARM.__at_0x904fb000"      // display image size  0x0003fc00 
#define QSPI_FLASH_DISPLAY_IMAGE_41_ADDRESS         ".ARM.__at_0x9053ac00"      // display image size  0x0003fc00 
#define QSPI_FLASH_DISPLAY_IMAGE_42_ADDRESS         ".ARM.__at_0x9057a800"      // display image size  0x0003fc00 
#define QSPI_FLASH_DISPLAY_IMAGE_43_ADDRESS         ".ARM.__at_0x905ba400"      // display image size  0x0003fc00 
#define QSPI_FLASH_DISPLAY_IMAGE_44_ADDRESS         ".ARM.__at_0x905fa000"      // display image size  0x0003fc00
#define QSPI_FLASH_DISPLAY_IMAGE_45_ADDRESS         ".ARM.__at_0x90639c00"      // display image size  0x0003fc00
#define QSPI_FLASH_DISPLAY_IMAGE_51_ADDRESS         ".ARM.__at_0x90679800"      // display image size  0x0003fc00
#define QSPI_FLASH_DISPLAY_IMAGE_52_ADDRESS         ".ARM.__at_0x906b9400"      // display image size  0x0003fc00
#define QSPI_FLASH_DISPLAY_IMAGE_53_ADDRESS         ".ARM.__at_0x906f9000"      // display image size  0x0003fc00
#define QSPI_FLASH_DISPLAY_IMAGE_54_ADDRESS         ".ARM.__at_0x90738c00"      // display image size  0x0003fc00
#define QSPI_FLASH_DISPLAY_IMAGE_55_ADDRESS         ".ARM.__at_0x90778800"      // display image size  0x0003fc00
#define QSPI_FLASH_DISPLAY_IMAGE_61_ADDRESS         ".ARM.__at_0x907b8400"      // display image size  0x0003fc00
#define QSPI_FLASH_DISPLAY_IMAGE_62_ADDRESS         ".ARM.__at_0x907f8000"      // display image size  0x0003fc00
#define QSPI_FLASH_DISPLAY_IMAGE_63_ADDRESS         ".ARM.__at_0x90837c00"      // display image size  0x0003fc00
#define QSPI_FLASH_DISPLAY_IMAGE_64_ADDRESS         ".ARM.__at_0x90877800"      // display image size  0x0003fc00
#define QSPI_FLASH_DISPLAY_IMAGE_65_ADDRESS         ".ARM.__at_0x908b7400"      // display image size  0x0003fc00
#define QSPI_FLASH_DISPLAY_IMAGE_71_ADDRESS         ".ARM.__at_0x908f7000"      // display image size  0x0003fc00
#define QSPI_FLASH_DISPLAY_IMAGE_72_ADDRESS         ".ARM.__at_0x90936c00"      // display image size  0x0003fc00
#define QSPI_FLASH_DISPLAY_IMAGE_73_ADDRESS         ".ARM.__at_0x90976800"      // display image size  0x0003fc00
#define QSPI_FLASH_DISPLAY_IMAGE_74_ADDRESS         ".ARM.__at_0x909b6400"      // display image size  0x0003fc00
#define QSPI_FLASH_DISPLAY_IMAGE_75_ADDRESS         ".ARM.__at_0x909f6000"      // display image size  0x0003fc00
#define QSPI_FLASH_DISPLAY_IMAGE_81_ADDRESS         ".ARM.__at_0x90a35c00"      // display image size  0x0003fc00
#define QSPI_FLASH_DISPLAY_IMAGE_82_ADDRESS         ".ARM.__at_0x90a75800"      // display image size  0x0003fc00 
#define QSPI_FLASH_DISPLAY_IMAGE_83_ADDRESS         ".ARM.__at_0x90ab5400"      // display image size  0x0003fc00
#define QSPI_FLASH_DISPLAY_IMAGE_84_ADDRESS         ".ARM.__at_0x90af5000"      // display image size  0x0003fc00
#define QSPI_FLASH_DISPLAY_IMAGE_85_ADDRESS         ".ARM.__at_0x90b34c00"      // display image size  0x0003fc00

#define QSPI_FLASH_DISPLAY_ERROR_IMAGE_01_ADDRESS   ".ARM.__at_0x90b74800"      // display image size  0x0003fc00

#define QSPI_FLASH_DISPLAY_IMAGE_NUMBER_0_ADDRESS   ".ARM.__at_0x90bb4400"      // display image size  0x000019ac
#define QSPI_FLASH_DISPLAY_IMAGE_NUMBER_1_ADDRESS   ".ARM.__at_0x90bb5dac"      // display image size  0x000019ac
#define QSPI_FLASH_DISPLAY_IMAGE_NUMBER_2_ADDRESS   ".ARM.__at_0x90bb7758"      // display image size  0x000019ac
#define QSPI_FLASH_DISPLAY_IMAGE_NUMBER_3_ADDRESS   ".ARM.__at_0x90bb9104"      // display image size  0x000019ac
#define QSPI_FLASH_DISPLAY_IMAGE_NUMBER_4_ADDRESS   ".ARM.__at_0x90bbaab0"      // display image size  0x000019ac
#define QSPI_FLASH_DISPLAY_IMAGE_NUMBER_5_ADDRESS   ".ARM.__at_0x90bbc45c"      // display image size  0x000019ac
#define QSPI_FLASH_DISPLAY_IMAGE_NUMBER_6_ADDRESS   ".ARM.__at_0x90bbde08"      // display image size  0x000019ac
#define QSPI_FLASH_DISPLAY_IMAGE_NUMBER_7_ADDRESS   ".ARM.__at_0x90bbf7b4"      // display image size  0x000019ac
#define QSPI_FLASH_DISPLAY_IMAGE_NUMBER_8_ADDRESS   ".ARM.__at_0x90bc1160"      // display image size  0x000019ac
#define QSPI_FLASH_DISPLAY_IMAGE_NUMBER_9_ADDRESS   ".ARM.__at_0x90bc2b0c"      // display image size  0x000019ac

#define QSPI_FLASH_BUTTON_DND_0_IMAGE_ADDRESS       ".ARM.__at_0x90bc44b8"      // display image size  0x00005dc0
#define QSPI_FLASH_BUTTON_DND_1_IMAGE_ADDRESS       ".ARM.__at_0x90bca278"      // display image size  0x00005dc0
#define QSPI_FLASH_BUTTON_MAID_0_IMAGE_ADDRESS      ".ARM.__at_0x90bd0038"      // display image size  0x00005dc0
#define QSPI_FLASH_BUTTON_MAID_1_IMAGE_ADDRESS      ".ARM.__at_0x90bd5df8"      // display image size  0x00005dc0
#define QSPI_FLASH_BUTTON_RST_SOS_0_IMAGE_ADDRESS   ".ARM.__at_0x90bdbbb8"      // display image size  0x00005dc0
#define QSPI_FLASH_BUTTON_RST_SOS_1_IMAGE_ADDRESS   ".ARM.__at_0x90be1978"      // display image size  0x00005dc0
#define QSPI_FLASH_BUTTON_DOOR_OPEN_IMAGE_ADDRESS   ".ARM.__at_0x90be7738"      // display image size  0x00007918
#define QSPI_FLASH_BUTTON_OK_IMAGE_ADDRESS          ".ARM.__at_0x90bef050"      // display image size  0x0000724c

#define QSPI_FLASH_ENG_FLAG_IMAGE_ADDRESS           ".ARM.__at_0x90bf629c"      // display image size  0x00006400
#define QSPI_FLASH_GER_FLAG_IMAGE_ADDRESS           ".ARM.__at_0x90bfc69c"      // display image size  0x00006400
#define QSPI_FLASH_FRA_FLAG_IMAGE_ADDRESS           ".ARM.__at_0x90c02a9c"      // display image size  0x00006400
#define QSPI_FLASH_KSA_FLAG_IMAGE_ADDRESS           ".ARM.__at_0x90c08e9c"      // display image size  0x00006400
#define QSPI_FLASH_CHN_FLAG_IMAGE_ADDRESS           ".ARM.__at_0x90c0f29c"      // display image size  0x00006400
#define QSPI_FLASH_JAP_FLAG_IMAGE_ADDRESS           ".ARM.__at_0x90c1569c"      // display image size  0x00006400
#define QSPI_FLASH_ITA_FLAG_IMAGE_ADDRESS           ".ARM.__at_0x90c1ba9c"      // display image size  0x00006400
#define QSPI_FLASH_TRK_FLAG_IMAGE_ADDRESS           ".ARM.__at_0x90c21e9c"      // display image size  0x00006400
#define QSPI_FLASH_SLO_FLAG_IMAGE_ADDRESS           ".ARM.__at_0x90c2829c"      // display image size  0x00006400

#define QSPI_FLASH_NEW_FIRMWARE_ADRESS              ".ARM.__at_0x90f00000"      // new firmware size   0x00100000
//

extern GUI_CONST_STORAGE GUI_BITMAP bm_display_00;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_01;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_02;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_03;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_04;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_05;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_11;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_12;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_13;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_14;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_15;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_21;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_22;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_23;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_24;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_25;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_31;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_32;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_33;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_34;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_35;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_41;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_42;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_43;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_44;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_45;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_51;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_52;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_53;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_54;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_55;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_61;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_62;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_63;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_64;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_65;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_71;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_72;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_73;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_73;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_74;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_75;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_81;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_82;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_83;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_84;
extern GUI_CONST_STORAGE GUI_BITMAP bm_display_85;

extern GUI_CONST_STORAGE GUI_BITMAP bm_display_error_01;

extern GUI_CONST_STORAGE GUI_BITMAP bm_number_0;
extern GUI_CONST_STORAGE GUI_BITMAP bm_number_1;
extern GUI_CONST_STORAGE GUI_BITMAP bm_number_2;
extern GUI_CONST_STORAGE GUI_BITMAP bm_number_3;
extern GUI_CONST_STORAGE GUI_BITMAP bm_number_4;
extern GUI_CONST_STORAGE GUI_BITMAP bm_number_5;
extern GUI_CONST_STORAGE GUI_BITMAP bm_number_6;
extern GUI_CONST_STORAGE GUI_BITMAP bm_number_7;
extern GUI_CONST_STORAGE GUI_BITMAP bm_number_8;
extern GUI_CONST_STORAGE GUI_BITMAP bm_number_9;


extern GUI_CONST_STORAGE GUI_BITMAP bm_btn_dnd_0;
extern GUI_CONST_STORAGE GUI_BITMAP bm_btn_dnd_1;
extern GUI_CONST_STORAGE GUI_BITMAP bm_btn_maid_0;
extern GUI_CONST_STORAGE GUI_BITMAP bm_btn_maid_1;
extern GUI_CONST_STORAGE GUI_BITMAP bm_btn_rst_sos_0;
extern GUI_CONST_STORAGE GUI_BITMAP bm_btn_rst_sos_1;
extern GUI_CONST_STORAGE GUI_BITMAP bm_btn_door_open;
extern GUI_CONST_STORAGE GUI_BITMAP bm_btn_ok;

extern GUI_CONST_STORAGE GUI_BITMAP bm_eng_flag;
extern GUI_CONST_STORAGE GUI_BITMAP bm_de_flag;
extern GUI_CONST_STORAGE GUI_BITMAP bm_fr_flag;
extern GUI_CONST_STORAGE GUI_BITMAP bm_ksa_flag;
extern GUI_CONST_STORAGE GUI_BITMAP bm_chn_flag;
extern GUI_CONST_STORAGE GUI_BITMAP bm_jap_flag;
extern GUI_CONST_STORAGE GUI_BITMAP bm_ita_flag;
extern GUI_CONST_STORAGE GUI_BITMAP bm_trk_flag;
extern GUI_CONST_STORAGE GUI_BITMAP bm_slo_flag;

//eng
//ger
//fra
//arab
//china
//jap
//ita
//tur
//slov
//
//	Animation
//
#endif // RESOURCE_H

/*************************** End of file ****************************/
