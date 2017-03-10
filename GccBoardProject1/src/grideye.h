/*
 * grideye.h
 *
 * Created: 3/1/2017 8:54:53 PM
 *  Author: Terence Sun
 */ 

#ifndef GRIDEYE_H_
#define GRIDEYE_H_

#define NUM_PIXELS 64
#define GE_MODE_NORMAL 0x00
#define GE_MODE_SLEEP 0x10
#define GE_MODE_STANDBY_1 0x20
#define GE_MODE_STANDBY_2 0x21

void init_grideye(void);
bool ge_is_sleeping(void);
bool ge_set_mode(uint8_t mode);
double ge_get_ambient_temp(void);
void ge_get_frame(uint16_t *frame_buffer);

#endif /* GRIDEYE_H_ */