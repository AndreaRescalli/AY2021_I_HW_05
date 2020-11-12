/* ========================================
 *
 * Copyright LTEBS srl, 2020
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF LTEBS srl.
 *
 * \file  Utility.c
 * \brief Source file including the definition of a function that allows the setting of
 *          the CONTROL REGISTER 1 according to the desired value
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


// Includes
#include "Utility.h"
#include "I2C.h"
#include "project.h"
#include <stdio.h>


// Useful variables
char message[50] = {'\0'};


/*
 * Definition of function that sets the operating frequency of the LIS3DH.
 * As parameters it requires:
 * - current value of the Control Register 1 (where the frequency is set)
 * - desired value to be written in the register
*/
void SetOperatingFrequency(uint8_t register_value, 
                           uint8_t desired_value) {
    
    // Read the register: we will update only if there is a different value in it
    uint8_t error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                                LIS3DH_CTRL_REG1,
                                                &register_value);
    if(error == NO_ERROR) {
        
        if (register_value != desired_value) {
            // Set the frequency by writing on the register the correct value
            // This also ensures LPen bit is 0 (it's embedded in the desired_value)
            register_value = desired_value;
            error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                                 LIS3DH_CTRL_REG1,
                                                 register_value);
            if(error == ERROR) {
                UART_PutString("Error occurred during I2C communication.\r\n");
            }
        } // end write
        
    } // end if(read is ok)
    else {
        UART_PutString("Error occurred during I2C communication.\r\n");
    }
    
} // end SetOperatingFrequency
                                              

/* [] END OF FILE */
