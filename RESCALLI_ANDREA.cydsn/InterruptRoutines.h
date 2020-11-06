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

#ifndef __INTERRUPT_ROUTINES_H_
    #define __INTERRUPT_ROUTINES_H_
    
    #include "cytypes.h"
    
    // Globals
    volatile uint8_t flag_push;
    
    // Declaration of ISR that informs whether the button has been pressed
    // in order to update the sampling frequency for the accelerometer
    CY_ISR_PROTO(Custom_ISR_Push);
    
#endif

/* [] END OF FILE */
