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
static uint16_t frame1[NUM_PIXELS];
static uint16_t* MEDIAN_FILTER_POINTERS_BUFF[MEDIAN_FILTER_POINTERS_BUFF_SIZE];
static uint16_t frame[NUM_RAW_FRAMES][NUM_PIXELS];
static uint16_t COUNT_BUFF[COUNT_BUFF_SIZE];
static uint16_t frame_conv[GRID_SIZE][GRID_SIZE];
static struct keepCount count;


void median_buffer_init(uint16_t* MEDIAN_FILTER_POINTERS_BUFF[], uint16_t** frame) {
	for (int i = 0; i < NUM_RAW_FRAMES; i++) {
		MEDIAN_FILTER_POINTERS_BUFF[i] = &frame[i];
	}
	return;
}



int main (void) {
	system_init();
	init_uart();
	init_grideye();
	delay_init();
	median_buffer_init(MEDIAN_FILTER_POINTERS_BUFF, frame);
	
	
	count->countIn = 0;
	count->countOut = 0;
	uint16_t frameCount = 0;
	uint16_t lastFrame = 0;
	count->TRIGGER_COLUNM[0][0] = TRIGGER_COLUNM_2;
	count->TRIGGER_COLUNM[0][1] = CHECK_OFFSET_2;
	count->TRIGGER_COLUNM[1][0] = TRIGGER_COLUNM_5;
	count->TRIGGER_COLUNM[1][1] = CHECK_OFFSET_5;
	count->counted = false;
	
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
			
			//double ambient_temp = get_ambient_temp();
			
			//uint16_t len = snprintf((char *) buffer, sizeof(buffer), "It is %.2lf degrees Celsius.\r\n", ambient_temp);
			//uart_out(buffer, len);
			ge_get_frame(PIXEL_BUFFER);
			uint16_t size = 0;
			for (int i = 0; i < NUM_PIXELS; i++) {
				size += sprintf((char *) (buffer + size), "%d,", PIXEL_BUFFER[i]);
			}
			buffer[size-1] = '\r';
			buffer[size] = '\n';
			frameCount++;
			if (frameCount <= NUM_RAW_FRAMES) {
				initialize_mem(PIXEL_BUFFER, frameCount);
			} else {
				dstack(MEDIAN_FILTER_POINTERS_BUFF, PIXEL_BUFFER);;
			}
			uint16_t *frame_median = median_frame(MEDIAN_FILTER_POINTERS_BUFF, frame1);
			
			count = countPeople(&count, frame_median, frameCount, COUNT_BUFF, lastFrame);
			uart_out(buffer, size+1);
		}
	}
}
