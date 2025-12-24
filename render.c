#include "common.h"
#include "bitmap.h"
#include "render.h"

#define CELL_WIDTH 2
#define ALIVE_STR "█"
#define DEAD_STR " "
#define ROW_SPLIT_STR "-"
#define COL_SPLIT_STR "|"
#define CURRENT_CELL_STR ">"

// Clear terminal
void clear_terminal() {
	printf("\e[1;1H\e[2J");
}

// Prints a line that splits rows
void print_row_split(Bitmap bitmap) {
	for (uint16 c = 0; c < bitmap.width; c++)
		for (uint8 i = 0; i < CELL_WIDTH+1; i++)
			printf("%s", ROW_SPLIT_STR);
	printf("%s\n", ROW_SPLIT_STR);
}

// Prints bitmap on a terminal
void print_bitmap(Bitmap bitmap) {
	for (uint16 r = 0; r < bitmap.height; r++) {
		print_row_split(bitmap);
		for (uint16 c = 0; c < bitmap.width; c++) {
			printf("%s", COL_SPLIT_STR);
			char *cell_str = get_bit(bitmap, r, c) ? ALIVE_STR : DEAD_STR;
			for (uint8 i = 0; i < CELL_WIDTH; i++)
				printf("%s", cell_str);
		}
		printf("%s\n", COL_SPLIT_STR);
	}
	print_row_split(bitmap);
	for (uint16 r = 0; r < bitmap.height; r++) {
		for (uint16 c = 0; c < bitmap.width; c++) {
			printf("%hhu", get_bit(bitmap, r, c));
		}
		printf("\n");
	}
}

// Print setup bitmap (before starting) on a terminal
void print_setup_bitmap(Bitmap bitmap, uint16 row, uint16 col) {
	for (uint16 r = 0; r < bitmap.height; r++) {
		print_row_split(bitmap);
		for (uint16 c = 0; c < bitmap.width; c++) {
			uint8 current_cell = (r == row) && (c == col) ? 1 : 0;
			printf("%s", COL_SPLIT_STR);
			char *cell_str = get_bit(bitmap, r, c) ? ALIVE_STR : DEAD_STR;
			if (current_cell)
				printf("%s", CURRENT_CELL_STR);
			for (uint8 i = 0; i < CELL_WIDTH - current_cell; i++)
				printf("%s", cell_str);
		}
		printf("%s\n", COL_SPLIT_STR);
	}
	print_row_split(bitmap);
}
