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
 * Definition of function that searches for connected devices on the I2C bus.
 * As only parameter it requires a device adress (will be incremental for a scan operation)
*/
uint8_t I2C_Peripheral_IsDeviceConnected(uint8_t device_address) {

    // Send a start condition followed by a stop condition
    uint8_t temp = I2C_Master_MasterSendStart(device_address, I2C_Master_WRITE_XFER_MODE);
    I2C_Master_MasterSendStop();
    
    // If no error is generated, a device is connected
    if (temp == I2C_Master_MSTR_NO_ERROR) {
        return DEVICE_CONNECTED;
    }
    
    return DEVICE_UNCONNECTED;
    
} // end I2C_Peripheral_IsDeviceConnected


/*
 * Definition of function that reads one byte from a device's register 
 * through I2C protcol. The parameters needed are:
 * - adress of the device
 * - adress of the register we want to read
 * - pointer where to save the read data
*/
void I2C_Peripheral_ReadRegister(uint8_t device_address, 
                                 uint8_t register_address,
                                 uint8_t* data) {
                                    
    // Start condition
    
    // Send start condition to the target device                               
    uint8_t temp = I2C_Master_MasterSendStart(device_address,I2C_Master_WRITE_XFER_MODE);
    
    if (temp == I2C_Master_MSTR_NO_ERROR) {
        // Communicate register's address
        temp = I2C_Master_MasterWriteByte(register_address);
        
        if (temp == I2C_Master_MSTR_NO_ERROR) {
            // Send restart condition
            temp = I2C_Master_MasterSendRestart(device_address, I2C_Master_READ_XFER_MODE);
            
            if (temp == I2C_Master_MSTR_NO_ERROR) {
                // Read data and notify slave that transfer is completed
                *data = I2C_Master_MasterReadByte(I2C_Master_NAK_DATA);
                
            } // end read
        } // end restart
    } // end start
    
    // Send stop condition
    I2C_Master_MasterSendStop();

} // end I2C_Peripheral_ReadRegister
                                
                                
/*
 * Definition of function that reads multiple device's registers
 * through I2C protcol. The parameters needed are:
 * - adress of the device
 * - adress of the register we want to read
 * - # registers we want to read
 * - pointer where to save the read data
*/                               
void I2C_Peripheral_ReadRegisterMulti(uint8_t device_address,
                                      uint8_t register_address,
                                      uint8_t register_count,
                                      uint8_t* data) {
                                        
    // Start condition
                                        
    // Send start condition to the target device                                     
    uint8_t temp = I2C_Master_MasterSendStart(device_address,I2C_Master_WRITE_XFER_MODE);
    
    if (temp == I2C_Master_MSTR_NO_ERROR) {
        // Communicate register's adress with the MSb equal to 1 to allow autoincrement
        // for multiple data read (as indicated in the datasheet)
        register_address |= 0x80; // In this way we set the MSb to 1, without 
                                  // other modifications on the adress
        temp = I2C_Master_MasterWriteByte(register_address);
        
        if (temp == I2C_Master_MSTR_NO_ERROR) {
            // Send restart condition
            temp = I2C_Master_MasterSendRestart(device_address, I2C_Master_READ_XFER_MODE);
            
            if (temp == I2C_Master_MSTR_NO_ERROR) {
                // Read as long as we have registers to read
                uint8_t counter = register_count;
                
                while(counter>1) {
                    // Read data and notify slave that transfer continues
                    data[register_count-counter] = I2C_Master_MasterReadByte(I2C_Master_ACK_DATA);
                    counter--;
                }
                
                // Read last data and notify slave that transfer is completed
                data[register_count-1] = I2C_Master_MasterReadByte(I2C_Master_NAK_DATA);
                    
            } // end read           
        } // end restart        
    } // end start
    
    // Send stop condition
    I2C_Master_MasterSendStop();

} // end I2C_Peripheral_ReadRegisterMulti


/*
 * Definition of function that writes a byte to a device's register
 * through I2C protcol. The parameters needed are:
 * - adress of the device
 * - adress of the register we want to read
 * - data to be written
*/
void I2C_Peripheral_WriteRegister(uint8_t device_address,
                                  uint8_t register_address,
                                  uint8_t data) {
                                    
    // Start condition
                                    
    // Send start condition to the target device                                
    uint8_t temp = I2C_Master_MasterSendStart(device_address, I2C_Master_WRITE_XFER_MODE);
    
    if (temp == I2C_Master_MSTR_NO_ERROR) {
        // Communicate register's address
        temp = I2C_Master_MasterWriteByte(register_address);
        
        if (temp == I2C_Master_MSTR_NO_ERROR) {
            // Write byte
            temp = I2C_Master_MasterWriteByte(data);
        } // end write
    }  // end start
    
    // Send stop condition
    I2C_Master_MasterSendStop();

} // end I2C_Peripheral_WriteRegister
                                

/* [] END OF FILE */
