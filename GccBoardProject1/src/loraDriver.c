/*
 * loraDriver.c
 *
 * Created: 3/24/2017 12:08:18 PM
 *  Author: mazacar8
 */

#include "loraDriver.h"
#include <stdio.h>
#include <asf.h>
#include "saml21j18b.h"
#include "status_codes.h"

uint8_t reset_N = PIN_PB00;
uint16_t rx_offset = 0;

/**
 * @brief      Initializes UART for the RN2903 and debug interface
 *
 */
void init_uart() {
	struct usart_config config_usart;
	struct usart_config config_lora;
	
	//Debug Interface configuration
	usart_get_config_defaults(&config_usart);
	config_usart.baudrate    = DEBUG_BAUD;
	config_usart.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
	config_usart.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
	config_usart.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
	config_usart.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
	config_usart.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;
	
	//Lora UART interface configuration
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
	printSerial(msg,sizeof(msg));
}

/**
 * @brief      Initializes and saves setup values for RN2903
 *
 */
void lora_setup()
{
	uint16_t n = (uint16_t) sprintf(tx_buffer,"Setting up LORA...\r\n");
	printSerial(tx_buffer,n);
	lora_reset();
	n = (uint16_t) sprintf(tx_buffer,"sys factoryRESET\r\n");
	send_lora_command(tx_buffer,n);
	delay_ms(5000);
	read_response();
	n = (uint16_t) sprintf(tx_buffer,"mac set appeui 0000000000000100\r\n");
	send_lora_command(tx_buffer,n);
	n = (uint16_t) sprintf(tx_buffer,"mac set deveui 1122334455667799\r\n");
	send_lora_command(tx_buffer,n);
	n = (uint16_t) sprintf(tx_buffer,"mac set appkey 2b7e151628aed2a6abf7156669cf4f3c\r\n");
	send_lora_command(tx_buffer,n);
	n = (uint16_t) sprintf(tx_buffer,"mac set dr 0\r\n");
	send_lora_command(tx_buffer,n);
	n = (uint16_t) sprintf(tx_buffer,"mac set pwridx 5\r\n");
	send_lora_command(tx_buffer,n);
	n = (uint16_t) sprintf(tx_buffer,"mac set adr off\r\n");
	send_lora_command(tx_buffer,n);

	setup_channels();
	
	n = (uint16_t) sprintf(tx_buffer,"mac set devaddr 00000000\r\n");
	send_lora_command(tx_buffer,n);
	n = (uint16_t) sprintf(tx_buffer,"radio set pwr 20\r\n");
	send_lora_command(tx_buffer,n);
	n = (uint16_t) sprintf(tx_buffer,"radio set prlen 8\r\n");
	send_lora_command(tx_buffer,n);
	n = (uint16_t) sprintf(tx_buffer,"mac save\r\n");
	send_lora_command(tx_buffer,n);
	lora_join();
}

/**
 * @brief      Resets the RN2903
 *
 */
void lora_reset(){
	
	port_get_config_defaults(&reset_config);
	reset_config.direction = PORT_PIN_DIR_OUTPUT;
	reset_config.input_pull = PORT_PIN_PULL_NONE;
	reset_config.powersave = false;
	port_pin_set_config(reset_N,&reset_config);
	port_pin_set_output_level(reset_N,LOW);
	port_pin_set_output_level(reset_N,HIGH);
	delay_ms(2000);
	
}

/**
 * @brief      Switches off all but channels 0-7 for Lora transmit
 *
 */
void setup_channels(){
	uint16_t n;
	
	for (int i = 0; i < 72; i++) {
		if (i < 8){
			n = (uint16_t) sprintf(tx_buffer,"mac set ch status %d on\r\n",i);
			send_lora_command(tx_buffer,n);
		}
		
		else{
			n = (uint16_t) sprintf(tx_buffer,"mac set ch status %d off\r\n",i);
			send_lora_command(tx_buffer,n);
		}
		
	}
	
}

/**
 * @brief      Attempts to join the Lora network a maximum of 5 times
 *
 */
