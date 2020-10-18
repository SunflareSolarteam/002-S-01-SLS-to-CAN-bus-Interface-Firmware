/*
 * File:   canbus.c
 * Author: Hylke
 *
 * Created on February 19, 2019, 7:15 PM
 */


#include "canbus.h"

#include "mcc_generated_files/can_types.h"
#include "mcc_generated_files/uart1.h"
#include "softwaretimer.h"
#include "sls.h"
#include "Temp_read.h"

static int8_t can_bus_transmit_timer = SOFTWARETIMER_NONE;
static uint8_t can_bus_message_number_to_be_send = 0;

static void can_bus_transmit_periodic_messages(void) {
    uCAN_MSG msg;
    
    // Always the same for all messages
    msg.frame.msgtype = CAN_MSG_DATA;
    msg.frame.idType = CAN_FRAME_STD;
    msg.frame.dlc = 8;
    
    // The can bus peripheral only has 8 transmit buffers.
    // When we want to send more than 8 in a short time the buffers will fill up and new messages wont be put in the buffer.
    // The code below starts with message 0 and only move to the next message if it was successfully put in the buffer.
    switch (can_bus_message_number_to_be_send) {
        case 0:
            msg.frame.id = 0x180 | CAN_BUS_NODE_ID;
            msg.frame.data0 = CAN_BUS_NODE_ID;
            msg.frame.data1 = 0x00;
            msg.frame.data2 = 0x20;
            msg.frame.data3 = 0x01;
            msg.frame.data4 = (get_sls_status() >> 0) & 0xFF;
            msg.frame.data5 = (get_sls_status() >> 8) & 0xFF;
            msg.frame.data6 = (get_sls_status() >> 16) & 0xFF;
            msg.frame.data7 = (get_sls_status() >> 24) & 0xFF;
            if (CAN1_transmit(CAN_PRIORITY_MEDIUM, &msg)) {
                can_bus_message_number_to_be_send++;
            }
            break;
        case 1:
            msg.frame.id = 0x180 | CAN_BUS_NODE_ID;
            msg.frame.data0 = CAN_BUS_NODE_ID;
            msg.frame.data1 = 0x01;
            msg.frame.data2 = 0x20;
            msg.frame.data3 = 0x01;
            msg.frame.data4 = ((uint32_t)get_sls_derating() >> 0) & 0xFF;
            msg.frame.data5 = ((uint32_t)get_sls_derating() >> 8) & 0xFF;
            msg.frame.data6 = ((uint32_t)get_sls_derating() >> 16) & 0xFF;
            msg.frame.data7 = ((uint32_t)get_sls_derating() >> 24) & 0xFF;
            if (CAN1_transmit(CAN_PRIORITY_MEDIUM, &msg)) {
                can_bus_message_number_to_be_send++;
            }
            break;
        case 2:
            msg.frame.id = 0x280 | CAN_BUS_NODE_ID;
            msg.frame.data0 = CAN_BUS_NODE_ID;
            msg.frame.data1 = 0x00;
            msg.frame.data2 = 0x20;
            msg.frame.data3 = 0x01;
            msg.frame.data4 = ((uint16_t)get_sls_temp_power_module_100mdeg() >> 0) & 0xFF;
            msg.frame.data5 = ((uint16_t)get_sls_temp_power_module_100mdeg() >> 8) & 0xFF;
            msg.frame.data6 = 0x00;
            msg.frame.data7 = 0x00;
            if (CAN1_transmit(CAN_PRIORITY_MEDIUM, &msg)) {
                can_bus_message_number_to_be_send++;
            }
            break;
        case 3:
            msg.frame.id = 0x280 | CAN_BUS_NODE_ID;
            msg.frame.data0 = CAN_BUS_NODE_ID;
            msg.frame.data1 = 0x00;
            msg.frame.data2 = 0x20;
            msg.frame.data3 = 0x02;
            msg.frame.data4 = ((uint16_t)get_sls_temp_electronics_100mdeg() >> 0) & 0xFF;
            msg.frame.data5 = ((uint16_t)get_sls_temp_electronics_100mdeg() >> 8) & 0xFF;
            msg.frame.data6 = 0x00;
            msg.frame.data7 = 0x00;
            if (CAN1_transmit(CAN_PRIORITY_MEDIUM, &msg)) {
                can_bus_message_number_to_be_send++;
            }
            break;
        case 4:
            msg.frame.id = 0x280 | CAN_BUS_NODE_ID;
            msg.frame.data0 = CAN_BUS_NODE_ID;
            msg.frame.data1 = 0x01;
            msg.frame.data2 = 0x20;
            msg.frame.data3 = 0x01;
            msg.frame.data4 = ((uint16_t) get_temp_1()>>0)& 0xFF;
            msg.frame.data5 = ((uint16_t) get_temp_1()>>8)& 0xFF;
            msg.frame.data6 = 0x00;
            msg.frame.data7 = 0x00;
            if (CAN1_transmit(CAN_PRIORITY_MEDIUM, &msg)) {
                can_bus_message_number_to_be_send++;
            }
            break;
        case 5:
            msg.frame.id = 0x280 | CAN_BUS_NODE_ID;
            msg.frame.data0 = CAN_BUS_NODE_ID;
            msg.frame.data1 = 0x01;
            msg.frame.data2 = 0x20;
            msg.frame.data3 = 0x02;
            msg.frame.data4 = ((uint16_t) get_temp_2()>>0)& 0xFF;
            msg.frame.data5 = ((uint16_t) get_temp_2()>>8)& 0xFF;
            msg.frame.data6 = 0x00;
            msg.frame.data7 = 0x00;
            if (CAN1_transmit(CAN_PRIORITY_MEDIUM, &msg)) {
                can_bus_message_number_to_be_send++;
            }
            break;
        case 6:
            msg.frame.id = 0x380 | CAN_BUS_NODE_ID;
            msg.frame.data0 = CAN_BUS_NODE_ID;
            msg.frame.data1 = 0x00;
            msg.frame.data2 = 0x20;
            msg.frame.data3 = 0x01;
            msg.frame.data4 = (get_sls_uzk_10mv() >> 0) & 0xFF;
            msg.frame.data5 = (get_sls_uzk_10mv() >> 8) & 0xFF;
            msg.frame.data6 = 0x00;
            msg.frame.data7 = 0x00;
            if (CAN1_transmit(CAN_PRIORITY_MEDIUM, &msg)) {
                can_bus_message_number_to_be_send++;
            }
            break;
        case 7:
            msg.frame.id = 0x380 | CAN_BUS_NODE_ID;
            msg.frame.data0 = CAN_BUS_NODE_ID;
            msg.frame.data1 = 0x01;
            msg.frame.data2 = 0x20;
            msg.frame.data3 = 0x01;
            msg.frame.data4 = ((uint16_t)get_sls_motor_current_100ma() >> 0) & 0xFF;
            msg.frame.data5 = ((uint16_t)get_sls_motor_current_100ma() >> 8) & 0xFF;
            msg.frame.data6 = 0x00;
            msg.frame.data7 = 0x00;
            if (CAN1_transmit(CAN_PRIORITY_MEDIUM, &msg)) {
                can_bus_message_number_to_be_send++;
            }
            break;
        case 8:
            msg.frame.id = 0x380 | CAN_BUS_NODE_ID;
            msg.frame.data0 = CAN_BUS_NODE_ID;
            msg.frame.data1 = 0x02;
            msg.frame.data2 = 0x20;
            msg.frame.data3 = 0x01;
            msg.frame.data4 = ((uint16_t)get_sls_input_current_100ma() >> 0) & 0xFF;
            msg.frame.data5 = ((uint16_t)get_sls_input_current_100ma() >> 8) & 0xFF;
            msg.frame.data6 = 0x00;
            msg.frame.data7 = 0x00;
            if (CAN1_transmit(CAN_PRIORITY_MEDIUM, &msg)) {
                can_bus_message_number_to_be_send++;
            }
            break;
        case 9:
            msg.frame.id = 0x380 | CAN_BUS_NODE_ID;
            msg.frame.data0 = CAN_BUS_NODE_ID;
            msg.frame.data1 = 0x03;
            msg.frame.data2 = 0x20;
            msg.frame.data3 = 0x01;
            msg.frame.data4 = ((uint16_t)get_sls_rpm() >> 0) & 0xFF;
            msg.frame.data5 = ((uint16_t)get_sls_rpm() >> 8) & 0xFF;
            msg.frame.data6 = 0x00;
            msg.frame.data7 = 0x00;
            if (CAN1_transmit(CAN_PRIORITY_MEDIUM, &msg)) {
                can_bus_message_number_to_be_send++;
            }
            break;
        case 10:
            msg.frame.id = 0x480 | CAN_BUS_NODE_ID;
            msg.frame.data0 = CAN_BUS_NODE_ID;
            msg.frame.data1 = 0x01;
            msg.frame.data2 = 0x20;
            msg.frame.data3 = 0x01;
            msg.frame.data4 = ((uint16_t)get_sls_max_motor_current_limit_100ma() >> 0) & 0xFF;
            msg.frame.data5 = ((uint16_t)get_sls_max_motor_current_limit_100ma() >> 8) & 0xFF;
            msg.frame.data6 = 0x00;
            msg.frame.data7 = 0x00;
            if (CAN1_transmit(CAN_PRIORITY_MEDIUM, &msg)) {
                can_bus_message_number_to_be_send++;
            }
            break;
        case 11:
            msg.frame.id = 0x480 | CAN_BUS_NODE_ID;
            msg.frame.data0 = CAN_BUS_NODE_ID;
            msg.frame.data1 = 0x02;
            msg.frame.data2 = 0x20;
            msg.frame.data3 = 0x01;
            msg.frame.data4 = ((uint16_t)get_sls_max_input_current_limit_100ma() >> 0) & 0xFF;
            msg.frame.data5 = ((uint16_t)get_sls_max_input_current_limit_100ma() >> 8) & 0xFF;
            msg.frame.data6 = 0x00;
            msg.frame.data7 = 0x00;
            if (CAN1_transmit(CAN_PRIORITY_MEDIUM, &msg)) {
                can_bus_message_number_to_be_send++;
            }
            break;
        case 12:
            msg.frame.id = 0x480 | CAN_BUS_NODE_ID;
            msg.frame.data0 = CAN_BUS_NODE_ID;
            msg.frame.data1 = 0x03;
            msg.frame.data2 = 0x20;
            msg.frame.data3 = 0x01;
            msg.frame.data4 = (get_sls_max_rpm_limit() >> 0) & 0xFF;
            msg.frame.data5 = (get_sls_max_rpm_limit() >> 8) & 0xFF;
            msg.frame.data6 = 0x00;
            msg.frame.data7 = 0x00;
            if (CAN1_transmit(CAN_PRIORITY_MEDIUM, &msg)) {
                can_bus_message_number_to_be_send++;
            }
            break;
        default:
            // All messages have been send.
            // Do nothing
            break;
    }
}

void can_bus_init(void) {
    // Enable the CAN bus
    CAN1_TransmitEnable();
    //CAN1_ReceiveEnable();
    can_bus_transmit_timer = softwaretimer_create(SOFTWARETIMER_CONTINUOUS_MODE);
    softwaretimer_start(can_bus_transmit_timer, CAN_BUS_SEND_PERIOD_MS);
}

// Needs to be called in the main loop
void can_bus_process(void) {
    // Read can bus message
    // No messages needs to be received
    // Transmit messages
    can_bus_transmit_periodic_messages();
    
    if (softwaretimer_get_expired(can_bus_transmit_timer)) {
        // Start sending periodic messages from the start
        can_bus_message_number_to_be_send = 0;
    }
}

