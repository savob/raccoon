 /*
 * MAIN Generated Driver File
 * 
 * @file main.c
 * 
 * @defgroup main MAIN
 * 
 * @brief This is the generated driver implementation file for the MAIN driver.
 *
 * @version MAIN Driver Version 1.0.2
 *
 * @version Package Version: 3.1.2
*/

/*
© [2025] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/
#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/timer/delay.h"
#include "mcc_generated_files/touch/touch.h"

// Needed to get some variables from touch system
extern volatile uint8_t measurement_done_touch;
extern volatile uint8_t time_to_measure_touch_flag;

const uint16_t FLASH_PERIOD_MS = 3000;
const uint8_t START_FLASHES = 5;


int main(void)
{
    SYSTEM_Initialize();
    
    for (uint8_t i = 0; i < START_FLASHES; i++) {
//        LED_SetHigh();
        DELAY_milliseconds(FLASH_PERIOD_MS / 5);
//        LED_SetLow();
        DELAY_milliseconds(FLASH_PERIOD_MS / 5); 
    }
    
    while(1) {
        touch_process();
#if (DEF_TOUCH_LOWPOWER_ENABLE == 1u)
        if (time_to_measure_touch_flag != 1u) {
             __asm__ __volatile__ ( "sleep" "\n\t" :: );
        }
#endif
        
        if(measurement_done_touch == 1) {
            measurement_done_touch = 0;
            
            uint8_t key_status = get_sensor_state(0) & KEY_TOUCHED_MASK;
            if (0u == key_status) continue;
            
//            LED_SetHigh();
            DELAY_milliseconds(FLASH_PERIOD_MS);
//            LED_SetLow();
            // DELAY_milliseconds(FLASH_PERIOD_MS / 10); // Stay off for a bit?
        }
    }    
}