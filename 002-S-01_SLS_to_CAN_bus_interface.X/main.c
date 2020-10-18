/**
  Generated main.c file from MPLAB Code Configurator

  @Company
    Microchip Technology Inc.

  @File Name
    main.c

  @Summary
    This is the generated main.c using PIC24 / dsPIC33 / PIC32MM MCUs.

  @Description
    This source file provides main entry point for system intialization and application code development.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.95-b-SNAPSHOT
        Device            :  dsPIC33EP128GS804
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.36
        MPLAB 	          :  MPLAB X v5.10
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

/**
  Section: Included Files
*/
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include <stdint.h>
#include "softwaretimer.h"
#include "sls.h"
#include "canbus.h"
#include "Temp_read.h"

/*
 * Main application
 */
int main(void) {
    int8_t led_on_timer = SOFTWARETIMER_NONE, led_off_timer = SOFTWARETIMER_NONE;
    
    IO_LED_R_SetLow();
    IO_LED_G_SetHigh();
    
    // initialize the device
    SYSTEM_Initialize();
    softwaretimer_init();
    sls_init();
    can_bus_init();
    
    IO_LED_R_SetHigh();
    
    led_on_timer = softwaretimer_create(SOFTWARETIMER_CONTINUOUS_MODE);
    softwaretimer_start(led_on_timer, 1000);
    led_off_timer = softwaretimer_create(SOFTWARETIMER_SINGLE_MODE);
    
    while (1) {
        sls_process();
        can_bus_process();
        Temp_read();
        if (softwaretimer_get_expired(led_on_timer)) {
            IO_LED_G_SetLow();
            softwaretimer_start(led_off_timer, 25);
        }
        if (softwaretimer_get_expired(led_off_timer)) {
            IO_LED_G_SetHigh();
        }
        if (get_sls_receive_status()) {
            IO_LED_R_SetLow();
        } else {
            IO_LED_R_SetHigh();
        }
    }
    return 1; 
}


