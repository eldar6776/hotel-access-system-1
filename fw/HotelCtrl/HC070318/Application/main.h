
#ifndef __MAIN_H__
#define __MAIN_H__

#include "stm32f4xx.h"

//0x8008000
//0xE80000
#define VECT_TAB_OFFSET  0x10000 


//#define DEST_IP_ADDR0   192
//#define DEST_IP_ADDR1   168
//#define DEST_IP_ADDR2   1
//#define DEST_IP_ADDR3   11

//#define UDP_SERVER_PORT  10

#define MAC_ADDR0   2
#define MAC_ADDR1   1
#define MAC_ADDR2   2
#define MAC_ADDR3   0
#define MAC_ADDR4   0
#define MAC_ADDR5   0
 
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   20
#define IP_ADDR3   199
   
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   20
#define GW_ADDR3   1  

#define RMII_MODE

#define BootloaderActivate()					((system_config &= 0x0fffffff),(system_config |= 0x80000000))
#define BootloaderDeactivate()					(system_config &= 0x0fffffff)
#define IsBootloaderRequested()					(system_config & 0x80000000)
#define BootloaderSDcardError()					((system_config &= 0x0fffffff),(system_config |= 0x40000000))
#define IsBootloaderSDcardError()				(system_config & 0x40000000)
#define BootloaderUpdateSucces()				((system_config &= 0x0fffffff),(system_config |= 0x20000000))
#define IsBootloaderUpdateSucces()				(system_config & 0x20000000)
#define BootloaderFileError()					((system_config &= 0x0fffffff),(system_config |= 0x10000000))
#define IsBootloaderFileError()					(system_config & 0x10000000)
#define HTTP_ServerEnable()						(system_config |= 0x00000100)	
#define HTTP_ServerDisable()					(system_config &= 0xfffffeff)
#define IsHTTP_ServerEnabled()					(system_config & 0x00000100)	
#define WEB_ConfigEnable()						(system_config |= 0x00000200)
#define WEB_ConfigDisable()						(system_config &= 0xfffffdff)
#define IsWEB_ConfigEnabled()					(system_config & 0x00000200)
#define TFTP_ServerEnable()						(system_config |= 0x00000400)	
#define TFTP_ServerDisable()					(system_config &= 0xfffffbff)
#define IsTFTP_ServerEnabled()					(system_config & 0x00000400)
#define TIME_BroadcastEnable()					(system_config |= 0x00000800)	
#define TIME_BroadcastDisable()					(system_config &= 0xfffff7ff)
#define IsTIME_BroadcastEnabled()				(system_config & 0x00000800)
#define DHCP_ClientEnable()						(system_config |= 0x00001000)	
#define DHCP_ClientDisable()					(system_config &= 0xffffefff)
#define IsDHCP_ClientEnabled()					(system_config & 0x00001000)

extern volatile uint32_t SystickCnt;
extern volatile uint32_t delay_tm;
extern uint32_t system_config;
uint32_t get_systick(void);
#endif 
