/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

// Includes
#include "I2C.h"
#include "I2C_Master.h"



/*
 * Definition of function that reads one byte from a device's register 
 * through I2C protcol. The parameters needed are:
 * - adress of the device
 * - adress of the register we want to read
 * - pointer where to save the read data
 *
*/
void I2C_Peripheral_ReadRegister(uint8_t device_address, 
                                 uint8_t register_address,
                                 uint8_t* data) {
                                    
    // Send start condition
    uint8_t error = I2C_Master_MasterSendStart(device_address,I2C_Master_WRITE_XFER_MODE);
    if (error == I2C_Master_MSTR_NO_ERROR) {
    
        // Write address of register to be read
        error = I2C_Master_MasterWriteByte(register_address);
        if (error == I2C_Master_MSTR_NO_ERROR) {
            // Send restart condition
            error = I2C_Master_MasterSendRestart(device_address, I2C_Master_READ_XFER_MODE);
            if (error == I2C_Master_MSTR_NO_ERROR) {
                // Read data without acknowledgement
                *data = I2C_Master_MasterReadByte(I2C_Master_NAK_DATA);
            }
        }
        
    }
    
    // Send stop condition
    I2C_Master_MasterSendStop();

}
                                
                                
/*
 * Definition of function that reads multiple device's registers
 * through I2C protcol. The parameters needed are:
 * - adress of the device
 * - adress of the register we want to read
 * - # registers we want to read
 * - pointer where to save the read data
 *
*/                               
void I2C_Peripheral_ReadRegisterMulti(uint8_t device_address,
                                      uint8_t register_address,
                                      uint8_t register_count,
                                      uint8_t* data) {
                                        
    // Send start condition
    uint8_t error = I2C_Master_MasterSendStart(device_address,I2C_Master_WRITE_XFER_MODE);
    if (error == I2C_Master_MSTR_NO_ERROR) {

        // Write address of register to be read with the MSb equal to 1
        register_address |= 0x80; // Datasheet indication for multi read -- autoincrement
        error = I2C_Master_MasterWriteByte(register_address);
        if (error == I2C_Master_MSTR_NO_ERROR) {

            // Send restart condition
            error = I2C_Master_MasterSendRestart(device_address, I2C_Master_READ_XFER_MODE);
            if (error == I2C_Master_MSTR_NO_ERROR) {

                // Continue reading until we have register to read
                uint8_t counter = register_count;
                while(counter>1) {
                    data[register_count-counter] =
                        I2C_Master_MasterReadByte(I2C_Master_ACK_DATA);
                    counter--;
                }
                
                // Read last data without acknowledgement
                data[register_count-1]
                    = I2C_Master_MasterReadByte(I2C_Master_NAK_DATA);
                    
            }
            
        }
        
    }
    
    // Send stop condition
    I2C_Master_MasterSendStop();

}


/*
 * Definition of function that writes a byte to a device's register
 * through I2C protcol. The parameters needed are:
 * - adress of the device
 * - adress of the register we want to read
 * - data to be written
 *
*/
void I2C_Peripheral_WriteRegister(uint8_t device_address,
                                  uint8_t register_address,
                                  uint8_t data) {
                                    
    // Send start condition
    uint8_t error = I2C_Master_MasterSendStart(device_address, I2C_Master_WRITE_XFER_MODE);
    if (error == I2C_Master_MSTR_NO_ERROR) {
    
        // Write register address
        error = I2C_Master_MasterWriteByte(register_address);
        if (error == I2C_Master_MSTR_NO_ERROR) {
            // Write byte of interest
            error = I2C_Master_MasterWriteByte(data);
        }
        
    }
    
    // Send stop condition
    I2C_Master_MasterSendStop();

}
                                

/* [] END OF FILE */
