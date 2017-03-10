/*
 * grideye.h
 *
 * Created: 3/1/2017 8:54:53 PM
 *  Author: Terence Sun
 */ 


#ifndef GRIDEYE_H_
#define GRIDEYE_H_

#define NUM_PIXELS 64

void init_grideye(void);
bool is_sleeping(void);
bool set_mode_sleep(void);
double get_ambient_temp(void);
void get_frame(uint16_t *frame_buffer);

#endif /* GRIDEYE_H_ */