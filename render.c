#include "common.h"
#include "bitmap.h"
#include "render.h"

#define ALIVE_STR "██"
#define DEAD_STR "  "
#define ROW_SPLIT_STR "-"
#define COL_SPLIT_STR "|"

// Clear terminal
void clear_terminal() {
	printf("\e[1;1H\e[2J");
}

// Prints a line that splits rows
void print_row_split(Bitmap bitmap) {
	for (uint16 c = 0; c < bitmap.width; c++)
		printf("---");
	printf("-\n");
}

// Prints bitmap on a terminal
void print_bitmap(Bitmap bitmap) {
	for (uint16 r = 0; r < bitmap.height; r++) {
		print_row_split(bitmap);
		for (uint16 c = 0; c < bitmap.width; c++)
			printf("%s%s", COL_SPLIT_STR, get_bit(bitmap, r, c) ? ALIVE_STR : DEAD_STR);
		printf("%s\n", COL_SPLIT_STR);
	}
	print_row_split(bitmap);
}

