/*
* grideye.c
*
* Created: 3/24/2017 10:51:44 PM
*  Author: Udaya Malik
*/

#include <asf.h>
#include <grideye.h>
#include <peopleCounting.h>
#include <numpyInC.h>

static uint16_t FRAME_CONV[GRID_SIZE][GRID_SIZE];
static uint16_t FRAME[NUM_RAW_FRAMES][NUM_PIXELS];
static struct keepCount count;

void initialize_count() {
	count.countIn = 0;
	count.countOut = 0;
	count.TRIGGER_COLUNM[0][0] = TRIGGER_COLUNM_2;
	count.TRIGGER_COLUNM[0][1] = CHECK_OFFSET_2;
	count.TRIGGER_COLUNM[1][0] = TRIGGER_COLUNM_5;
	count.TRIGGER_COLUNM[1][1] = CHECK_OFFSET_5;
	count.counted = false;
}

void median_buffer_init(uint16_t *median_filters_buff[]) {
	for (int i = 0; i < NUM_RAW_FRAMES; i++) {
		median_filters_buff[i] = &FRAME[i];
	}
	return;
}

void pc_new_frame(uint16_t frame count, uint16_t pix_buff[], uint16_t *median_filters_buff[]) {
	if (frame_count < NUM_RAW_FRAMES) {
		initialize_mem(pix_buff, frame_count);
	} else {
		dstack(median_filters_buff, pix_buff);
	}
	return;
}

void start_count(uint16_t frame count, uint16_t pix_buff[], uint16_t *median_filter_buff[], uint16_t count_buffer[]) {
	pc_new_frame(frame_count, pix_buff, median_filter_buff);
	uint16_t *frame_median = median_frame(median_filter_buff);
	countPeople(frame_median, frame_count, count_buffer);
	return;
}

bool is_local_max(uint16_t count_buffer[], uint16_t frame_index, uint16_t row, uint16_t col) {
	convert_to_frame(count_buffer, frame_index);
	uint16_t current_max = FRAME_CONV[row][col];
	
	// Greater than (row+1, col), (row-1, col)
	if ((row < 7 && current_max < FRAME_CONV[row+1][col] )|| (row > 0 && current_max < FRAME_CONV[row-1][col])) {
		return false;
	}
	// Greater than (row, col+1), (row, col-1)
	if ((col < 7 && current_max < FRAME_CONV[row][col+1]) || (col > 0 && current_max < FRAME_CONV[row][col-1])) {
		return false;
	}
	// Greater than (row+1, col+1), (row-1, col-1)
	if ((row < 7 && col < 7 && current_max < FRAME_CONV[row+1][col+1]) || (row > 0 && col > 0 && current_max < FRAME_CONV[row-1][col-1])) {
		return false;
	}
	// Greater than (row+1, col-1), (row-1, col+1)
	if ((row < 7 && col > 0 && current_max < FRAME_CONV[row+1][col-1]) || (row > 0 && col < 7 && current_max < FRAME_CONV[row-1][col+1])) {
		return false;
	}
	return true;
}

void add_frame(uint16_t frame_median[], uint16_t count_buffer[], uint16_t frame_count) {
	if (frame_count < 6) {
		for (int i = 0; i < NUM_PIXELS; i++) {
			count_buffer[(frame_count)*NUM_PIXELS+i] = frame_median[i];
		}
	} else {
		for (int i = 0; i < 6*NUM_PIXELS; i++){
			count_buffer[i] = count_buffer[NUM_PIXELS+i];
		}
		for (int i = 0; i < NUM_PIXELS; i++) {
			count_buffer[6*NUM_PIXELS+i] = frame_median[i];
		}
	}
	count.counted = false;
	return;
}

uint16_t getMaxId(uint16_t count_buffer[], uint16_t ind, uint16_t col) {
	convert_to_frame(count_buffer, ind);
	uint16_t maxNum = FRAME_CONV[0][0];
	uint16_t maxRowId = 0;
	
	for (int i=1; i < GRID_SIZE; i++){
		if (maxNum > FRAME_CONV[i][col]) {
			maxNum = FRAME_CONV[i][col];
			maxRowId = i;
		}
	}
	return maxRowId;
}

