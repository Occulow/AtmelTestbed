/*
* grideye.c
*
* Created: 3/1/2017 8:28:44 PM
*  Author: Terence Sun
*/

#include <asf.h>
#include <grideye.h>

#define DATA_LENGTH 1
static uint8_t write_buffer[DATA_LENGTH] = {
	0x0E
};

static uint8_t read_buffer[DATA_LENGTH];

#define SLAVE_ADDRESS 0x68
#define REG_THERM_LSB 0x0E
#define REG_THERM_MSB 0x0F
#define REG_PIXEL_BASE 0x80

/* Number of times to try to send packet if failed. */
#define TIMEOUT 1000

/* Init software module. */
struct i2c_master_module i2c_master_instance;

uint8_t read_byte(uint8_t addr);

void init_grideye(void)
{
	/* Initialize config structure and software module. */
	struct i2c_master_config config_i2c_master;
	i2c_master_get_config_defaults(&config_i2c_master);

	/* Change buffer timeout to something longer. */
	config_i2c_master.buffer_timeout = 10000;
	
	/* Initialize and enable device with config. */
	i2c_master_init(&i2c_master_instance, CONF_I2C_MASTER_MODULE, &config_i2c_master);

	i2c_master_enable(&i2c_master_instance);
}
bool ge_is_sleeping(void) {
	return false;
}
bool ge_set_mode_sleep(void) {
	return false;
}

/**
 * Gets ambient temperature in Celsius
 */
double ge_get_ambient_temp(void)
{
	uint8_t lsb, msb;

	lsb = read_byte(REG_THERM_LSB);
	msb = read_byte(REG_THERM_MSB);
	return (((msb << 8) + lsb) * 0.0625);
}

/**
* Gets a frame from the GridEye
*/
void ge_get_frame(uint16_t *frame_buffer)
{
	uint8_t lsb, msb;
	for (int i = 0; i < NUM_PIXELS; i++) {
		lsb = read_byte(REG_PIXEL_BASE + 2*i);
		msb = read_byte(REG_PIXEL_BASE + 2*i + 1);
		frame_buffer[i] = ((msb << 8) + lsb);
		// frame_buffer[i] = (((msb << 8) + lsb) * 0.25);
	}
}

uint8_t read_byte(uint8_t addr) {
	uint16_t timeout = 0;
	struct i2c_master_packet packet = {
		.address = SLAVE_ADDRESS,
		.data_length = 1,
		.ten_bit_address = false,
		.high_speed = false,
		.hs_master_code = 0x0,
	};
	
	/* Write addr to read */
	write_buffer[0] = addr;
	packet.data = write_buffer;
	while (i2c_master_write_packet_wait(&i2c_master_instance, &packet) != STATUS_OK) {
		/* Increment timeout counter and check if timed out. */
		if (timeout++ == TIMEOUT) {
			break;
		}
	}
	
	/* Read value */
	packet.data = read_buffer;
	while (i2c_master_read_packet_wait(&i2c_master_instance, &packet) !=
	STATUS_OK) {
		/* Increment timeout counter and check if timed out. */
		if (timeout++ == TIMEOUT) {
			break;
		}
	}
	
	return read_buffer[0];
}

