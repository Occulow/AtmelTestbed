/*
 * loraDriver.h
 *
 * Created: 3/24/2017 12:08:35 PM
 *  Author: mazacar8
 */ 

#include <stdint.h>
#include <stdbool.h>

#ifndef LORADRIVER_H_
#define LORADRIVER_H_

#define MAX_JOIN_ATTEMPTS 5
#define MAX_STATUS_CHECKS 5
#define LORA_BAUD 57600
#define DEBUG_BAUD 115200
#define RX_BUFFER_SIZE 100
#define TX_BUFFER_SIZE 100

struct port_config reset_config;
struct usart_module usart_instance;
struct usart_module lora_uart_instance;

volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
volatile uint8_t tx_buffer[TX_BUFFER_SIZE];

void lora_setup(void);
void lora_join(void);
void lora_reset(void);
void send_lora_data(uint8_t string[]);
void setup_channels();
void init_uart(void);
void send_lora_command(uint8_t cmd[],uint16_t len);
void printSerial(uint8_t cmd[],uint16_t len);
bool read_response(void);
void send_lora_count(uint16_t count);

#endif /* LORADRIVER_H_ */