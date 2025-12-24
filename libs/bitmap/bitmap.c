#include "../common.h"
#include "bitmap.h"

// Initializes and returns a bitmap of 0's with inputted height and width
Bitmap init_bitmap() {
	uint16 height = 0;
	uint16 width = 0;

	while (height == 0) {
		printf("Height (1-65535): ");
		scanf("%hu", &height);
	}
	while (width == 0) {
		printf("Width (1-65535): ");
		scanf("%hu", &width);
	}
	printf("\n");

	uint16 width_bytes = (width+7) / 8;
	uint32 size = height * width_bytes;
	byte *map = calloc(size, sizeof(byte));

	Bitmap bitmap;
	bitmap.map = map;
	bitmap.height = height;
	bitmap.width = width;
	bitmap.width_bytes = width_bytes;

	return bitmap;
}

// Sets a whole byte in a bitmap
void set_byte(Bitmap *bitmap, uint16 row, uint16 col_byte, byte val) {
	bitmap->map[row * bitmap->width_bytes + col_byte] = val;
}

// Returns a bit on a given position in a bitmap
byte get_bit(Bitmap bitmap, uint16 row, uint16 col) {
	uint16 col_byte = col / 8;
	uint8 bit = 7 - col%8;
	return (bitmap.map[row * bitmap.width_bytes + col_byte] >> bit) & 1;
}

// Invert a single bit in bitmap
void invert_bit(Bitmap *bitmap, uint16 row, uint16 col) {
	uint16 col_byte = col / 8;
	uint8 bit = 7 - col%8;
	bitmap->map[row * bitmap->width_bytes + col_byte] ^= 1 << bit;
}

// Shifts 2 bytes and returns OR of them
// Used for determining bytes representing neighboring bits
byte shift_or_bytes(byte byte1, uint8 shift1, byte byte2, uint8 shift2) {
	return (byte1 << shift1) | (byte2 >> shift2);
}

// Counts bits set to 1 across words on same positions
// Used for determining count of neighbors
void count_bits_on_positions(uint8 *counts, byte *bytes) {
	for (uint8 pos = 0; pos < 8; pos++) {
		uint8 count = 0;
		// count from right to left
		for (uint8 i = 0; i < 8; i++) {
			if (bytes[i] & 1) count++;
			bytes[i] >>= 1;
		}
		// save from right to left, since it's counted from right to left
		counts[7-pos] = count;
	}
}

// Counts the neighbors of each bit in the whole row
uint8* count_neighbors_row(uint16 width, uint16 width_bytes, byte *row, byte *row_above, byte *row_below) {
	uint8 *counts = calloc(8*width_bytes, sizeof(uint8));

	// array which contains neighbor data for the whole byte, one entry for one neighbor data, 8 neighbors total
	byte shifted_bytes[8];
	// prev bytes to avoid handling first byte as a seperate case
	byte prev_byte_above = 0;
	byte prev_byte = 0;
	byte prev_byte_below = 0;

	for (uint16 col_byte = 0; col_byte < width_bytes; col_byte++) {
		// current bytes
		byte curr_byte_above = *(row_above + col_byte);
		byte curr_byte = *(row + col_byte);
		byte curr_byte_below = *(row_below + col_byte);

		// top-left neighbor
		shifted_bytes[0] = shift_or_bytes(prev_byte_above, 7, curr_byte_above, 1);
		// center-left neighbor
		shifted_bytes[1] = shift_or_bytes(prev_byte, 7, curr_byte, 1);
		// bottom-left neighbor
		shifted_bytes[2] = shift_or_bytes(prev_byte_below, 7, curr_byte_below, 1);

		// top-center neighbor
		shifted_bytes[3] = curr_byte_above;
		// bottom-center neighbor
		shifted_bytes[4] = curr_byte_below;

		// determine if last byte - if so, don't consider next byte and clip the extra bits
		if (col_byte != width_bytes-1) {
			// top-right neighbor
			shifted_bytes[5] = shift_or_bytes(curr_byte_above, 1, *(row_above + col_byte+1), 7);
			// center-right neighbor
			shifted_bytes[6] = shift_or_bytes(curr_byte, 1, *(row + col_byte+1), 7);
			// bottom-right neighbor
			shifted_bytes[7] = shift_or_bytes(curr_byte_below, 1, *(row_below + col_byte+1), 7);
		}
		else {
			// top-right neighbor
			shifted_bytes[5] = curr_byte_above << 1;
			// center-right neighbor
			shifted_bytes[6] = curr_byte << 1;
			// bottom-right neighbor
			shifted_bytes[7] = curr_byte_below << 1;

			// clip extra bits
			uint8 extra_bits = width%8 ? 8 - width%8 : 0;
			byte byte_mask = 0xFF << extra_bits;
			for (uint8 i = 0; i < 8; i++)
				shifted_bytes[i] &= byte_mask;
		}

		// count neighbors for this byte
		count_bits_on_positions(counts + 8*col_byte, shifted_bytes);

		// update prev bytes
		prev_byte_above = curr_byte_above;
		prev_byte = curr_byte;
		prev_byte_below = curr_byte_below;
	}

	return counts;
}

// Determines whether cells in a row live or die depending on their neighbor count
byte* decide_fate_row(byte *row, uint16 width, uint16 width_bytes, uint8 *counts) {
	byte *new_row = calloc(width_bytes, sizeof(byte));
	for (uint16 col_byte = 0; col_byte < width_bytes; col_byte++) {
		// masks for 2 and 3 neighbors, since otherwise cell always dies/stays dead
		byte mask_2_neighbors = 0;
		byte mask_3_neighbors = 0;
		for (uint8 bit = 0; bit < 8; bit++) {
			// access the bits from the least significant bit in a byte
			if (counts[8*col_byte + 7-bit] == 2)
				mask_2_neighbors |= 1 << bit;
			else if (counts[8*col_byte + 7-bit] == 3)
	    			mask_3_neighbors |= 1 << bit;
		}
		// if 3 neighbors - always alive next frame
		// if 2 neighbors - stays alive
		new_row[col_byte] = mask_3_neighbors | (mask_2_neighbors & row[col_byte]);
	}
	free(counts);
	return new_row;
}

// Change state of the game to the one in the next frame
void convolve_conway(Bitmap *bitmap) {
	byte *new_map = calloc(bitmap->height * bitmap->width_bytes, sizeof(byte));
	byte *zeros = calloc(bitmap->width_bytes, sizeof(byte));
	
	// set prev row to zeros to avoid handling first row seperately
	byte *prev_row = zeros;
	for (uint16 r = 0; r < bitmap->height; r++) {
		uint8 *counts;
		byte *curr_row = bitmap->map + r*bitmap->width_bytes;

		// Determine if it's the last row, to set row_below to zeros if so
		if (r != bitmap->height - 1)
			counts = count_neighbors_row(bitmap->width, bitmap->width_bytes, curr_row, prev_row, bitmap->map + (r+1)*bitmap->width_bytes);
		else
			counts = count_neighbors_row(bitmap->width, bitmap->width_bytes, curr_row, prev_row, zeros);

		// decide fate of the current row and set it in the new map
		byte *new_fate = decide_fate_row(curr_row, bitmap->width, bitmap->width_bytes, counts);
		memmove(new_map + r*bitmap->width_bytes, new_fate, sizeof(byte) * bitmap->width_bytes);

		// update prev row
		prev_row = curr_row;
	}
	free(bitmap->map);
	free(zeros);
	bitmap->map = new_map;
}

