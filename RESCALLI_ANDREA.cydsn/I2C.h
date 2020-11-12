/* ========================================
 *
 * Copyright LTEBS srl, 2020
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF LTEBS srl.
 *
 * \file  I2C.h
 * \brief Header file including all the functions necessary for the I2C communication
 *
 * I2C communication from PSoC (master) to a slave accelerometer (LIS3DH). Operating frequency
 * of the device can be changed (and stored into EEPROM, from where will be loaded into the
 * LIS3DH's register at startup) by using the on-board button of the PSoC.
 * Data collected on the 3 axes will be sent via UART to the Bridge Panel Control in m/s^2
 * 
 *
 * \author: Andrea Rescalli
 * \date:   14/11/2020
 *
 * ========================================
*/

#ifndef __I2C_H_
    #define __I2C_H_
    
    // Includes
    #include "cytypes.h"
    
    
    // Defines
    #define DEVICE_CONNECTED    1
    #define DEVICE_UNCONNECTED  0
    
    #define ERROR    1
    #define NO_ERROR 0
    
    
    /*
     * Declaration of function that searches for connected devices on the I2C bus.
     * As only parameter it requires a device adress (will be incremental for a scan operation)
     *
    */
    uint8_t I2C_Peripheral_IsDeviceConnected(uint8_t device_address);
    
    
    
    /*
     * Declaration of function that reads one byte from a device's register 
     * through I2C protcol. The parameters needed are:
     * - adress of the device
     * - adress of the register we want to read
     * - pointer where to save the read data
     *
    */
    uint8_t I2C_Peripheral_ReadRegister(uint8_t device_address, 
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
    uint8_t I2C_Peripheral_ReadRegisterMulti(uint8_t device_address,
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
    uint8_t I2C_Peripheral_WriteRegister(uint8_t device_address,
                                         uint8_t register_address,
                                         uint8_t data);
     
#endif

/* [] END OF FILE */