void lora_join() {
	int i,j;
	uint16_t n;
	bool accepted = false;
	
	//Loop MAX_JOIN_ATTEMPTS to try and join the network
	for(i = 0; i < MAX_JOIN_ATTEMPTS; i++){
		
		n = (uint16_t) sprintf(tx_buffer,"mac join otaa\r\n");
		send_lora_command(tx_buffer,n);
		if(rx_buffer[0] != 'o'){
			//Command not received by RN2903. Needs to be sent again.
			n = (uint16_t) sprintf(tx_buffer,"Command Failed. Retrying in 10 s\r\n");
			printSerial(tx_buffer,n);
			delay_ms(10000);
			continue;
		}
		
		//Loop until response from gateway is received
		for(j = 0; j < MAX_STATUS_CHECKS; j++){
			
			if(read_response()){
				//Gateway responded
				if(rx_buffer[0] == 'a'){	//Checking if 'accepted'. Temporarily doing it this way until read_response is fixed
					accepted = true;
					n = (uint16_t) sprintf(tx_buffer,"Join Accepted!\r\n");
					printSerial(tx_buffer,n);
					break;
				}
				else if(j == MAX_STATUS_CHECKS - 1){
					n = (uint16_t) sprintf(tx_buffer,"Join Failed... Aborting\r\n");
					printSerial(tx_buffer,n);
					accepted = false;
				}
				else{
					//Join request sent and denied, so new command must be sent
					n = (uint16_t) sprintf(tx_buffer,"Join Failed... Retrying in 10 seconds\r\n");
					printSerial(tx_buffer,n);
					accepted = false;
					delay_ms(10000);
					break;
				}
				
			}
			
			else{
				//Wait 5 seconds before checking for the response again
				n = sprintf(tx_buffer,"Waiting 5 seconds\r\n");
				printSerial(tx_buffer,n);
				delay_ms(5000);
			}
			
		}
		
		if(accepted){
			break;
		}
	}
	
}

/**
 * @brief      Sends a command to the RN2903
 *
 * @param	   cmd	Command to send 
 * @param	   len	Length of command to send
 */
void send_lora_command(uint8_t cmd[],uint16_t len)
{
	while(usart_write_buffer_wait(&lora_uart_instance, cmd, len) != STATUS_OK);
	uint8_t msg[] = "Sent command: ";
	printSerial(msg,sizeof(msg));
	printSerial(cmd,len);
	//delay_ms(500);
	read_response();
	
}

/**
 * @brief      Prints to the debug interface
 *
 * @param	   cmd	Command to print
 * @param	   len	Length of command to print
 */
void printSerial(uint8_t cmd[],uint16_t len)
{
	while(usart_write_buffer_wait(&usart_instance, cmd, len) != STATUS_OK);
	
}

/**
 * @brief      Reads the response from RN2903
 *
 * @return     true if there was a response, false if there wasn't
 */
bool read_response()
{	
		
	uint16_t c,n;
	uint8_t msg[] = "ReadBuf overflow, emptying buffer\r\n";
	uint8_t noresp[] = "No response\r\n";
	uint8_t msg1[] = "Received from RN2903: ";
	
	status_code_genare_t err;
	uint8_t check[10];
	
	if(usart_read_wait(&lora_uart_instance, &c) == STATUS_OK){
		rx_buffer[rx_offset++] = c;
		while((char)c != '\n'){
			while((err = usart_read_wait(&lora_uart_instance, &c))!=STATUS_OK){
				n = sprintf(check,"ERROR: %x\r\n",err);
				//printSerial(check,n);
			};
			if(rx_offset == RX_BUFFER_SIZE){
				n = (uint16_t) sprintf(tx_buffer,"ReadBuf overflow, emptying buffer\r\n");
				printSerial(tx_buffer,n);
			}
			rx_buffer[rx_offset++] = c;
			
		}
		
		rx_buffer[rx_offset] = 0;
		printSerial(msg1,sizeof(msg1)-1);
		printSerial(rx_buffer,rx_offset);
		rx_offset = 0;
		return true;
		
	}
	
	else{
		printSerial(noresp,sizeof(noresp));
		return false;
	}
	
}

/**
 * @brief      Sends "string" over the Lora Network
 *
 * @param	   string	String to send over the network
 */
void send_lora_data(uint8_t string[]){
	
	uint16_t n = sprintf(tx_buffer,"mac tx uncnf 1 %x\r\n",string);
	send_lora_command(tx_buffer,n);
}

/**
 * @brief      Sends an integer over the Lora Network
 *
 * @param	   count	Integer to send over the network
 */
void send_lora_count(uint16_t count){
	
	uint16_t n = sprintf(tx_buffer,"mac tx uncnf 1 %x\r\n",count);
	send_lora_command(tx_buffer,n);
}

