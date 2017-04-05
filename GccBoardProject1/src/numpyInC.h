/*
 * numpyInC.h
 *
 * Created: 3/24/2017 8:59 PM
 * Author: Udaya Malik
 */ 

#ifndef NUMPYINC_H_
#define NUMPYINC_H_

#include <asf.h>
#include <string.h>

void init_uart(void);
void uart_out(const uint8_t *string, uint16_t length);
static uint16_t frame_median[NUM_PIXELS];
static uint16_t array[NUM_RAW_FRAMES];

void dstack(uint16_t * MEDIAN_FILTER_POINTERS_BUFF[], uint16_t PIXEL_BUFFER[]);
float median(uint16_t array[], int len);
uint16_t* median_frame(uint16_t* MEDIAN_FILTER_POINTERS_BUFF[], uint16_t frame1[]);

#endif 
