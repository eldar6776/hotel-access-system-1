/**
 ******************************************************************************
 * File Name          : mifare.h
 * Date               : 28/02/2016 23:16:19
 * Description        : mifare RC522 software modul
 ******************************************************************************
 *
 *
 *
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "mifare.h"

/* Variables  ----------------------------------------------------------------*/
uint8_t mifare_rx_buffer[FIFO_SIZE];
uint8_t mifare_tx_buffer[FIFO_SIZE];
/* Defines    ----------------------------------------------------------------*/
/* Objects    ----------------------------------------------------------------*/
/* Code       ----------------------------------------------------------------*/
void MifareInit(void){

    MIFARE_ResetOn();
    //delay 10 ms
    MIFARE_ResetOff();
    //delay 50 ms
    
    // When communicating with a PICC we need a timeout if something goes wrong.
    // f_timer = 13.56 MHz / (2*TPreScaler+1) where TPreScaler = [TPrescaler_Hi:TPrescaler_Lo].
    // TPrescaler_Hi are the four low bits in TModeReg. TPrescaler_Lo is TPrescalerReg.
    MIFARE_WriteByteToRegister(TModeReg, 0x80);         // TAuto=1; timer starts automatically at the end of the transmission in all communication modes at all speeds
    MIFARE_WriteByteToRegister(TPrescalerReg, 0xA9);    // TPreScaler = TModeReg[3..0]:TPrescalerReg, ie 0x0A9 = 169 => f_timer=40kHz, ie a timer period of 25us.
    MIFARE_WriteByteToRegister(TReloadRegH, 0x03);      // Reload timer with 0x3E8 = 1000, ie 25ms before timeout.
    MIFARE_WriteByteToRegister(TReloadRegL, 0xE8);
    MIFARE_WriteByteToRegister(TxASKReg, 0x40);         // Default 0x00. Force a 100 % ASK modulation independent of the ModGsPReg register setting
    MIFARE_WriteByteToRegister(ModeReg, 0x3D);          // Default 0x3F. Set the preset value for the CRC coprocessor for the CalcCRC command to 0x6363 (ISO 14443-3 part 6.2.4)
 
    MIFARE_WriteByteToRegister(RFCfgReg, (0x07<<4));    // Set Rx Gain to max
 
    MIFARE_AntennaOn();                        // Enable the antenna driver pins TX1 and TX2 (they were disabled by the reset)

    
}// End of mifare init
/**
 * Writes a byte to the specified register in the MFRC522 chip.
 * The interface is described in the datasheet section 8.1.2.
 */
void MIFARE_WriteByteToRegister(uint8_t reg, uint8_t value){

    MIFARE_Select();
    mifare_tx_buffer[0] = reg & 0x7e;
    mifare_tx_buffer[1] = value;
    HAL_SPI_Transmit(&hspi2, &mifare_tx_buffer[0] , 2, 10);
    MIFARE_Unselect();
    
}// End of mifare write register
/**
 * Writes a number of bytes to the specified register in the MFRC522 chip.
 * The interface is described in the datasheet section 8.1.2.
 */
void MIFARE_WriteStringToRegister(uint8_t reg, uint8_t count, uint8_t *values) {
  
    uint8_t index = 1;
    uint8_t cnt = count;
    
    if(count > sizeof (mifare_tx_buffer)) return;
    
    MIFARE_Select(); /* Select SPI Chip MFRC522 */
    // MSB == 0 is for writing. LSB is not used in address. Datasheet section 8.1.2.3.
    mifare_tx_buffer[0] = reg & 0x7e;
    
    while(cnt--) {
        
        mifare_tx_buffer[index++] = *values++;
    
    }// End of while..
 
    HAL_SPI_Transmit(&hspi2, &mifare_tx_buffer[0] , (count + 1), 10);
    
    MIFARE_Unselect(); /* Release SPI Chip MFRC522 */
    
}// End of write string to mifare register

uint8_t MIFARE_ReadByteFromRegister(uint8_t reg){
    
    mifare_tx_buffer[0] = reg | 0x80;
    mifare_tx_buffer[1] = 0;
    MIFARE_Select();
    HAL_SPI_TransmitReceive(&hspi2, &mifare_tx_buffer[0], &mifare_rx_buffer[0], 2U, 10U); 
    MIFARE_Unselect();
    
    return (mifare_rx_buffer[1]);

}// End of read byte rom register

