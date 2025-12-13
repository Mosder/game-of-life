#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Initializes a bitmap of 0's with inputted width and height
char* init_bitmap(int *width, int *height) {
	printf("Width: ");
	scanf("%d", width);
	printf("Height: ");
	scanf("%d", height);
	printf("\n");

	int width_bytes = (*width+7) / 8;
	int size = *height * width_bytes;
	return calloc(size, sizeof(char));
}

// Sets a whole byte in a bitmap
void set_byte(char *bitmap, int width_bytes, int row, int byte, char val) {
	bitmap[row*width_bytes + byte] = val;
}

// Returns a bit on a given position in a bitmap
char get_bit(char *bitmap, int width_bytes, int row, int col) {
	int byte = col / 8;
	int bit = 7 - col%8;
	return (bitmap[row*width_bytes + byte] >> bit) & 1;
}

// Prints a bitmap
void print_bitmap(char *bitmap, int width, int height) {
	int width_bytes = (width+7) / 8;
	for (int r = 0; r < height; r++) {
		for (int c = 0; c < width; c++)
			printf("%d", get_bit(bitmap, width_bytes, r, c));
		printf("\n");
	}
	printf("\n");
}

// Shifts 2 bytes and returns OR of them
// Used for determining bytes representing neighboring bits
char shift_or_bytes(char byte1, int shift1, char byte2, int shift2) {
	return (byte1 << shift1) | (byte2 >> shift2);
}

// Counts bits set to 1 across words on same positions
// Used for determining count of neighbors
void count_bits_on_positions(int *counts, char *bytes) {
	for (int pos = 0; pos < 8; pos++) {
		int count = 0;
		// count from right to left
		for (int i = 0; i < 8; i++) {
			if (bytes[i] & 1) count++;
			bytes[i] >>= 1;
		}
		// save from right to left, since it's counted from right to left
		counts[7-pos] = count;
	}
}

// Counts the neighbors of each bit in the whole row
int* count_neighbors_row(int width, char *row, char *row_above, char *row_below) {
	int width_bytes = (width+7) / 8;
	int *counts = calloc(8*width_bytes, sizeof(int));

	// array which contains neighbor data for the whole byte, one entry for one neighbor data, 8 neighbors total
	char shifted_bytes[8];
	// prev bytes to avoid handling first byte as a seperate case
	char prev_byte_above = 0;
	char prev_byte = 0;
	char prev_byte_below = 0;

	for (int byte = 0; byte < width_bytes; byte++) {
		// top-left neighbor
		shifted_bytes[0] = shift_or_bytes(prev_byte_above, 7, *(row_above+byte), 1);
		// center-left neighbor
		shifted_bytes[1] = shift_or_bytes(prev_byte, 7, *(row+byte), 1);
		// bottom-left neighbor
		shifted_bytes[2] = shift_or_bytes(prev_byte_below, 7, *(row_below+byte), 1);

		// top-center neighbor
		shifted_bytes[3] = *(row_above+byte);
		// bottom-center neighbor
		shifted_bytes[4] = *(row_below+byte);

		// determine if last byte - if so, don't consider next byte and clip the extra bits
		if (byte != width_bytes-1) {
			// top-right neighbor
			shifted_bytes[5] = shift_or_bytes(*(row_above+byte), 1, *(row_above+byte+1), 7);
			// center-right neighbor
			shifted_bytes[6] = shift_or_bytes(*(row+byte), 1, *(row+byte+1), 7);
			// bottom-right neighbor
			shifted_bytes[7] = shift_or_bytes(*(row_below+byte), 1, *(row_below+byte+1), 7);
		}
		else {
			// top-right neighbor
			shifted_bytes[5] = *(row_above+width_bytes-1) << 1;
			// center-right neighbor
			shifted_bytes[6] = *(row+width_bytes-1) << 1;
			// bottom-right neighbor
			shifted_bytes[7] = *(row_below+width_bytes-1) << 1;

			// clip extra bits
			int extra_bits = width%8 ? 8 - width%8 : 0;
			char byte_mask = 0xFF;
			byte_mask <<= extra_bits;
			for (int i = 0; i < 8; i++)
				shifted_bytes[i] &= byte_mask;
		}

		// count neighbors for this byte
		count_bits_on_positions(counts + 8*byte, shifted_bytes);

		// update prev bytes
		prev_byte_above = *(row_above+byte);
		prev_byte = *(row+byte);
		prev_byte_below = *(row_below+byte);
	}

	return counts;
}

// Determines whether cells in a row live or die depending on their neighbor count
char* decide_fate_row(char *row, int width, int *counts) {
	int width_bytes = (width+7) / 8;
	char *new_row = calloc(width_bytes, sizeof(char));
	for (int byte = 0; byte < width_bytes; byte++) {
		// masks for 2 and 3 neighbors, since otherwise cell always dies/stays dead
		char mask_2_neighbors = 0;
		char mask_3_neighbors = 0;
		for (int bit = 7; bit > -1; bit--) {
			// access the bits from the most significant bit in a byte
			if (counts[8*byte + 7-bit] == 2)
				mask_2_neighbors |= 1 << bit;
			else if (counts[8*byte + 7-bit] == 3)
	    			mask_3_neighbors |= 1 << bit;
		}
		// if 3 neighbors - always alive next frame
		// if 2 neighbors - stays alive
		new_row[byte] = mask_3_neighbors | (mask_2_neighbors & row[byte]);
	}
	free(counts);
	return new_row;
}

// Calculate state of the game in the next frame
char* convolve_conway(char *bitmap, int width, int height) {
	int width_bytes = (width+7) / 8;
	char *new_bitmap = calloc(height*width_bytes, sizeof(char));
	char *zeros = calloc(width_bytes, sizeof(char));
	
	// prev row of zeros to avoid handling first row seperately
	char *prev_row = zeros;
	for (int r = 0; r < height; r++) {
		int *counts;
		// Determine if it's the last row, to set row below to zeros if so
		if (r != height-1)
			counts = count_neighbors_row(width, bitmap + r*width_bytes, prev_row, bitmap + (r+1)*width_bytes);
		else
			counts = count_neighbors_row(width, bitmap + r*width_bytes, prev_row, zeros);

		// decide fate of the current row and set the bytes in the new bitmap
		char *new_fate = decide_fate_row(bitmap + r*width_bytes, width, counts);
		for (int byte = 0; byte < width_bytes; byte++)
			set_byte(new_bitmap, width_bytes, r, byte, new_fate[byte]);

		// update prev row
		prev_row = bitmap + r*width_bytes;
	}
	free(bitmap);
	free(zeros);
	return new_bitmap;
}

int main() {
	int width, height;
	char *bitmap = init_bitmap(&width, &height);

	set_byte(bitmap, (width+7)/8, 1, 0, 0b00111000);
	set_byte(bitmap, (width+7)/8, 3, 0, 0b00110000);
	set_byte(bitmap, (width+7)/8, 4, 0, 0b00110000);
	print_bitmap(bitmap, width, height);

	for (int i = 0; i < 5; i++) {
		bitmap = convolve_conway(bitmap, width, height);
		print_bitmap(bitmap, width, height);
	}
}

