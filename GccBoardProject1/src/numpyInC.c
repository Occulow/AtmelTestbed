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
static uint16_t frame_median[NUM_PIXELS];
static uint16_t array[NUM_RAW_FRAMES];

void init_uart(void);
void uart_out(const uint8_t *string, uint16_t length);


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


void dstack(uint16_t * MEDIAN_FILTER_POINTERS_BUFF[], uint16_t PIXEL_BUFFER[]) {
	uint16_t *oldFrame= MEDIAN_FILTER_POINTERS_BUFF[0];
	for (int i = 0; i < NUM_PIXELS; i++){
		oldFrame[i] = PIXEL_BUFFER[i];
	}
	for (int i = 0; i < MEDIAN_FILTER_POINTERS_BUFF_SIZE - 1; i++) {
		MEDIAN_FILTER_POINTERS_BUFF[i] = MEDIAN_FILTER_POINTERS_BUFF[i+1];
	}
	MEDIAN_FILTER_POINTERS_BUFF[9] = &oldFrame[0];
	return;
}

float median(uint16_t array[], int len) {
	float temp;
	for (int i = 0; i < len; i++) {
		for (int j = i+1; j < len; j++) {
			if (array[j] < array[i]) {
				temp = array[i];
				array[i] = array[j];
				array[j] = temp;
			}
		}
	}
	
	if
	(len % 2 == 0) {
		return ((array[len/2] + array[len/2 - 1])/2.0);
		} else {
		return array[len/2];
	}
	
}

uint16_t* median_frame(uint16_t* MEDIAN_FILTER_POINTERS_BUFF[], uint16_t frame1[]) {
	int j = 0;
	while (j < NUM_PIXELS) {
		for (int i = 0; i < MEDIAN_FILTER_POINTERS_BUFF_SIZE; i++) {
			frame1 = MEDIAN_FILTER_POINTERS_BUFF[i];
			array[i] = frame1[i];
		}
		for (int i = 0; i < 10; i++) {
		}
		frame_median[j] = median(array, NUM_RAW_FRAMES);
		j++;
	}
	return &frame_median[0];
}