void MIFARE_ReadeStringFromRegister(uint8_t reg, uint8_t count, uint8_t *values, uint8_t rxAlign){
 
    if (count == 0) { return; }
 
    mifare_tx_buffer[0] = reg | 0x80;                           // MSB == 1 is for reading. LSB is not used in address. Datasheet section 8.1.2.3.
    uint8_t index = 0;                                          // Index in values array.

    MIFARE_Select();                                            /* Select SPI Chip MFRC522 */
    count--;                                                    // One read is performed outside of the loop
    HAL_SPI_Transmit(&hspi2, &mifare_tx_buffer[0] , 1, 10);     // Tell MFRC522 which address we want to read
 
    while (index < count)
    {
        if ((index == 0) && rxAlign) // Only update bit positions rxAlign..7 in values[0]
        {
            // Create bit mask for bit positions rxAlign..7
            uint8_t mask = 0;
            for (uint8_t i = rxAlign; i <= 7; i++)
            {
                mask |= (1 << i);
            }
 
            // Read value and tell that we want to read the same address again.
            HAL_SPI_TransmitReceive(&hspi2, &mifare_tx_buffer[0], &mifare_rx_buffer[0], 1, 10);
            uint8_t value = mifare_rx_buffer[0];
            // Apply mask to both current value of values[0] and the new data in value.
            values[0] = (values[index] & ~mask) | (value & mask);
        }
        else
        {
            // Read value and tell that we want to read the same address again.
            mifare_tx_buffer[0] = reg | 0x80;
            HAL_SPI_TransmitReceive(&hspi2, &mifare_tx_buffer[0], &mifare_rx_buffer[0], 1, 10);
            values[index] = mifare_rx_buffer[0];
        }
 
        index++;
    }
    mifare_tx_buffer[0] = 0;
    HAL_SPI_TransmitReceive(&hspi2, &mifare_tx_buffer[0], &mifare_rx_buffer[0], 1, 10);
    values[index] = mifare_rx_buffer[0]; // Read the final byte. Send 0 to stop reading.
 
    MIFARE_Unselect();                       /* Release SPI Chip MFRC522 */
    
}// End of read string from mifare register
 
/**
 * Sets the bits given in mask in register reg.
 */
void MIFARE_SetRegisterBits(uint8_t reg, uint8_t mask){
    
    uint8_t tmp = MIFARE_ReadByteFromRegister(reg);
    MIFARE_WriteByteToRegister(reg, tmp | mask);     // set bit mask
    
}// End of set mifare register bits

void MIFARE_ClearRegisterBits(uint8_t reg, uint8_t mask){

    uint8_t tmp = MIFARE_ReadByteFromRegister(reg);
    MIFARE_WriteByteToRegister(reg, tmp & (~mask));    // clear bit mask
    
}// End of clear mifare register bits


/**
 * Use the CRC coprocessor in the MFRC522 to calculate a CRC_A.
 */
uint8_t MIFARE_CalculateCRC(uint8_t *data, uint8_t length, uint8_t *result)
{
    MIFARE_WriteByteToRegister(CommandReg, PCD_Idle);         // Stop any active command.
    MIFARE_WriteByteToRegister(DivIrqReg, 0x04);              // Clear the CRCIRq interrupt request bit
    MIFARE_SetRegisterBits(FIFOLevelReg, 0x80);               // FlushBuffer = 1, FIFO initialization
    MIFARE_WriteStringToRegister(FIFODataReg, length, data);  // Write data to the FIFO
    MIFARE_WriteByteToRegister(CommandReg, PCD_CalcCRC);      // Start the calculation
 
    // Wait for the CRC calculation to complete. Each iteration of the while-loop takes 17.73us.
    uint16_t i = 5000;
    uint8_t n;
    while (1)
    {
        n = MIFARE_ReadByteFromRegister(DivIrqReg);  // DivIrqReg[7..0] bits are: Set2 reserved reserved MfinActIRq   reserved CRCIRq reserved reserved
        if (n & 0x04)
        {
            // CRCIRq bit set - calculation done
            break;
        }
    
        if (--i == 0)
        {
            // The emergency break. We will eventually terminate on this one after 89ms.
            // Communication with the MFRC522 might be down.
            return STATUS_TIMEOUT;
        }
    }
 
    // Stop calculating CRC for new content in the FIFO.
    MIFARE_WriteByteToRegister(CommandReg, PCD_Idle);
 
    // Transfer the result from the registers to the result buffer
    result[0] = MIFARE_ReadByteFromRegister(CRCResultRegL);
    result[1] = MIFARE_ReadByteFromRegister(CRCResultRegH);
  
    return STATUS_OK;
  
}// End of calculate mifare CRC

void MIFARE_AntennaOn()
{
    uint8_t value = MIFARE_ReadByteFromRegister(TxControlReg);
    if ((value & 0x03) != 0x03)
    {
        MIFARE_WriteByteToRegister(TxControlReg, value | 0x03);
    }
}// End of mifare antena on

void MIFARE_Reset()
{
    MIFARE_WriteByteToRegister(CommandReg, PCD_SoftReset); // Issue the SoftReset command.
    // The datasheet does not mention how long the SoftRest command takes to complete.
    // But the MFRC522 might have been in soft power-down mode (triggered by bit 4 of CommandReg)
    // Section 8.8.2 in the datasheet says the oscillator start-up time is the start up time of the crystal + 37,74us. Let us be generous: 50ms.
  
    //wait_ms(50);
 
    // Wait for the PowerDown bit in CommandReg to be cleared
    while (MIFARE_ReadByteFromRegister(CommandReg) & (1<<4))
    {
        // PCD still restarting - unlikely after waiting 50ms, but better safe than sorry.
    }
}// End of mifare reset
/******************************   END OF FILE  **********************************/
