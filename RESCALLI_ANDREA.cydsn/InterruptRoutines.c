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
