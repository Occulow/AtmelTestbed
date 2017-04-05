
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

bool is_local_max(uint16_t COUNT_BUFF[], uint16_t frame_index, uint16_t row, uint16_t col);
uint16_t* addFrame(uint16_t PIXEL_BUFFER[], uint16_t COUNT_BUFF[], uint16_t frameCount, struct keepCount *count);
uint16_t getMaxId(uint16_t COUNT_BUFF[], uint16_t ind, uint16_t col);
void convertToFrame(uint16_t COUNT_BUFF[], uint16_t ind);
int determine_direction(uint16_t trigger_col, uint16_t offset, uint16_t* COUNT_BUFF);
void doCount(struct keepCount *count, uint16_t COUNT_BUFF[], uint16_t frameCount);
void countPeople(struct keepCount* count, uint16_t PIXEL_BUFFER[], uint16_t frameCount, uint16_t COUNT_BUFF[], uint16_t lastFrame);
void initialize_mem(uint16_t PIXEL_BUFFER[], uint16_t frameCount);
#endif 
