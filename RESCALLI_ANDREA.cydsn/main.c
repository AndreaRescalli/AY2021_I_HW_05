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
#include <stdio.h>


// Defines
# define STARTUP_REG 0x0000

#define A   1
#define B   10
#define C   25
#define D   50
#define E   100
#define F   200

// Accelerometer macros
#define LIS3DH_DEVICE_ADDRESS       0x18  // Adress of slave device (accelerometer)
#define LIS3DH_WHO_AM_I_REG         0x0F  // WHO AM I register adress
#define LIS3DH_CTRL_REG1            0x20  // Control register 1 adress
#define LIS3DH_CTRL_REG4            0x23  // Control register 4 adress
#define LIS3DH_STATUS_REG           0x27  // Status register adress
#define LIS3DH_OUT_X_L              0x28  // X-axis output LSB register adress
#define LIS3DH_OUT_X_H              0x29  // X-axis output MSB register adress
#define LIS3DH_OUT_Y_L              0x2A  // Y-axis output LSB register adress
#define LIS3DH_OUT_Y_H              0x2B  // Y-axis output MSB register adress
#define LIS3DH_OUT_Z_L              0x2C  // Z-axis output LSB register adress
#define LIS3DH_OUT_Z_H              0x2D  // Z-axis output MSB register adress

#define LIS3DH_HR_MODE_CTRL_REG4    0x08  // Set operating mode to high resolution

#define LIS3DH_1_HZ_CTRL_REG1       0x17  // Set sampling frequency to 1 Hz
#define LIS3DH_10_HZ_CTRL_REG1      0x27  // Set sampling frequency to 10 Hz
#define LIS3DH_25_HZ_CTRL_REG1      0x37  // Set sampling frequency to 25 Hz
#define LIS3DH_50_HZ_CTRL_REG1      0x47  // Set sampling frequency to 50 Hz
#define LIS3DH_100_HZ_CTRL_REG1     0x57  // Set sampling frequency to 100 Hz
#define LIS3DH_200_HZ_CTRL_REG1     0x67  // Set sampling frequency to 200 Hz


// Globals
static uint8 count_push         = 0; // Flag that informs whether the button has been pressed
static uint8 sampling_frequency = 0; // Varaible that stores the sampling frequency at which the
                                     // accelerometer operates [Hz]   

// TEST
char msg[50] = {'\0'};
uint8 test_write_read = 0;
                                    

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
    
    // Read sampling frequency from EEPROM (adress 0x0000)
    sampling_frequency = EEPROM_ReadByte(STARTUP_REG);
    
    // The very first time the device is set up, or in case the device has been used for
    // something else previously, in that cell of the EEPROM we could have anything.. 
    // so check if it consistent with an allowed sampling frequency (and set the
    // counter accordingly) otherwise set it, by default, to the lowest (1 Hz)
    switch(sampling_frequency) {
    
        case A:
            count_push = 1;
            UART_PutString("1 Hz\r\n");
            break;
        case B:
            count_push = 2;
            UART_PutString("10 Hz\r\n");
            break;
        case C:
            count_push = 3;
            UART_PutString("25 Hz\r\n");
            break;
        case D:
            count_push = 4;
            UART_PutString("50 Hz\r\n");
            break;        
        case E:
            count_push = 5;
            UART_PutString("100 Hz\r\n");
            break;        
        case F:
            count_push = 6;
            UART_PutString("200 Hz\r\n");
            break;
        default:
            // Write default value on EEPROM
            EEPROM_UpdateTemperature();
            EEPROM_WriteByte(A,STARTUP_REG);
            
            test_write_read = EEPROM_ReadByte(STARTUP_REG);
            sprintf(msg, "Default value set: %d\r\n", test_write_read);
            UART_PutString(msg);
            sampling_frequency = A;            
            count_push = 1;
            break;
        
    }
    

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
                    sprintf(msg, "counter: %d\r\n", count_push);
                    UART_PutString(msg);                    
                    break;
                case 2:
                    sprintf(msg, "counter: %d\r\n", count_push);
                    UART_PutString(msg);
                    break;
                case 3:
                    sprintf(msg, "counter: %d\r\n", count_push);
                    UART_PutString(msg);
                    break;
                case 4:
                    sprintf(msg, "counter: %d\r\n", count_push);
                    UART_PutString(msg);
                    break;
                case 5:
                    sprintf(msg, "counter: %d\r\n", count_push);
                    UART_PutString(msg);
                    break;
                case 6:
                    sprintf(msg, "counter: %d\r\n", count_push);
                    UART_PutString(msg);
                    count_push = 0;
                    break;
                default:
                    UART_PutString("Error\r\n");
                    break;
            
            }
            
            
        }
        
    }
}

/* [] END OF FILE */
