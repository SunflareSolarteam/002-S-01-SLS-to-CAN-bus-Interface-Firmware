/* 
 * File:   Temp_read.h
 * Author: ruurd
 *
 * Created on May 8, 2019, 5:06 PM
 */

#include "Temp_read.h"
#include "temp_lookup_table.h"
#include <stdint.h>
#include "mcc_generated_files/adc1.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/can1.h"

static volatile uint16_t raw_temp_1; //analog data
static volatile uint16_t temp_1 = 0;
static volatile uint16_t raw_temp_2; //analog data
static volatile uint16_t temp_2 = 0;

void Temp_read(void){
    raw_temp_1 = ADC1_Core0ConversionResultGet();
    raw_temp_2 = ADC1_Core1ConversionResultGet();   
    
 if (TEMP_LOOKUP_TABLE_INPUT_MIN <= raw_temp_1 && raw_temp_1 <= TEMP_LOOKUP_TABLE_INPUT_MAX) {
   temp_1 = temp_lookup_table[raw_temp_1 - TEMP_LOOKUP_TABLE_OFFSET];}
 else { // Error value
   temp_1 = 0xFFFF; }
    
if (TEMP_LOOKUP_TABLE_INPUT_MIN <= raw_temp_2 && raw_temp_2 <= TEMP_LOOKUP_TABLE_INPUT_MAX) {
   temp_2 = temp_lookup_table[raw_temp_2 - TEMP_LOOKUP_TABLE_OFFSET];}
else { // Error value
   temp_2 = 0xFFFF; }

    printf("Raw temp 1 %d\r\n", raw_temp_2);
    printf("Temp 1 %d\r\n", temp_2);

}

uint16_t get_raw_temp_1(void){
    return raw_temp_1;
}

uint16_t get_temp_1(void){
    return temp_1;
}

uint16_t get_raw_temp_2(void){
    return raw_temp_2;
}

uint16_t get_temp_2(void){
    return temp_2;
}