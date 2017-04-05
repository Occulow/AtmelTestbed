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


bool is_local_max(uint16_t COUNT_BUFF[], uint16_t frame_index, uint16_t row, uint16_t col) {
	convertToFrame(COUNT_BUFF, frame_index);
	uint16_t current_max = frame_conv[row][col];
	
	// Greater than (row+1, col), (row-1, col)
	if (current_max < frame_conv[row+1][col] || current_max < frame_conv[row-1][col]) {
		return false;
	}
	// Greater than (row, col+1), (row, col-1)
	if (current_max < frame_conv[row][col+1] || current_max < frame_conv[row][col-1]) {
		return false;
	}
	// Greater than (row+1, col+1), (row-1, col-1)
	if (current_max < frame_conv[row+1][col+1] || current_max < frame_conv[row-1][col-1]) {
		return false;
	}
	// Greater than (row+1, col-1), (row-1, col+1)
	if (current_max < frame_conv[row+1][col-1] || current_max < frame_conv[row-1][col+1]) {
		return false;
	}
	return true;
}

uint16_t* addFrame(uint16_t PIXEL_BUFFER[], uint16_t COUNT_BUFF[], uint16_t frameCount, struct keepCount *count) {
	if (frameCount < 7) {
		for (int i = 0; i < NUM_PIXELS; i++) {
			COUNT_BUFF[(frameCount-1)*NUM_PIXELS+i] = PIXEL_BUFFER[i];
		}
	}
	else {
		for (int i = 0; i < 6*NUM_PIXELS; i++){
			COUNT_BUFF[i] = COUNT_BUFF[NUM_PIXELS+i];
		}
		for (int i = 0; i < NUM_PIXELS; i++) {
			COUNT_BUFF[7*NUM_PIXELS+i] = PIXEL_BUFFER[i];
		}
	}
	count->counted = false;
	return COUNT_BUFF;
}

uint16_t getMaxId(uint16_t COUNT_BUFF[], uint16_t ind, uint16_t col) {
	convertToFrame(COUNT_BUFF, ind);
	
	uint16_t maxNum = frame_conv[0][0];
	uint16_t maxRowId = 0;
	
	for (int i=1; i < GRID_SIZE; i++){
		if (maxNum > frame_conv[i][col]) {
			maxNum = frame_conv[i][col];
			maxRowId = i;
		}
	}
	return maxRowId;
}

void convertToFrame(uint16_t COUNT_BUFF[], uint16_t ind) {
	for (int i = 0; i < GRID_SIZE; i++) {
		for (int j = 0; j < GRID_SIZE; j++) {
			frame_conv[i][j] = COUNT_BUFF[(ind*NUM_PIXELS) + GRID_SIZE*i + j];
		}
	}
	return;
}

int determine_direction(uint16_t trigger_col, uint16_t offset, uint16_t COUNT_BUFF[]) {
	if (TRIGGER_INDEX >= COUNT_BUFF_FRAMES - 1 || TRIGGER_INDEX < 1){
		//exception
	}
	if (trigger_col + offset < 0 || trigger_col + offset >= GRID_SIZE) {
		//exception
	}
	printf("In determine_direction\n");
	uint16_t check_col = trigger_col + offset;
	uint16_t max_idx = getMaxId(COUNT_BUFF, TRIGGER_INDEX, trigger_col);
	uint16_t current_max = COUNT_BUFF[TRIGGER_INDEX*NUM_PIXELS + max_idx*GRID_SIZE + trigger_col];
	
	if (current_max >= TRIGGER_THRESHOLD && is_local_max(COUNT_BUFF, TRIGGER_INDEX, max_idx, trigger_col)) {
		// Check check_col in the past
		// (Change upper bound to look further into the past)
		for (uint16_t i = 1; i < 3; i++) {
			uint16_t pastMaxIdx = getMaxId(COUNT_BUFF, TRIGGER_INDEX-i, check_col);
			uint16_t pastMax = COUNT_BUFF[(TRIGGER_INDEX-i)*NUM_PIXELS + pastMaxIdx*GRID_SIZE + check_col];
			if (abs(pastMax - current_max) <= MAX_THRESHOLD && is_local_max(COUNT_BUFF, TRIGGER_INDEX-i, pastMaxIdx, check_col)) {
				if (offset < 0) {
					printf("Exiting determine_direction 1\n");
					return IN;
				}
				else if (offset > 0) {
					printf("Exiting determine_direction 2\n");
					return OUT;
				}
			}
		}
		
		// Check check_col in the future
		// (Change upper bound to look further into the future)
		for (uint16_t i = 1; i < 3; i++) {
			uint16_t futureMaxIdx = getMaxId(COUNT_BUFF, TRIGGER_INDEX + i, check_col);
			uint16_t futureMax = COUNT_BUFF[(TRIGGER_INDEX+i)*NUM_PIXELS + futureMaxIdx*GRID_SIZE + check_col];
			if (abs(futureMax - current_max) <= MAX_THRESHOLD && is_local_max(COUNT_BUFF, TRIGGER_INDEX + i, futureMaxIdx, check_col)) {
				if (offset < 0) {
					printf("Exiting determine_direction 3\n");
					return OUT;
					} else if (offset > 0) {
					printf("Exiting determine_direction 4\n");
					return IN;
				}
			}
		}
	}
	printf("Exiting determine_direction 5\n");
	return -1; //check if I can do that
}

void doCount(struct keepCount * count, uint16_t COUNT_BUFF[], uint16_t frameCount) {
	printf("in doCount\n");
	if (frameCount<NUM_RAW_FRAMES) {
		printf("Exiting doCount 1\n");
		return;
	}
	if (count->counted) {
		printf("Exiting doCount 2\n");
		return;
	}
	for (int i = 0; i < 2; i++){
		uint16_t trigger_col = count->TRIGGER_COLUNM[i][0];
		uint16_t offset = count->TRIGGER_COLUNM[i][1];
		int direction = determine_direction(trigger_col, offset, COUNT_BUFF);
		if (direction == IN) {
			count->countIn = count->countIn + 0.5;
			} else {
			count->countOut = count->countOut + 0.5;
		}
	}
	count->counted = true;
	printf("Exiting doCount 3\n");
	return;
	
}

void countPeople(struct keepCount* count, uint16_t PIXEL_BUFFER[], uint16_t frameCount, uint16_t COUNT_BUFF[], uint16_t lastFrame) {
	printf("In countPeople\n");
	COUNT_BUFF = addFrame(PIXEL_BUFFER, COUNT_BUFF, frameCount, count);
	printf("counted = %d\n", count->counted);
	doCount(count, COUNT_BUFF, frameCount);
	printf("Exiting countPeople 2\n");
	return;
	
}
void initialize_mem(uint16_t PIXEL_BUFFER[], uint16_t frameCount) {
	if (frameCount < NUM_RAW_FRAMES) {
		for (int i = 0; i < NUM_PIXELS; i++) {
			frame[frameCount-1][i] = PIXEL_BUFFER[i];
		}
	}
	return;
}
