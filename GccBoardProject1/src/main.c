/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * This is a bare minimum user application template.
 *
 * For documentation of the board, go \ref group_common_boards "here" for a link
 * to the board-specific documentation.
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
 * -# Basic usage of on-board LED and button
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include <grideye.h>

static uint16_t PIXEL_BUFFER[NUM_PIXELS];

struct usart_module usart_instance;

void init_uart(void);
void uart_out(const uint8_t *string, uint16_t length);


void init_uart() {
	struct usart_config config_usart;
	usart_get_config_defaults(&config_usart);
	
	config_usart.baudrate    = 115200;
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

int main (void)
{
	system_init();
	init_uart();
	init_grideye();
	delay_init();
	
	/* Insert application code here, after the board has been initialized. */

	/* This skeleton code simply sets the LED to the state of the button. */
	while (1) {
		/* Is button pressed? */
		//if (port_pin_get_input_level(BUTTON_0_PIN) == BUTTON_0_ACTIVE) {
			/* Yes, so turn LED on. */
			//port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);
			
			// Send frame
			uint8_t buffer[512];
			//double ambient_temp = get_ambient_temp();

			//uint16_t len = snprintf((char *) buffer, sizeof(buffer), "It is %.2lf degrees Celsius.\r\n", ambient_temp);
			//uart_out(buffer, len);
			get_frame(PIXEL_BUFFER);
			uint16_t size = 0;
			for (int i = 0; i < NUM_PIXELS; i++) {
				size += sprintf((char *) (buffer + size), "%d,", PIXEL_BUFFER[i]);
			}
			buffer[size-1] = '\r';
			buffer[size] = '\n';
			uart_out(buffer, size+1);
			//delay_ms(100);
		//} else {
			/* No, so turn LED off. */
		//	port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);
		//}
	}
}
