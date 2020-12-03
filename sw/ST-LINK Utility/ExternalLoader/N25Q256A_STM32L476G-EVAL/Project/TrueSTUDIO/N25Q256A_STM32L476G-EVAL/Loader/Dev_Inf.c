#include "Dev_Inf.h"

/* This structure containes information used by ST-LINK Utility to program and erase the device */
#if defined (__ICCARM__)
__root struct StorageInfo const StorageInfo  =  {
#else
struct StorageInfo const StorageInfo  =  {
#endif
   "N25Q256A_STM32l4xx-EVAL",
   NOR_FLASH,
   0x90000000,
   0x2000000,
   0x100,
   0xFF,
   // Specify Size and Address of Sectors (view example below)
   0x00000200, 0x00010000,     				 // Sector Num : 512 ,Sector Size: 64KBytes 
   0x00000000, 0x00000000,
}; 

/*  								Sector coding example
	A device with succives 16 Sectors of 1KBytes, 128 Sectors of 16 KBytes, 
	8 Sectors of 2KBytes and 16384 Sectors of 8KBytes
	
	0x00000010, 0x00000400,     							// 16 Sectors of 1KBytes
	0x00000080, 0x00004000,     							// 128 Sectors of 16 KBytes
	0x00000008, 0x00000800,     							// 8 Sectors of 2KBytes
	0x00004000, 0x00002000,     							// 16384 Sectors of 8KBytes
	0x00000000, 0x00000000,								// end
   */

