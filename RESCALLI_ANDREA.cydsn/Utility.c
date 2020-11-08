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
                UART_PutString("Power down, power up and reset the device.\r\n");    
            }
        } // end write
        
    } // end if(read is ok)
    else {
        UART_PutString("Error occurred during I2C communication.\r\n");
        UART_PutString("Power down, power up and reset the device.\r\n");   
    }
    
} // end SetOperatingFrequency
                        

// Definition of helping function to print float varaibles via UART
void PrintFloat(float value) {

    // Message that will be displayed
    char str[100];

    char *tmpSign = (value < 0) ? "-" : "";
    float tmpVal = (value < 0) ? -value : value;

    int16_t tmpInt1 = tmpVal;             // Get the integer
    float tmpFrac = tmpVal - tmpInt1;     // Get decimals
    int16_t tmpInt2 = tmpFrac * 1000;     // Turn decimals into integer

    // Print as parts, note that you need 0-padding for fractional bit.
    sprintf (str, "Acceleration = %s%d.%03d\n", tmpSign, tmpInt1, tmpInt2);
    UART_PutString(str);

} // end PrintFloat                       

/* [] END OF FILE */
