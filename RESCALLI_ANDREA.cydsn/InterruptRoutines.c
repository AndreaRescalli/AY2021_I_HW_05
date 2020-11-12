/* ========================================
 *
 * Copyright LTEBS srl, 2020
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF LTEBS srl.
 *
 * \file  InterruptRoutines.c
 * \brief Source file including the ISR of the button, used to switch between operating frequencies
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
#include "InterruptRoutines.h"


// Definition of ISR that informs whether the button has been pressed
// in order to update the sampling frequency for the accelerometer
CY_ISR(Custom_ISR_Push) {

    /* 
     * No need to clear any interrupt source:
     * interrupt component is configured for RISING_EDGE mode.
     * PLEASE NOTE THAT AN INTERRUPT IS GENERATED, ONCE, AT THE PUSH OF THE BUTTON
     * So, even if we keep the button pressed, the sequence will be incremented
     * just by one step at the moment we first press the button.
    */
    
    // Set the flag that tells the main code the button has been pressed
    flag_push = 1;

}

/* [] END OF FILE */
