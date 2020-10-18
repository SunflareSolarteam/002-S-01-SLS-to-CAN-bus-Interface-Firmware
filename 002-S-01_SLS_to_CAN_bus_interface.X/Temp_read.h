/* 
 * File:   Temp_read.h
 * Author: ruurd
 *
 * Created on May 8, 2019, 5:06 PM
 */

#ifndef TEMP_READ_H
#define	TEMP_READ_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdint.h> 
    
void Temp_read(void);
uint16_t get_raw_temp_1(void);

uint16_t get_temp_1(void);

uint16_t get_raw_temp_2(void);

uint16_t get_temp_2(void);


#ifdef	__cplusplus
}
#endif

#endif	/* TEMP_READ_H */

