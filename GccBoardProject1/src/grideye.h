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
bool ge_is_sleeping(void);
bool ge_set_mode_sleep(void);
double ge_get_ambient_temp(void);
void ge_get_frame(uint16_t *frame_buffer);

#endif /* GRIDEYE_H_ */