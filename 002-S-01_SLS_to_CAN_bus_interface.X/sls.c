/*
 * File:   sls.c
 * Author: Hylke
 *
 * Created on April 7, 2019, 11:44 AM
 */

#include "sls.h"
#include "slslookuptable.h"
#include <stdint.h>
#include "mcc_generated_files/uart1.h"
#include "mcc_generated_files/uart2.h"
#include "softwaretimer.h"

static int8_t sls_data_request_timer = SOFTWARETIMER_NONE;
static int8_t timeout_timer = SOFTWARETIMER_NONE;
static uint8_t sls_receive_status = 0;

static uint16_t sls_max_amps_100ma = 0;
static uint16_t sls_max_rpm = 0;

static uint32_t sls_status = 0;
static int16_t sls_temp_power_module_100mdeg = 0;
static int16_t sls_temp_electronics_100mdeg = 0;
static int16_t sls_temp_motor_1_100mdeg = 0;
static int16_t sls_temp_motor_2_100mdeg = 0;
static uint16_t sls_uzk_10mv = 0;
static int16_t sls_motor_current_100ma = 0;
static int16_t sls_input_current_100ma = 0;
static int16_t sls_rpm = 0;
static int16_t sls_max_rpm_limit = 0;
static int16_t sls_max_motor_current_limit_100ma = 0;
static int16_t sls_max_input_current_limit_100ma = 0;
static uint32_t sls_derating = 0;


void sls_init(void) {
    // Uart init is done in mcc
    sls_data_request_timer = softwaretimer_create(SOFTWARETIMER_CONTINUOUS_MODE);
    softwaretimer_start(sls_data_request_timer, 500);
    timeout_timer = softwaretimer_create(SOFTWARETIMER_SINGLE_MODE);
}

