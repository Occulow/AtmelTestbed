/*
 * uart.h
 *
 * Created: 3/24/2017 12:04:32 PM
 *  Author: mazacar8
 */ 

#include <stdint.h>

#ifndef UART_H_
#define UART_H_

#define MAX_READ_ATTEMPTS 5
#define READ_BUFFER_SIZE 200
#define LORA_BAUD 57600
#define DEBUG_BAUD 9600

struct usart_module usart_instance;
struct usart_module lora_uart_instance;

void init_uart(void);
void send_command(uint8_t cmd[],uint16_t len);
void send_command_noack(uint8_t cmd[],uint16_t len);
void read_response(void);



#endif /* UART_H_ */