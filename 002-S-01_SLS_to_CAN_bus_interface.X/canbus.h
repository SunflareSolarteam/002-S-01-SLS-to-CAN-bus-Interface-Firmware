// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef CANBUS_H
#define	CANBUS_H

#include <stdint.h>

#define CAN_BUS_SEND_PERIOD_MS  500
#define CAN_BUS_NODE_ID         0x10

// Initializes the can bus.
void can_bus_init(void);

// Sends and receives messages.
// Should be called once per main loop.
void can_bus_process(void);

#endif	
