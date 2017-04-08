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
#include <peopleCounting.h>
#include <numpyInC.h>

static uint16_t PIXEL_BUFFER[NUM_PIXELS];
static uint16_t *MEDIAN_FILTER_POINTERS_BUFF[MEDIAN_FILTER_POINTERS_BUFF_SIZE];
static uint16_t COUNT_BUFF[COUNT_BUFF_SIZE];




int main (void) {
	system_init();
	init_uart();
	init_grideye();
	delay_init();
	median_buffer_init(MEDIAN_FILTER_POINTERS_BUFF);
	initialize_count();
	uint16_t frame_count = 0;
	
	initialize_count();
	
	/* Insert application code here, after the board has been initialized. */

	/* This skeleton code simply sets the LED to the state of the button. */
	while (1) {
		/* Is button pressed? */
		if (port_pin_get_input_level(BUTTON_0_PIN) == BUTTON_0_ACTIVE) {
			if (ge_is_sleeping()) {
				ge_set_mode(GE_MODE_NORMAL);
				port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);
			} else {
				ge_set_mode(GE_MODE_SLEEP);
				port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);
			}
			delay_ms(100);
		}

		if (!ge_is_sleeping()) {
			// Send frame
			uint8_t buffer[512];

			ge_get_frame(PIXEL_BUFFER);
			start_count(frame_count, PIXEL_BUFFER, MEDIAN_FILTER_POINTERS_BUFF, COUNT_BUFF);
			frame_count++;
		}
	}
	return 0;
}
