/*
 * numpyInC.c
 *
 * Created: 3/24/2017 8:59 PM
 * Author: Udaya Malik
 */ 

#include <asf.h>
#include <string.h>
#include <numpyInC.h>

struct usart_module usart_instance;
static uint16_t FRAME_MEDIAN[NUM_PIXELS];
static uint16_t ARRAY[NUM_RAW_FRAMES];
static uint16_t FRAME_TEMP[NUM_PIXELS];



void init_uart() {
	struct usart_config config_usart;
	usart_get_config_defaults(&config_usart);
	
	config_usart.baudrate    = 9600;
	config_usart.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
	config_usart.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
	config_usart.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
	config_usart.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
	config_usart.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;
	
	while (usart_init(&usart_instance, EDBG_CDC_MODULE, &config_usart) != STATUS_OK) {
	}
	
	usart_enable(&usart_instance);
}

void uart_out(const uint8_t *string, uint16_t length) {
	while(usart_write_buffer_wait(&usart_instance, string, length) != STATUS_OK){}
}


void dstack(uint16_t * median_filter_buffer[], uint16_t pixel_buff[]) {
	uint16_t *oldFrame= median_filter_buffer[0];
	for (int i = 0; i < NUM_PIXELS; i++){
		oldFrame[i] = pixel_buff[i];
	}
	for (int i = 0; i < MEDIAN_FILTER_POINTERS_BUFF_SIZE - 1; i++) {
		median_filter_buffer[i] = median_filter_buffer[i+1];
	}
	median_filter_buffer[MEDIAN_POINTER_BUFF_LAST_INDEX] = &oldFrame[0];
	return;
}

uint16_t median(int len) {
	uint16_t temp;
	for (int i = 0; i < len; i++) {
		for (int j = i+1; j < len; j++) {
			if (ARRAY[j] < ARRAY[i]) {
				temp = ARRAY[i];
				ARRAY[i] = ARRAY[j];
				ARRAY[j] = temp;
			}
		}
	}
	
	if (len % 2 == 0) {
		return ((ARRAY[len/2] + ARRAY[len/2 - 1])/2);
		} else {
		return ARRAY[len/2];
	}
	
}

uint16_t* median_frame(uint16_t* median_filter_buffer[]) {
	for (int j = 0; j < NUM_PIXELS; j++) {
		for (int i = 0; i < MEDIAN_FILTER_POINTERS_BUFF_SIZE; i++) {
			FRAME_TEMP = median_filter_buffer[i];
			ARRAY[i] = FRAME_TEMP[j];
		}
		FRAME_MEDIAN[j] = median(NUM_RAW_FRAMES);
	}
	return &FRAME_MEDIAN[0];
}