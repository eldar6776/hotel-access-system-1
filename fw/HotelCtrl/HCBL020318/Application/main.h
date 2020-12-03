
#ifndef __MAIN_H__
#define __MAIN_H__

#include "stm32f4xx.h"

//#define WRITE_DEFAULT

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

#define BootloaderActivate()					((system_config &= 0x0f),(system_config |= 0x80))
#define BootloaderDeactivate()					(system_config &= 0x0f)
#define IsBootloaderRequested()					(system_config & 0x80)
#define BootloaderSDcardError()					((system_config &= 0x0f),(system_config |= 0x40))
#define BootloaderUpdateSucces()				((system_config &= 0x0f),(system_config |= 0x20))
#define BootloaderFileError()					((system_config &= 0x0f),(system_config |= 0x10))

#define RMII_MODE
extern __IO uint32_t TimingDelay;
extern uint32_t system_config;

uint32_t get_systick(void);
void Fail_Handler(void);
void ApplicationExe(void);
void Delay(__IO uint32_t nTime);

#endif 
