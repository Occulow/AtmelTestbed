/*
 * numpyInC.h
 *
 * Created: 3/24/2017 8:59 PM
 * Author: Udaya Malik
 */ 

#ifndef NUMPYINC_H_
#define NUMPYINC_H_

#define MEDIAN_POINTER_BUFF_LAST_INDEX 9

#include <asf.h>
#include <string.h>
#include <peopleCounting.h>
#include <grideye.h>

void init_uart(void);
void uart_out(const uint8_t *string, uint16_t length);
void dstack(uint16_t * median_filter_buffer[], uint16_t pixel_buff[]);
float median(int len);
uint16_t* median_frame(uint16_t* median_filter_buffer[]);
void init_uart(void);
void uart_out(const uint8_t *string, uint16_t length);

#endif 
