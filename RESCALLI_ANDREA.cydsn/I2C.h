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

#ifndef __I2C_H_
    #define __I2C_H_
    
    #include "cytypes.h"
    
    
    /*
     * Declaration of function that reads one byte from a device's register 
     * through I2C protcol. The parameters needed are:
     * - adress of the device
     * - adress of the register we want to read
     * - pointer where to save the read data
     *
    */
    void I2C_Peripheral_ReadRegister(uint8_t device_address, 
                                     uint8_t register_address,
                                     uint8_t* data);
    
    
    /*
     * Declaration of function that reads multiple device's registers
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
                                          uint8_t* data);
    

    /*
     * Declaration of function that writes a byte to a device's register
     * through I2C protcol. The parameters needed are:
     * - adress of the device
     * - adress of the register we want to read
     * - data to be written
     *
    */
    void I2C_Peripheral_WriteRegister(uint8_t device_address,
                                      uint8_t register_address,
                                      uint8_t data);
     
#endif

/* [] END OF FILE */