void sls_process(void) {
    uint8_t i, crc;
    uint8_t sls_rx_buffer[66];
    int32_t temp_s32;
    
    if (softwaretimer_get_expired(sls_data_request_timer)) {
        // Clear input buffer
        softwaretimer_start(timeout_timer, 100);
        while (!UART2_ReceiveBufferIsEmpty()) {
            UART2_Read();
            
            // Check for timeout
            if (softwaretimer_get_expired(timeout_timer)) {
                // Maybe set all data to 0?
                // Debug
                printf("Clear timeout\r\n");
                sls_receive_status = 1;
                return;
            }
        }
        
        // Request new data
        UART2_Write(0x21);  // "!"
        UART2_Write(0x03);  // 3d
        UART2_Write(0x53);  // "S"
        UART2_Write(0x77);  // CRC
        // Debug
        printf("Requesting new data\r\n");
        
        // Read 66 characters
        i = 0;
        softwaretimer_start(timeout_timer, 200);
        while (i < 66) {
            //Read Array from SLS
            if(!UART2_ReceiveBufferIsEmpty()){
                sls_rx_buffer[i] = UART2_Read();
                i++;
            }
            
            // Check for timeout
            if (softwaretimer_get_expired(timeout_timer)) {
                // Maybe set all data to 0?
                // Debug
                printf("Request timeout\r\n");
                sls_receive_status = 2;
                return;
            }
        }
        softwaretimer_stop(timeout_timer);
        
        // CRC check
        crc = 0;
        for (i = 0; i < 65; i++) {    
            crc += sls_rx_buffer[i];
        }
        if (crc != sls_rx_buffer[65]) {
            // Maybe set all data to 0?
            // Debug
            printf("CRC error\r\n");
            sls_receive_status = 3;
            return;
        }
        
        // Extract data
        sls_max_amps_100ma = (sls_rx_buffer[19] << 8 | sls_rx_buffer[20]);
        sls_max_rpm = (sls_rx_buffer[21] << 8 | sls_rx_buffer[22]);
        
        sls_temp_power_module_100mdeg = sls_temp_lookup_table[sls_rx_buffer[3]];
        sls_uzk_10mv = (sls_rx_buffer[5] << 8 | sls_rx_buffer[4]) * 66.11 * 100 / 1023;
        temp_s32 = ((uint32_t)(sls_rx_buffer[7] << 8 | sls_rx_buffer[6]) & 0x7FFF) * sls_max_amps_100ma / 4095;
        if ((sls_rx_buffer[7] << 8 | sls_rx_buffer[6]) & 0x8000) {
            temp_s32 = temp_s32 * -1;
        }
        sls_motor_current_100ma = temp_s32;
        //sls_motor_current_100ma = (int32_t)(sls_rx_buffer[7] << 8 | sls_rx_buffer[6]) * sls_max_amps_100ma / 4095;
        temp_s32 = ((uint32_t)(sls_rx_buffer[9] << 8 | sls_rx_buffer[8]) & 0x7FFF) * sls_max_rpm / 10922;
        if ((sls_rx_buffer[9] << 8 | sls_rx_buffer[8]) & 0x8000) {
            temp_s32 = temp_s32 * -1;
        }
        sls_rpm = temp_s32;
        sls_status = ((uint32_t)sls_rx_buffer[10] << 16 | sls_rx_buffer[11] << 8 | sls_rx_buffer[12]);
        sls_derating = ((uint32_t)sls_rx_buffer[16] << 16 | sls_rx_buffer[17] << 8 | sls_rx_buffer[18]);
        sls_max_rpm_limit = (int32_t)(sls_rx_buffer[29] << 8 | sls_rx_buffer[28]) * sls_max_rpm / 10922;
        sls_max_motor_current_limit_100ma = (int32_t)(sls_rx_buffer[31] << 8 | sls_rx_buffer[30]) * sls_max_amps_100ma / 4095;
        sls_max_input_current_limit_100ma = (int32_t)(sls_rx_buffer[33] << 8 | sls_rx_buffer[32]) * sls_max_amps_100ma / 4095;
        temp_s32 = ((uint32_t)(sls_rx_buffer[43] << 8 | sls_rx_buffer[42]) & 0x7FFF) * sls_max_amps_100ma / 4095;
        if ((sls_rx_buffer[43] << 8 | sls_rx_buffer[42]) & 0x8000) {
            temp_s32 = temp_s32 * -1;
        }
        sls_input_current_100ma = temp_s32;
        //sls_input_current_100ma = (uint32_t)(sls_rx_buffer[43] << 8 | sls_rx_buffer[42]) * sls_max_amps_100ma / 4095;
        sls_temp_electronics_100mdeg = sls_temp_lookup_table[sls_rx_buffer[60]];
        
        printf("max amps %d\r\n", sls_max_amps_100ma);
        printf("max rpm %d\r\n", sls_max_rpm);
        printf("temp power %d\r\n", sls_temp_power_module_100mdeg);
        printf("uzk %d\r\n", sls_uzk_10mv);
        printf("motor current %d\r\n", sls_motor_current_100ma);
        printf("rpm %d\r\n", sls_rpm);
        printf("status %lu\r\n", sls_status);
        printf("derating %lu\r\n", sls_derating);
        printf("max rpm limit %d\r\n", sls_max_rpm_limit);
        printf("max motor current limit %d\r\n", sls_max_motor_current_limit_100ma);
        printf("max input current limit %d\r\n", sls_max_input_current_limit_100ma);
        printf("input current %d\r\n", sls_input_current_100ma);
        printf("temp elec %d\r\n", sls_temp_electronics_100mdeg);
        
        sls_receive_status = 0;
    }
    return;
}

uint32_t get_sls_status(void) {
    return sls_status;
}

int16_t get_sls_temp_power_module_100mdeg(void) {
    return sls_temp_power_module_100mdeg;
}

int16_t get_sls_temp_electronics_100mdeg(void) {
    return sls_temp_electronics_100mdeg;
}

int16_t get_sls_temp_motor_1_100mdeg(void) {
    return sls_temp_motor_1_100mdeg;
}

int16_t get_sls_temp_motor_2_100mdeg(void) {
    return sls_temp_motor_2_100mdeg;
}

uint16_t get_sls_uzk_10mv(void) {
    return sls_uzk_10mv;
}

int16_t get_sls_motor_current_100ma(void) {
    return sls_motor_current_100ma;
}

int16_t get_sls_input_current_100ma(void) {
    return sls_input_current_100ma;
}

int16_t get_sls_rpm(void) {
    return sls_rpm;
}

int16_t get_sls_max_rpm_limit(void) {
    return sls_max_rpm_limit;
}

int16_t get_sls_max_motor_current_limit_100ma(void) {
    return sls_max_motor_current_limit_100ma;
}

int16_t get_sls_max_input_current_limit_100ma(void) {
    return sls_max_input_current_limit_100ma;
}

uint32_t get_sls_derating(void) {
    return sls_derating;
}

uint8_t get_sls_receive_status(void) {
    return sls_receive_status;
}
