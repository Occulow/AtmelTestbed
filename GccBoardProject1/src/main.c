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
static uint8_t current_mode = 0;
struct usart_module usart_instance;

void init_uart(void);
void init_standby(void);
void uart_out(const uint8_t *string, uint16_t length);
void wakeup_device(void);
void sleep_device(void);
void configure_extint_channel(void);
void configure_extint_callbacks(void);


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

void configure_extint_channel(void) {
	struct extint_chan_conf config_extint_chan;
	extint_chan_get_config_defaults(&config_extint_chan);
	
	config_extint_chan.gpio_pin = BUTTON_0_EIC_PIN;
	config_extint_chan.gpio_pin_mux = BUTTON_0_EIC_MUX;
	config_extint_chan.gpio_pin_pull = EXTINT_PULL_UP;
	config_extint_chan.detection_criteria = EXTINT_DETECT_FALLING;

	extint_chan_set_config(BUTTON_0_EIC_LINE, &config_extint_chan);
	while(extint_chan_is_detected(BUTTON_0_EIC_LINE)) {
		extint_chan_clear_detected(BUTTON_0_EIC_LINE);
	}
	extint_register_callback(wakeup_device, BUTTON_0_EIC_LINE, EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(BUTTON_0_EIC_LINE, EXTINT_CALLBACK_TYPE_DETECT);
}

void configure_extint_callbacks(void) {
}

void init_standby(void) {
	struct system_standby_config standby_conf;
	system_standby_get_config_defaults(&standby_conf);
	system_standby_set_config(&standby_conf);
}

void wakeup_device(void) {
	if (current_mode == 0) {
		current_mode = 1;
	} else {
		current_mode = 0;
		LED_Off(LED0_GPIO);
	}
}

void sleep_device(void) {
	current_mode = 1;
	LED_On(LED0_GPIO);
	system_set_sleepmode(SYSTEM_SLEEPMODE_STANDBY);
	system_sleep();
}

int main (void) {
	system_init();
	init_uart();
	// init_grideye();
	delay_init();
	init_standby();
	
	configure_extint_channel();
	configure_extint_callbacks();
	uint8_t hearbeat_msg[] = "Awake.\r\n";
	
	while (1) {
		uart_out(hearbeat_msg, sizeof(hearbeat_msg));
		delay_s(1);
		if (current_mode == 1) {
			sleep_device();
		}
	}
}
