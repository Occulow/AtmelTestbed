/*
 * CFile1.c
 *
 * Created: 3/24/2017 11:52:40 AM
 *  Author: mazacar8
 */

#include <asf.h>
#include "uart.h"
#include <stdio.h>

size_t readBuffOffset = 0;
uint8_t readBuf[READ_BUFFER_SIZE];

void init_uart() {
	struct usart_config config_usart;
	struct usart_config config_lora;
	usart_get_config_defaults(&config_usart);
	
	config_usart.baudrate    = DEBUG_BAUD;
	config_usart.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
	config_usart.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
	config_usart.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
	config_usart.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
	config_usart.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;
	
	usart_get_config_defaults(&config_lora);
	config_lora.baudrate = LORA_BAUD;
	config_lora.mux_setting = EXT3_UART_SERCOM_MUX_SETTING;
	config_lora.pinmux_pad0 = EXT3_UART_SERCOM_PINMUX_PAD0;
	config_lora.pinmux_pad1 = EXT3_UART_SERCOM_PINMUX_PAD1;
	config_lora.pinmux_pad2 = EXT3_UART_SERCOM_PINMUX_PAD2;
	config_lora.pinmux_pad3 = EXT3_UART_SERCOM_PINMUX_PAD3;
	
	while (usart_init(&usart_instance, EDBG_CDC_MODULE, &config_usart) != STATUS_OK) {
	}
	
	while (usart_init(&lora_uart_instance, EXT3_UART_MODULE, &config_lora) != STATUS_OK) {
	}
	
	usart_enable(&usart_instance);
	usart_enable(&lora_uart_instance);
	uint8_t msg[] = "UART Initialized\r\n";
	usart_write_buffer_wait(&usart_instance,msg,sizeof(msg)-1);
}

void send_command(uint8_t cmd[],uint16_t len)
{
	usart_write_buffer_wait(&lora_uart_instance, cmd, len);
	uint8_t msg[] = "Sent command: ";
	uint8_t term[] = "\r\n";
	usart_write_buffer_wait(&usart_instance,msg,sizeof(msg)-1);
	usart_write_buffer_wait(&usart_instance,cmd,len);
	usart_write_buffer_wait(&usart_instance,term,2);
	delay_ms(1000);
	read_response();
	delay_ms(2500);
	
}

void send_command_noack(uint8_t cmd[],uint16_t len)
{
	usart_write_buffer_wait(&lora_uart_instance, cmd, len);
	delay_ms(2500);
}

void read_response()
{
	uint16_t c;
	uint8_t msg[] = "ReadBuf overflow, emptying buffer\r\n";
	uint8_t noresp[] = "No response\r\n";
	uint8_t msg1[] = "Received from RN2903: ";
	bool response_found = false;
	
	if(usart_read_wait(&lora_uart_instance, &c) == STATUS_OK){
		
		readBuf[readBuffOffset++] = c;
		while((uint8_t)c != '\n'){
			if(usart_read_wait(&lora_uart_instance, &c) == STATUS_OK){
				if(readBuffOffset == READ_BUFFER_SIZE){
					usart_write_buffer_wait(&usart_instance,msg,sizeof(msg)-1);
				}
				readBuf[readBuffOffset++] = c;
			}
		}
		
		readBuf[readBuffOffset] = 0;
		usart_write_buffer_wait(&usart_instance,msg1,sizeof(msg1)-1);
		usart_write_buffer_wait(&usart_instance,readBuf,readBuffOffset);
		readBuffOffset = 0;
		
	}
	
	else{
		usart_write_buffer_wait(&usart_instance,noresp,sizeof(noresp)-1);
	}
			
}
