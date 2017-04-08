
/*
 * peopleCounting.h
 *
 * Created: 3/24/2017 8:59 PM
 * Author: Udaya Malik
 */ 
#ifndef PEOPLECOUNTING_H_
#define PEOPLECOUNTING_H_

#define COUNT_BUFF_SIZE 448
#define COUNT_BUFF_FRAMES 7
#define TRIGGER_COLUNM_2 2
#define TRIGGER_COLUNM_5 5
#define CHECK_OFFSET_2 1
#define CHECK_OFFSET_5 -1
#define IN false
#define OUT true
#define TRIGGER_INDEX COUNT_BUFF_SIZE/2
#define GRID_SIZE 8
#define TRIGGER_THRESHOLD 20
#define MAX_THRESHOLD 10
#define MEDIAN_FILTER_POINTERS_BUFF_SIZE 10
#define NUM_RAW_FRAMES 10


struct keepCount{
	uint16_t countIn;
	uint16_t countOut;
	uint16_t TRIGGER_COLUNM[2][2];
	bool counted;
};

void median_buffer_init(uint16_t *median_filters_buff[]);
void initialize_count();
void pc_new_frame(uint16_t frame_count, uint16_t pix_buff[], uint16_t *median_filters_buff[]);
void start_count(uint16_t frame_count, uint16_t pix_buff[], uint16_t *median_filter_buff[], uint16_t count_buffer[]);
bool is_local_max(uint16_t count_buffer[], uint16_t frame_index, uint16_t row, uint16_t col);
void add_frame(uint16_t frame_median[], uint16_t count_buffer[], uint16_t frame_count)
uint16_t getMaxId(uint16_t count_buffer[], uint16_t ind, uint16_t col);
void convert_to_frame(uint16_t count_buffer[], uint16_t ind);
int determine_direction(uint16_t trigger_col, uint16_t offset, uint16_t count_buffer[]);
void doCount(uint16_t count_buffer[], uint16_t frame_count);
void countPeople(uint16_t frame_median[], uint16_t frame_count, uint16_t count_buffer[]);
void initialize_mem(uint16_t pix_buff[], uint16_t frame_count);
#endif 
