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

#ifndef __UTILITY_H_
    #define __UTILITY_H_
    
    // Includes
    #include "cytypes.h"
    
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
    
    
    /*
     * Declaration of function that sets the operating frequency of the LIS3DH.
     * As parameters it requires:
     * - current value of the Control Register 1 (where the frequency is set)
     * - desired value to be written in the register
     *
    */
    void SetOperatingFrequency(uint8_t register_value, 
                               uint8_t desired_value);
    
#endif

/* [] END OF FILE */