void convert_to_frame(uint16_t count_buffer[], uint16_t ind) {
	for (int i = 0; i < GRID_SIZE; i++) {
		for (int j = 0; j < GRID_SIZE; j++) {
			FRAME_CONV[i][j] = count_buffer[(ind*NUM_PIXELS) + GRID_SIZE*i + j];
		}
	}
	return;
}

int determine_direction(uint16_t trigger_col, uint16_t offset, uint16_t count_buffer[]) {
	if (TRIGGER_INDEX >= COUNT_BUFF_FRAMES - 1 || TRIGGER_INDEX < 1){
		//exception
	}
	if (trigger_col + offset < 0 || trigger_col + offset >= GRID_SIZE) {
		//exception
	}
	uint16_t check_col = trigger_col + offset;
	uint16_t max_idx = getMaxId(count_buffer, TRIGGER_INDEX, trigger_col);
	uint16_t current_max = COUNT_BUFF[TRIGGER_INDEX*NUM_PIXELS + max_idx*GRID_SIZE + trigger_col];
	
	if (current_max >= TRIGGER_THRESHOLD && is_local_max(count_buffer, TRIGGER_INDEX, max_idx, trigger_col)) {
		// Check check_col in the past
		// (Change upper bound to look further into the past)
		for (uint16_t i = 1; i < 3; i++) {
			uint16_t pastMaxIdx = getMaxId(count_buffer, TRIGGER_INDEX-i, check_col);
			uint16_t pastMax = count_buffer[(TRIGGER_INDEX-i)*NUM_PIXELS + pastMaxIdx*GRID_SIZE + check_col];
			if (abs(pastMax - current_max) <= MAX_THRESHOLD && is_local_max(count_buffer, TRIGGER_INDEX-i, pastMaxIdx, check_col)) {
				if (offset < 0) {
					return IN;
				}
				else if (offset > 0) {
					return OUT;
				}
			}
		}
		
		// Check check_col in the future
		// (Change upper bound to look further into the future)
		for (uint16_t i = 1; i < 3; i++) {
			uint16_t futureMaxIdx = getMaxId(count_buffer, TRIGGER_INDEX + i, check_col);
			uint16_t futureMax = count_buffer[(TRIGGER_INDEX+i)*NUM_PIXELS + futureMaxIdx*GRID_SIZE + check_col];
			if (abs(futureMax - current_max) <= MAX_THRESHOLD && is_local_max(count_buffer, TRIGGER_INDEX + i, futureMaxIdx, check_col)) {
				if (offset < 0) {
					return OUT;
					} else if (offset > 0) {
					return IN;
				}
			}
		}
	}
	return -1; //check if I can do that
}

void doCount(uint16_t count_buffer[], uint16_t frame_count) {
	if (frame_count<NUM_RAW_FRAMES) {
		return;
	}
	if (count.counted) {
		return;
	}
	for (int i = 0; i < 2; i++){
		uint16_t trigger_col = count.TRIGGER_COLUNM[i][0];
		uint16_t offset = count.TRIGGER_COLUNM[i][1];
		int direction = determine_direction(trigger_col, offset, count_buffer);
		if (direction == IN) {
			count.countIn = count.countIn + 0.5;
			} else {
			count.countOut = count.countOut + 0.5;
		}
	}
	count.counted = true;
	return;
	
}

void countPeople(uint16_t frame_median[], uint16_t frame_count, uint16_t count_buffer[]) {
	add_frame(frame_median, count_buffer, frame_count);
	doCount(count_buffer, frame_count);
	return;
	
}
void initialize_mem(uint16_t pix_buff[], uint16_t frame_count) {
	if (frame_count < NUM_RAW_FRAMES) {
		for (int i = 0; i < NUM_PIXELS; i++) {
			FRAME[i] = pix_buff[i];
		}
	}
	return;
}
