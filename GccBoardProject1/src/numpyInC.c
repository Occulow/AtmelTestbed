/*
 * numpyInC.c
 *
 * Created: 3/24/2017 8:59 PM
 * Author: Udaya Malik
 */ 

#include <asf.h>
#include <string.h>

struct usart_module usart_instance;

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


int * fromString(char* data, const char* delim) {
	int * array;
	char * pt = strtok(data, delim);
	uint8_t debug1[] = "NOW is this printing\r\n";
	uart_out(debug1, sizeof(debug1) - 1);
	if (pt != NULL) {
		uint8_t debug2[] = "is this printing\r\n";
		uart_out(debug2, sizeof(debug2) - 1);
	}else {
		uint8_t debug3[] = "NOW 2 is this printing\r\n";
		uart_out(debug3, sizeof(debug3) - 1);
	}
	uart_out((uint8_t *) pt, sizeof(pt) - 1);
	int i = 0;
	while (pt != NULL) {
		array[i] = atoi(pt);
		uint8_t msg = array[i];
		uart_out(msg, sizeof(msg) - 1);
		pt = strtok(NULL, delim);
	}
	return array;
}

uint8_t * dstack(uint8_t * array1, uint8_t * array2) {
	uart_out(array2, sizeof(array1)-1);
	uint8_t * newArray[3];
	uint8_t i = 0;
	uint8_t i1=0;
	uint8_t i2 = 0;
	uint8_t pt1, pt2;
	pt1 = array1[i1];
	pt2 = array2[i2];
	
	while (pt1 != NULL && pt2 != NULL){
		uint8_t pt3[2];
		pt3[0] = pt1;
		pt3[1] = pt2;
		uart_out(pt3, sizeof(pt3));
		newArray[i] = pt3;
		uart_out(newArray[i], sizeof(newArray[i])-1);
		i ++;
		pt1 = array1[i1++];
		pt2 = array2[i2++];
	}
	uint8_t debug[] = "is this printing\r\n";
	uart_out(debug,sizeof(debug)-1);
	uart_out(*newArray, sizeof(*newArray)-1);
	return (newArray);
}

