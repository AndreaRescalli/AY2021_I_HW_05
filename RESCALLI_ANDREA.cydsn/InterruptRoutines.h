/* ========================================
 *
 * Copyright LTEBS srl, 2020
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF LTEBS srl.
 *
 * \file  InterruptRoutines.h
 * \brief Header file including the ISR of the button, used to switch between operating frequencies
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

#ifndef __INTERRUPT_ROUTINES_H_
    #define __INTERRUPT_ROUTINES_H_
    
    #include "cytypes.h"
    
    // Globals
    volatile uint8_t flag_push; // Flag that informs whether the button has been pressed
    
    // Declaration of ISR that informs whether the button has been pressed
    // in order to update the sampling frequency for the accelerometer
    CY_ISR_PROTO(Custom_ISR_Push);
    
#endif

/* [] END OF FILE */
