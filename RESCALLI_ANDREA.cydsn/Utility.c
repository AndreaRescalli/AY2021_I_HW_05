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

#include "Utility.h"
#include "I2C.h"
#include "project.h"
#include <stdio.h>

char message[50] = {'\0'};

void SetOperatingFrequency(uint8_t register_value, 
                           uint8_t desired_value) {
    
    // Read the register: we will update only if there is a different value in it
    I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                LIS3DH_CTRL_REG1,
                                &register_value);
    
    if (register_value != desired_value) {
        
        UART_PutString("\r\nUpdating Sampling Frequency\r\n");
        // Set the frequency by writing on the register the correct value
        register_value = desired_value;
        I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                     LIS3DH_CTRL_REG1,
                                     register_value);
        sprintf(message, "Desired value for CONTROL REGISTER 1: 0x%02X\r\n", register_value);
        UART_PutString(message);
        
        // Check that the register has been overwritten correctly
        I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                    LIS3DH_CTRL_REG1,
                                    &register_value);
        sprintf(message, "CONTROL REGISTER 1 after overwrite: 0x%02X\r\n", register_value);
        UART_PutString(message);    
            
    }
    
}

/* [] END OF FILE */
