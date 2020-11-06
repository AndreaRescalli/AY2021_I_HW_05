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
#include "project.h"
#include "InterruptRoutines.h"
#include "I2C.h"
#include "Utility.h"
#include <stdio.h>


// Defines
# define STARTUP_REG 0x0000

#define A   1
#define B   10
#define C   25
#define D   50
#define E   100
#define F   200


// Useful variables
static uint8_t count_push         = 0; // Flag that informs whether the button has been pressed
static uint8_t sampling_frequency = 0; // Varaible that stores the sampling frequency at which the
                                       // accelerometer operates [Hz]   

// TEST VARAIBLES
char msg[50]            = {'\0'};
uint8_t who_am_i_reg    = 0;
uint8_t status_register = 0;
uint8_t ctrl_reg1       = 0;
uint8_t ctrl_reg4       = 0;

uint8_t test_write_read = 0;
                                    

int main(void) {

    CyGlobalIntEnable; /* Enable global interrupts. */
    
    // Start components (EEPROM, UART, I2C)
    EEPROM_Start();
    UART_Start();
    I2C_Master_Start();
    
    // The LIS3DH datasheet states that the boot procedure of the device is completed
    // 5ms after the power-up of the device
    CyDelay(5);
    
    // Start ISRs
    ISR_Push_StartEx(Custom_ISR_Push);    
    
    // Init flags
    flag_push = 0;
    
    // Check which devices are present on the I2C bus (just as intial control)
    for (int i = 0;i<128;i++) {
        
        // Scan the whole I2C bus
        if (I2C_Peripheral_IsDeviceConnected(i)) {
            
            // Display address of conencted device in hex format
            sprintf(msg, "Connected device: 0x%02X [Expected: 0x18]\r\n", i);
            UART_PutString(msg);
            
            // Read WHO AM I register of connected device
            I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_WHO_AM_I_REG, 
                                        &who_am_i_reg);
            sprintf(msg, "WHO AM I register: 0x%02X [Expected: 0x33]\r\n", who_am_i_reg);
            UART_PutString(msg); 
            
            // Read Status register of connected device
            I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_STATUS_REG,
                                        &status_register);
            sprintf(msg, "STATUS REGISTER: 0x%02X\r\n", status_register);
            UART_PutString(msg);
            
            // Read Control Register 1 of connected device
            I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_CTRL_REG1,
                                        &ctrl_reg1);
            sprintf(msg, "CONTROL REGISTER 1: 0x%02X\r\n", ctrl_reg1);
            UART_PutString(msg); 

            // Read Control Register 4 of connected device
            I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_CTRL_REG4,
                                        &ctrl_reg4);
            sprintf(msg, "CONTROL REGISTER 4: 0x%02X\r\n", ctrl_reg4);
            UART_PutString(msg);            
            
        } // end if
        
    } // end scan check
    
    
    // First of all, we have to set the LIS3DH to High Resolution operating mode
    // This will be done only if the device is not in HR mode yet
    if (ctrl_reg4 != LIS3DH_HR_MODE_CTRL_REG4) {
        
        UART_PutString("\r\nUpdating Operating Mode\r\n");
        
        // Update the register with the correct value
        ctrl_reg4 = LIS3DH_HR_MODE_CTRL_REG4;
        I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                     LIS3DH_CTRL_REG4,
                                     ctrl_reg4);
        sprintf(msg, "Desired value for CONTROL REGISTER 4: 0x%02X\r\n", ctrl_reg4);
        UART_PutString(msg);
        
        // Check that the register has been overwritten correctly
        I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                    LIS3DH_CTRL_REG4,
                                    &ctrl_reg4);
        sprintf(msg, "CONTROL REGISTER 4 after overwrite: 0x%02X\r\n", ctrl_reg4);
        UART_PutString(msg);    
            
    } // end HR setting
        
    
    // Read sampling frequency from EEPROM (adress 0x0000)
    sampling_frequency = EEPROM_ReadByte(STARTUP_REG);
    sprintf(msg, "Stored Sampling Frequency: %d\r\n", sampling_frequency);
    UART_PutString(msg);
    
    // The very first time the device is set up, or in case the device has been used for
    // something else previously, in that cell of the EEPROM we could have anything.. 
    // so check if it consistent with an allowed sampling frequency (and set the
    // counter accordingly) otherwise set it, by default, to the lowest (1 Hz). 
    // At the same time, set the LIS3DH accordingly
    switch(sampling_frequency) {
    
        case A:
            count_push = 1;
            // Set frequency
            SetOperatingFrequency(ctrl_reg1, LIS3DH_1_HZ_CTRL_REG1);
            break;
        case B:
            count_push = 2;
            // Set frequency
            SetOperatingFrequency(ctrl_reg1, LIS3DH_10_HZ_CTRL_REG1);
            break;
        case C:
            count_push = 3;
            // Set frequency
            SetOperatingFrequency(ctrl_reg1, LIS3DH_25_HZ_CTRL_REG1);
            break;
        case D:
            count_push = 4;
            // Set frequency
            SetOperatingFrequency(ctrl_reg1, LIS3DH_50_HZ_CTRL_REG1);
            break;        
        case E:
            count_push = 5;
            // Set frequency
            SetOperatingFrequency(ctrl_reg1, LIS3DH_100_HZ_CTRL_REG1);
            break;        
        case F:
            count_push = 6;
            // Set frequency
            SetOperatingFrequency(ctrl_reg1, LIS3DH_200_HZ_CTRL_REG1);
            break;
        default:
            // Write default value on EEPROM (1 Hz)
            EEPROM_UpdateTemperature();
            EEPROM_WriteByte(A,STARTUP_REG);
            
            // EEPROM test
            test_write_read = EEPROM_ReadByte(STARTUP_REG);
            sprintf(msg, "Default value set: %d\r\n", test_write_read);
            UART_PutString(msg);
            
            sampling_frequency = A;            
            count_push = 1;
            // Set frequency
            SetOperatingFrequency(ctrl_reg1, LIS3DH_1_HZ_CTRL_REG1);
            break;
        
    } // end initial frequency setting
    

    for(;;) {
    
        // Check for button press
        if(flag_push) {
            // Reset flag
            flag_push = 0;
            
            // Keep track of how many pushes have been done
            count_push++;
            
            // Set the appropriate sampling frequency
            switch(count_push) {
            
                case 1:
                    // Write on EEPROM
                    EEPROM_UpdateTemperature();
                    EEPROM_WriteByte(A,STARTUP_REG);
                    // Set frequency
                    SetOperatingFrequency(ctrl_reg1, LIS3DH_1_HZ_CTRL_REG1);
                    break;
                case 2:
                    // Write on EEPROM
                    EEPROM_UpdateTemperature();
                    EEPROM_WriteByte(B,STARTUP_REG);
                    // Set frequency
                    SetOperatingFrequency(ctrl_reg1, LIS3DH_10_HZ_CTRL_REG1);
                    break;
                case 3:
                    // Write on EEPROM
                    EEPROM_UpdateTemperature();
                    EEPROM_WriteByte(C,STARTUP_REG);
                    // Set frequency
                    SetOperatingFrequency(ctrl_reg1, LIS3DH_25_HZ_CTRL_REG1);
                    break;
                case 4:
                    // Write on EEPROM
                    EEPROM_UpdateTemperature();
                    EEPROM_WriteByte(D,STARTUP_REG);
                    // Set frequency
                    SetOperatingFrequency(ctrl_reg1, LIS3DH_50_HZ_CTRL_REG1);
                    break;
                case 5:
                    // Write on EEPROM
                    EEPROM_UpdateTemperature();
                    EEPROM_WriteByte(E,STARTUP_REG);
                    // Set frequency
                    SetOperatingFrequency(ctrl_reg1, LIS3DH_100_HZ_CTRL_REG1);
                    break;
                case 6:
                    // Write on EEPROM
                    EEPROM_UpdateTemperature();
                    EEPROM_WriteByte(F,STARTUP_REG);
                    // Start frequency
                    SetOperatingFrequency(ctrl_reg1, LIS3DH_200_HZ_CTRL_REG1);
                    count_push = 0;
                    break;
                default:
                    UART_PutString("Error\r\n");
                    break;
            
            } // end switch(count_push)
            
            
        } // end if(flag_push)
        
    } // end for
    
} // end main

/* [] END OF FILE */
