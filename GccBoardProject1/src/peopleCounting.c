/*
* grideye.c
*
* Created: 3/24/2017 10:51:44 PM
*  Author: Udaya Malik
*/

#include <asf.h>
#include <grideye.h>
#include <peopleCounting.h>


bool is_local_max(uint16_t ** frame, uint16_t row, uint16_t col) {
	uint16_t current_max = frame[row][col];
	// Greater than (row+1, col), (row-1, col)
	if (current_max < frame[row+1][col] || current_max < frame[row-1][col]) {
		return false;
	}
	// Greater than (row, col+1), (row, col-1)
	if (current_max < frame[row][col+1] || current_max < frame[row][col-1]) {
		return false;
	}
	// Greater than (row+1, col+1), (row-1, col-1)
	if (current_max < frame[row+1][col+1] || current_max < frame[row-1][col-1]) {
		return false;
	}
	// Greater than (row+1, col-1), (row-1, col+1)
	if (current_max < frame[row+1][col-1] || current_max < frame[row-1][col+1]) {
		return false;
	}
	return true;
}

uint16_t *** COUNT_BUFF addFrame(uint16_t ** pix_buff, uint16_t*** COUNT_BUFF, uint16_t frameCount) {
	if (frameCount < 7) {
		COUNT_BUFF[frameCount-1] = pix_buff;
	}
	else {
		for (int i = 0; i < 6; i++){
			COUNT_BUFF[i] = COUNT_BUFF[i+1];
		}
		COUNT_BUFF[7] = pix_buff;
	}
	return COUNT_BUFF;
}

uint16_t getMaxId(uint16_t ** array, uint16_t col) {
	uint16_t maxNum = array[0];
	uint16_t maxRowId = 0;
	
	for (int i=1; i < GRID_SIZE; i++){
		if (maxNum > array[i][col]) {
			maxNum = array[i][col];
			maxRowId = i;
		}
	}
	return maxRowId;
}


bool determine_direction(uint16_t trigger_col, uint16_t offset, uint16_t *** COUNT_BUFF) {
	if (TRIGGER_INDEX >= COUNT_BUFF_SIZE - 1 || TRIGGER_INDEX < 1){
		//exception
	}
	if (trigger_col + offset < 0 || trigger_col + offset >= GRID_SIZE) {
		//exception
	}
	
	uint16_t check_col = trigger_col + offset;
	
	uint16_t max_idx = getMaxId(COUNT_BUFF[TRIGGER_INDEX], trigger_col);
	uint16_t current_max = COUNT_BUFF[TRIGGER_INDEX][max_idx][trigger_col];
	
	if (current_max >= TRIGGER_THRESHOLD && is_local_max(COUNT_BUFF[TRIGGER_INDEX], max_idx, trigger_col)) {
		// Check check_col in the past
		// (Change upper bound to look further into the past)
		for (int i = 1; i < 3; i++) {
			uint16_t ** pastFrame = COUNT_BUFF[TRIGGER_INDEX - i];
			uint16_t pastMaxIdx = getMaxId(pastFrame, check_col);
			uint16_t pastMax = pastFrame[pastMaxIdx][check_col];
			if (abs(pastMax - current_max) <= MAX_THRESHOLD && is_local_max(pastFrame, pastMaxIdx, check_col)) {
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
		for (i = 1; i < 3; i++) {
			uint16_t ** futureFrame = COUNT_BUFF[TRIGGER_INDEX + i];
			uint16_t futureMaxIdx = getMaxId(futureFrame, check_col);
			uint16_t futureMax = futureFrame[futureMaxIdx][check_col];
			if (abs(futureMax - current_max) <= MAX_THRESHOLD && is_local_max(futureFrame, futureMaxIdx, check_col)) {
				if (offset < 0) {
					return OUT;
				} else if (offset > 0) {
					return IN;
				}
			}
		}
	}
	return NULL; //check if I can do that
}

void doCount(uint16_t ** pix_buff, struct keepCount * count, uint16_t*** COUNT_BUFF, uint16_t frameCount) {
	if (frameCount<COUNT_BUFF_SIZE) {
		return;	
	}
	if (count->counted) {
		return;
	}
	for (int i = 0; i < 2; i++){
		uint16_t trigger_col = count->TRIGGER_COLUNM[i][0];
		uint16_t offset = count->TRIGGER_COLUNM[i][1];
		bool direction = determine_direction(trigger_col, offset, COUNT_BUFF);
		if (direction == IN) {
			count->countIn = count->countIn + 0.5;
		} else {
			count->countOut = count->countOut + 0.5;
		}
	}
	count->counted = true;
	return;
	
	
}

struct keepCount * countPeople(struct keepCount * count, uint16_t PIXEL_BUFFER, uint16_t frameCount, uint16_t*** COUNT_BUFF, uint16_t lastFrame) {
	uint16_t pix_buff [8][8];
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			pix_buff[i][j] = PIXEL_BUFFER[8*i+j];
		}
	}
	if (count->counted) {
		return count;
	}
	COUNT_BUFF = addFrame(pix_buff, COUNT_BUFF, frameCount);
	doCount(pix_buff, count, COUNT_BUFF, frameCount);
	return count;
	
}