/* ========================================
 *
 * Copyright LTEBS srl, 2020
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF LTEBS srl.
 *
 * \file  main.c
 * \brief Main source file for the Assignment_5
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
#include "project.h"
#include "InterruptRoutines.h"
#include "I2C.h"
#include "Utility.h"
#include <stdio.h>


// Defines
    // EEPROM register where the frequency for the LIS3DH is stored
#define STARTUP_REG          0x0000  
    // Macros for the packet of data to be sent via UART
#define HEADER               0xA0
#define TAIL                 0xC0
#define AXES                 3
#define BYTE_TO_SEND         2*AXES
#define TRANSMIT_BUFFER_SIZE 1+BYTE_TO_SEND+1
    // Macros for the convertion of data (justified in the code)
#define DIGIT_TO_G           1000.0
#define G_TO_MS2             9.81
    // Macros for the LIS3DH are found in the "Utility.h" header file


// Useful variables
uint8_t count_push         = 0; // Variable that tracks the cycling of frequencies
uint8_t init_ctrl_reg1     = 0; // Varaible that stores the initial setting for 
                                // LIS3DH CONTROL REGISTER 1 (which sets the frequency)   

uint8_t DataBuffer[TRANSMIT_BUFFER_SIZE] = {'\0'}; // Buffer with XYZ data to be sent
uint8_t AccelerationData[BYTE_TO_SEND]   = {'\0'}; // Temporary buffer

int16_t OutAcc = 0;    // Auxiliary variable
float conv     = 0.0;  // Auxiliary variable   

                                    
// TEST VARAIBLES
char msg[50]            = {'\0'};
uint8_t test_write_read = 0;
uint8_t err             = 0;
                                    

int main(void) {

    CyGlobalIntEnable; /* Enable global interrupts. */
    
    // Start components (EEPROM, I2C, UART)
    EEPROM_Start();
    I2C_Master_Start();
    UART_Start();
    
    // The LIS3DH datasheet states that the boot procedure of the device is completed
    // 5ms after the power-up of the device
    CyDelay(5);
    
    
    /* ---------------------------------- */
    /*         CONNECTED DEVICES          */
    /* ---------------------------------- */    
    
    // Check which devices are present on the I2C bus (just as intial control)
    for (int i=0;i<128;i++) {
        // Scan the whole I2C bus --> search for LIS3DH
        if (I2C_Peripheral_IsDeviceConnected(i)) {
            // Display address of connected device in hex format
            sprintf(msg, "CONNECTED DEVICE: 0x%02X [Expected: 0x18]\r\n", i);
            UART_PutString(msg);
            
            /* !IMPORTANT!
             * For this application, only the 0x18 adress should be occupied on the I2C
             * bus. Any other connection will lead to an error in the communication
            */
            if(i != LIS3DH_DEVICE_ADDRESS) {
                UART_PutString("ERROR OCCURRED\r\n");
                UART_PutString("--------------------\r\n");
                UART_PutString("Reset the device\r\n");
                UART_PutString("If the problem persists: disconnect and reconnect LIS3DH to power line, then reset the PSoC.\r\n");
                UART_PutString("--------------------\r\n\r\n");
                return -1;
            }
        }
    } // end scan check
    
    
    /* ---------------------------------- */
    /*         INITIAL CONTROLS           */
    /* ---------------------------------- */
    
    // Read WHO AM I register of connected device
    uint8_t who_am_i_reg = 0;
    err = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                      LIS3DH_WHO_AM_I_REG, 
                                      &who_am_i_reg);
    if(err == NO_ERROR) {    
        sprintf(msg, "WHO AM I REGISTER: 0x%02X [Expected: 0x33]\r\n", who_am_i_reg);
        UART_PutString(msg);
    }
    else {
        UART_PutString("Error occurred during I2C communication.\r\n");
    }
    
    // Read Status register of connected device
    uint8_t status_register = 0;
    err = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                      LIS3DH_STATUS_REG,
                                      &status_register);
    if(err == NO_ERROR) {    
        sprintf(msg, "STATUS REGISTER: 0x%02X\r\n", status_register);
        UART_PutString(msg);
    }
    else {
        UART_PutString("Error occurred during I2C communication.\r\n");
    }    
    
    // Read Control Register 1 of connected device
    uint8_t ctrl_reg1 = 0;
    err = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                      LIS3DH_CTRL_REG1,
                                      &ctrl_reg1);
    if(err == NO_ERROR) {
        sprintf(msg, "CONTROL REGISTER 1: 0x%02X\r\n", ctrl_reg1);
        UART_PutString(msg);
    }
    else {
        UART_PutString("Error occurred during I2C communication.\r\n");
    }    

    // Read Control Register 4 of connected device
    uint8_t ctrl_reg4 = 0;
    err = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                      LIS3DH_CTRL_REG4,
                                      &ctrl_reg4);
    if(err == NO_ERROR) {
        sprintf(msg, "CONTROL REGISTER 4: 0x%02X\r\n", ctrl_reg4);
        UART_PutString(msg);
    }
    else {
        UART_PutString("Error occurred during I2C communication.\r\n");
    }    
    
    
    /* ---------------------------------- */
    /*           SET FREQUENCY            */
    /* ---------------------------------- */    
    
    // Read sampling frequency from EEPROM (adress 0x0000)
    init_ctrl_reg1 = EEPROM_ReadByte(STARTUP_REG);
    sprintf(msg, "EEPROM value for CONTROL REGISTER 1: 0x%02X\r\n", init_ctrl_reg1);
    UART_PutString(msg);
    
    /*
     * The very first time the device is set up, or in case the device has been used for
     * something else previously, in that cell of the EEPROM we could have anything.. 
     * so check if it consistent with an allowed sampling frequency (and set the
     * counter accordingly) otherwise set it, by default, to the lowest (1 Hz). 
     * At the same time, set the LIS3DH accordingly.
     * LPen bit is set to 0 to enable a proper HR setting in the control register 4.
    */
    switch(init_ctrl_reg1) {
    
        case LIS3DH_1_HZ_CTRL_REG1:
            count_push = 1;
            // Set frequency
            SetOperatingFrequency(ctrl_reg1, LIS3DH_1_HZ_CTRL_REG1);
            break;
        case LIS3DH_10_HZ_CTRL_REG1:
            count_push = 2;
            // Set frequency
            SetOperatingFrequency(ctrl_reg1, LIS3DH_10_HZ_CTRL_REG1);
            break;
        case LIS3DH_25_HZ_CTRL_REG1:
            count_push = 3;
            // Set frequency
            SetOperatingFrequency(ctrl_reg1, LIS3DH_25_HZ_CTRL_REG1);
            break;
        case LIS3DH_50_HZ_CTRL_REG1:
            count_push = 4;
            // Set frequency
            SetOperatingFrequency(ctrl_reg1, LIS3DH_50_HZ_CTRL_REG1);
            break;        
        case LIS3DH_100_HZ_CTRL_REG1:
            count_push = 5;
            // Set frequency
            SetOperatingFrequency(ctrl_reg1, LIS3DH_100_HZ_CTRL_REG1);
            break;        
        case LIS3DH_200_HZ_CTRL_REG1:
            count_push = 0; // To be ready to restart the cycle
            // Set frequency
            SetOperatingFrequency(ctrl_reg1, LIS3DH_200_HZ_CTRL_REG1);
            break;
        default:
            // Write default value on EEPROM (1 Hz)
            EEPROM_UpdateTemperature();
            EEPROM_WriteByte(LIS3DH_1_HZ_CTRL_REG1,STARTUP_REG);
            
            // EEPROM test
            test_write_read = EEPROM_ReadByte(STARTUP_REG);
            sprintf(msg, "Default value set: 0x%02X\r\n", test_write_read);
            UART_PutString(msg);
            
            init_ctrl_reg1 = test_write_read;            
            count_push = 1;
            // Set frequency
            SetOperatingFrequency(ctrl_reg1, LIS3DH_1_HZ_CTRL_REG1);
            break;
        
    } // end initial frequency setting    
    
    
    /* ---------------------------------- */
    /*         SET OPERATING MODE         */
    /* ---------------------------------- */ 
    
    /*
     * Then, we have to set the LIS3DH to High Resolution operating mode
     * This will be done only if the device is not in HR mode yet
     * To set properly the HR mode we need also the LPen bit in the CRTL_REG1 at 0..
     * This has been done when we set the sampling frequency of the device.
     * Sets also BDU
    */
    if (ctrl_reg4 != LIS3DH_HR_MODE_CTRL_REG4) {
        
        UART_PutString("\r\nUpdating Operating Mode\r\n");
        
        // Update the register with the correct value
        ctrl_reg4 = LIS3DH_HR_MODE_CTRL_REG4;
        err = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                           LIS3DH_CTRL_REG4,
                                           ctrl_reg4);
        if(err == NO_ERROR) {
            sprintf(msg, "CONTROL REGISTER 4 written as: 0x%02X\r\n", ctrl_reg4);
            UART_PutString(msg);    
        }
        else {
            UART_PutString("Error occurred during I2C communication.\r\n");
        }        
        
        // Check that the register has been overwritten correctly
        err = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                          LIS3DH_CTRL_REG4,
                                          &ctrl_reg4);
        if(err == NO_ERROR){
            sprintf(msg, "CONTROL REGISTER 4 after overwrite: 0x%02X\r\n", ctrl_reg4);
            UART_PutString(msg);
        }
        else {
            UART_PutString("Error occurred during I2C communication.\r\n");
        }
            
    } // end HR setting
        
    
    // Init flags
    flag_push = 0;
    
    // Start ISRs
    ISR_Push_StartEx(Custom_ISR_Push); 
    
    // Init packet of data
    DataBuffer[0]                      = HEADER;
    DataBuffer[TRANSMIT_BUFFER_SIZE-1] = TAIL;

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
                    EEPROM_WriteByte(LIS3DH_1_HZ_CTRL_REG1,STARTUP_REG);
                    // Set frequency
                    SetOperatingFrequency(ctrl_reg1, LIS3DH_1_HZ_CTRL_REG1);
                    break;
                case 2:
                    // Write on EEPROM
                    EEPROM_UpdateTemperature();
                    EEPROM_WriteByte(LIS3DH_10_HZ_CTRL_REG1,STARTUP_REG);
                    // Set frequency
                    SetOperatingFrequency(ctrl_reg1, LIS3DH_10_HZ_CTRL_REG1);
                    break;
                case 3:
                    // Write on EEPROM
                    EEPROM_UpdateTemperature();
                    EEPROM_WriteByte(LIS3DH_25_HZ_CTRL_REG1,STARTUP_REG);
                    // Set frequency
                    SetOperatingFrequency(ctrl_reg1, LIS3DH_25_HZ_CTRL_REG1);
                    break;
                case 4:
                    // Write on EEPROM
                    EEPROM_UpdateTemperature();
                    EEPROM_WriteByte(LIS3DH_50_HZ_CTRL_REG1,STARTUP_REG);
                    // Set frequency
                    SetOperatingFrequency(ctrl_reg1, LIS3DH_50_HZ_CTRL_REG1);
                    break;
                case 5:
                    // Write on EEPROM
                    EEPROM_UpdateTemperature();
                    EEPROM_WriteByte(LIS3DH_100_HZ_CTRL_REG1,STARTUP_REG);
                    // Set frequency
                    SetOperatingFrequency(ctrl_reg1, LIS3DH_100_HZ_CTRL_REG1);
                    break;
                case 6:
                    // Write on EEPROM
                    EEPROM_UpdateTemperature();
                    EEPROM_WriteByte(LIS3DH_200_HZ_CTRL_REG1,STARTUP_REG);
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
        err = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                          LIS3DH_STATUS_REG,
                                          &status_register);
        if(err == NO_ERROR) {
        
            // Acquire data only if we have new data available
            if(status_register & LIS3DH_ZYXDA_MASK) {
                
                // Read all the data from X, Y and Z axes
                err = I2C_Peripheral_ReadRegisterMulti(LIS3DH_DEVICE_ADDRESS, 
                                                       LIS3DH_OUT_X_L, 
                                                       BYTE_TO_SEND, 
                                                       AccelerationData);
                if(err == NO_ERROR) {
                    
                    // Right shift of 4 bit is necessary since data are in 12-bit resolution
                    // while the auxiliary variable is an int16
                    
                    // In HR mode one digit corresponds to one mg --> to have the g value we have
                    // to divide by 1000 and then, to have it in m/s^2, multiply by 9.81
                    
                    /* ---------------------------------- */
                    /*                 X                  */
                    /* ---------------------------------- */
                    OutAcc = (int16_t)((AccelerationData[0] | (AccelerationData[1]<<8)))>>4;
                    
                    // Conversion into m/s^2
                    conv = OutAcc/DIGIT_TO_G*G_TO_MS2;
                    OutAcc = (int16_t) (conv*1000);            
                    
                    DataBuffer[1] = (uint8_t) (OutAcc & 0xFF);
                    DataBuffer[2] = (uint8_t) (OutAcc>>8);
                    
                    
                    /* ---------------------------------- */
                    /*                 Y                  */
                    /* ---------------------------------- */
                    OutAcc = (int16_t)((AccelerationData[2] | (AccelerationData[3]<<8)))>>4;
                    
                    // Conversion into m/s^2
                    conv = OutAcc/DIGIT_TO_G*G_TO_MS2;
                    OutAcc = (int16_t) (conv*1000);
                    
                    DataBuffer[3] = (uint8_t) (OutAcc & 0xFF);
                    DataBuffer[4] = (uint8_t) (OutAcc>>8);
                    
                    
                    /* ---------------------------------- */
                    /*                 Z                  */
                    /* ---------------------------------- */
                    OutAcc = (int16_t)((AccelerationData[4] | (AccelerationData[5]<<8)))>>4;
                    
                    // Conversion into m/s^2
                    conv = OutAcc/DIGIT_TO_G*G_TO_MS2;
                    OutAcc = (int16_t) (conv*1000);                       
                    
                    DataBuffer[5] = (uint8_t) (OutAcc & 0xFF);
                    DataBuffer[6] = (uint8_t) (OutAcc>>8);
                    
                    
                    // Transmit the packet
                    UART_PutArray(DataBuffer,TRANSMIT_BUFFER_SIZE);
                    
                } // end if(read axes output is ok)
                
            } // end data transmission
            
        } // end if(read status register is ok)
                    
    } // end for
    
} // end main

/* [] END OF FILE */
