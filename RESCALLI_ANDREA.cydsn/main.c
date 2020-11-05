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

// Globals
static uint8 count_push         = 0; // Flag that informs whether the button has been pressed
static uint8 sampling_frequency = 0; // Varaible that stores the sampling frequency at which the
                                     // accelerometer operates [Hz]   

// TEST
char msg[50] = {'\0'};
uint8 test_write_read = 0;
                                    

int main(void) {

    CyGlobalIntEnable; /* Enable global interrupts. */
    
    // Start components (EEPROM, UART)
    EEPROM_Start();
    UART_Start();
    
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
