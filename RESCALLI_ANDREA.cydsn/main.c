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
#define STARTUP_REG     0x0000

#define ONE_HZ          1
#define TEN_HZ          10
#define TWENTYFIVE_HZ   25
#define FIFTY_HZ        50
#define HUNDRED_HZ      100
#define TWOHUNDRED_HZ   200

#define HEADER               0xA0
#define TAIL                 0xC0
#define AXES                 3
#define BYTE_TO_SEND         2*AXES
#define TRANSMIT_BUFFER_SIZE 1+BYTE_TO_SEND+1


// Useful variables
static uint8_t count_push         = 0; // Flag that informs whether the button has been pressed
static uint8_t sampling_frequency = 0; // Varaible that stores the sampling frequency at which the
                                       // accelerometer operates [Hz]   

static uint8_t DataBuffer[TRANSMIT_BUFFER_SIZE]; // Buffer with XYZ data to send
static uint8_t AccelerationData[BYTE_TO_SEND];   // Temporary buffer
int16_t OutAcc = 0;                              // Auxiliary variable
float conv = 0.0;                                  // Auxiliary variable   

                                    
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
    
    // Init flags
    flag_push = 0;
    
    // Start ISRs
    ISR_Push_StartEx(Custom_ISR_Push);    
    
    // Init packet of data
    DataBuffer[0]                      = HEADER;
    DataBuffer[TRANSMIT_BUFFER_SIZE-1] = TAIL;
    
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
            sprintf(msg, "WHO AM I REGISTER: 0x%02X [Expected: 0x33]\r\n", who_am_i_reg);
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
    
    /*
     * First of all, we have to set the LIS3DH to High Resolution operating mode
     * This will be done only if the device is not in HR mode yet
     * To set properly the HR mode we need also the LPen bit in the CRTL_REG1 at 0..
     * This will be done when we set the sampling frequency of the device
     * Sets also BDU
    */
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
    sprintf(msg, "Stored Sampling Frequency: %d Hz\r\n", sampling_frequency);
    UART_PutString(msg);
    
    /*
     * The very first time the device is set up, or in case the device has been used for
     * something else previously, in that cell of the EEPROM we could have anything.. 
     * so check if it consistent with an allowed sampling frequency (and set the
     * counter accordingly) otherwise set it, by default, to the lowest (1 Hz). 
     * At the same time, set the LIS3DH accordingly
    */
    switch(sampling_frequency) {
    
        case ONE_HZ:
            count_push = 1;
            // Set frequency
            SetOperatingFrequency(ctrl_reg1, LIS3DH_1_HZ_CTRL_REG1);
            break;
        case TEN_HZ:
            count_push = 2;
            // Set frequency
            SetOperatingFrequency(ctrl_reg1, LIS3DH_10_HZ_CTRL_REG1);
            break;
        case TWENTYFIVE_HZ:
            count_push = 3;
            // Set frequency
            SetOperatingFrequency(ctrl_reg1, LIS3DH_25_HZ_CTRL_REG1);
            break;
        case FIFTY_HZ:
            count_push = 4;
            // Set frequency
            SetOperatingFrequency(ctrl_reg1, LIS3DH_50_HZ_CTRL_REG1);
            break;        
        case HUNDRED_HZ:
            count_push = 5;
            // Set frequency
            SetOperatingFrequency(ctrl_reg1, LIS3DH_100_HZ_CTRL_REG1);
            break;        
        case TWOHUNDRED_HZ:
            count_push = 0; // To be ready to restart the cycle
            // Set frequency
            SetOperatingFrequency(ctrl_reg1, LIS3DH_200_HZ_CTRL_REG1);
            break;
        default:
            // Write default value on EEPROM (1 Hz)
            EEPROM_UpdateTemperature();
            EEPROM_WriteByte(ONE_HZ,STARTUP_REG);
            
            // EEPROM test
            test_write_read = EEPROM_ReadByte(STARTUP_REG);
            sprintf(msg, "Default value set: %d Hz\r\n", test_write_read);
            UART_PutString(msg);
            
            sampling_frequency = ONE_HZ;            
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
                    EEPROM_WriteByte(ONE_HZ,STARTUP_REG);
                    // Set frequency
                    SetOperatingFrequency(ctrl_reg1, LIS3DH_1_HZ_CTRL_REG1);
                    break;
                case 2:
                    // Write on EEPROM
                    EEPROM_UpdateTemperature();
                    EEPROM_WriteByte(TEN_HZ,STARTUP_REG);
                    // Set frequency
                    SetOperatingFrequency(ctrl_reg1, LIS3DH_10_HZ_CTRL_REG1);
                    break;
                case 3:
                    // Write on EEPROM
                    EEPROM_UpdateTemperature();
                    EEPROM_WriteByte(TWENTYFIVE_HZ,STARTUP_REG);
                    // Set frequency
                    SetOperatingFrequency(ctrl_reg1, LIS3DH_25_HZ_CTRL_REG1);
                    break;
                case 4:
                    // Write on EEPROM
                    EEPROM_UpdateTemperature();
                    EEPROM_WriteByte(FIFTY_HZ,STARTUP_REG);
                    // Set frequency
                    SetOperatingFrequency(ctrl_reg1, LIS3DH_50_HZ_CTRL_REG1);
                    break;
                case 5:
                    // Write on EEPROM
                    EEPROM_UpdateTemperature();
                    EEPROM_WriteByte(HUNDRED_HZ,STARTUP_REG);
                    // Set frequency
                    SetOperatingFrequency(ctrl_reg1, LIS3DH_100_HZ_CTRL_REG1);
                    break;
                case 6:
                    // Write on EEPROM
                    EEPROM_UpdateTemperature();
                    EEPROM_WriteByte(TWOHUNDRED_HZ,STARTUP_REG);
                    // Start frequency
                    SetOperatingFrequency(ctrl_reg1, LIS3DH_200_HZ_CTRL_REG1);
                    count_push = 0;
                    break;
                default:
                    UART_PutString("Error\r\n");
                    break;
            
            } // end switch(count_push)
                       
        } // end if(flag_push)
        
        // Read Status register
        I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                    LIS3DH_STATUS_REG,
                                    &status_register);
        
        // Acquire new data only if we have new data available
        if(status_register & LIS3DH_ZYXDA_MASK) {
            
            // Read all the data from X, Y and Z axes
            I2C_Peripheral_ReadRegisterMulti(LIS3DH_DEVICE_ADDRESS, 
                                             LIS3DH_OUT_X_L, 
                                             BYTE_TO_SEND, 
                                             AccelerationData);
            
            // Right shift of 4 bit is necessary since data are in 12-bit resolution
            // while the auxiliary variable is an int16
            
            // In HR mode one digit corresponds to one mg --> to have the g value we have
            // to divide by 1000 and then, to have it in m/s^2, to multiply by 9.81
            
            /* ---------------------------------- */
            // X values
            OutAcc = (int16_t)((AccelerationData[0] | (AccelerationData[1]<<8)))>>4;
            
            // Conversion into m/s^2
            conv = (float) (OutAcc/1000.0*9.81);
//            PrintFloat(conv);
            
            OutAcc = (int16_t) (conv*1000);
//            sprintf(msg, "X_acc : %d\r\n", OutAcc);
//            UART_PutString(msg);            
            
            DataBuffer[1] = (uint8_t) (OutAcc & 0xFF);
            DataBuffer[2] = (uint8_t) (OutAcc>>8);
            
            
            /* ----------------------------------- */
            // Y values
            OutAcc = (int16_t)((AccelerationData[2] | (AccelerationData[3]<<8)))>>4;
            
            // Conversion into m/s^2
            conv = (float) (OutAcc/1000.0*9.81);
//            PrintFloat(conv);

            OutAcc = (int16_t) (conv*1000);
//            sprintf(msg, "Y_acc : %d\r\n", OutAcc);
//            UART_PutString(msg);
            
            DataBuffer[3] = (uint8_t) (OutAcc & 0xFF);
            DataBuffer[4] = (uint8_t) (OutAcc>>8);
            
            
            /* ----------------------------------- */
            // Z values
            OutAcc = (int16_t)((AccelerationData[4] | (AccelerationData[5]<<8)))>>4;
            
            // Conversion into m/s^2
            conv = (float) (OutAcc/1000.0*9.81);
//            PrintFloat(conv);
            
            OutAcc = (int16_t) (conv*1000);            
//            sprintf(msg, "Z_acc : %d\r\n", OutAcc);
//            UART_PutString(msg);            
            
            DataBuffer[5] = (uint8_t) (OutAcc & 0xFF);
            DataBuffer[6] = (uint8_t) (OutAcc>>8);
            
            UART_PutArray(DataBuffer,TRANSMIT_BUFFER_SIZE);
            
        } // end data transmission
                    
    } // end for
    
} // end main

/* [] END OF FILE */
