#include "common.h"
#include "bitmap.h"
#include "setup.h"

// For movement - arrows
#define ARROWS_CH1 '\x1b'
#define ARROWS_CH2 '['
#define ARROWS_UP 'A'
#define ARROWS_DOWN 'B'
#define ARROWS_RIGHT 'C'
#define ARROWS_LEFT 'D'

// For movement - hjkl
#define UP 'k'
#define DOWN 'j'
#define RIGHT 'l'
#define LEFT 'h'

// For inverting a bit - c or space
#define ACCEPT 'c'
#define ACCEPT_ALT ' '

// For finishing setup - q or return
#define QUIT 'q'
#define QUIT_ALT '\n'

// Print setup keybinds
void print_setup_keys() {
	printf("Movement - %c%c%c%c / \u2190\u2193\u2191\u2192\n", RIGHT, DOWN, UP, LEFT);
	printf("Change bit - %c / Space\n", ACCEPT);
	printf("Finish setup - %c / Return\n", QUIT);
}

// Get character from stdin without waiting for new line
char get_char() {
	struct termios oldt, newt;
	char c;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);

	read(STDIN_FILENO, &c, 1);

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return c;
}

// Clump value, so it doesn't get out of bounds
uint16 clump_row_col(uint16 val, int8 change, uint16 max_val) {
	if (val == 0 && change == -1)
		return 0;
	if (val == max_val && change == 1)
		return max_val;
	return val + change;
}

// Do one step of setup
byte setup_step(Bitmap *bitmap, uint16 *row, uint16 *col) {
	switch (get_char()) {
		// handle movement - arrows
		case ARROWS_CH1:
			if (get_char() == ARROWS_CH2) {
				switch (get_char()) {
					case ARROWS_UP:
						*row = clump_row_col(*row, -1, bitmap->height-1);
						break;
					case ARROWS_DOWN:
						*row = clump_row_col(*row, 1, bitmap->height-1);
						break;
					case ARROWS_RIGHT:
						*col = clump_row_col(*col, 1, bitmap->width-1);
						break;
					case ARROWS_LEFT:
						*col = clump_row_col(*col, -1, bitmap->width-1);
						break;
				}
			}
			break;
		// handle movement - hjkl
		case UP:
			*row = clump_row_col(*row, -1, bitmap->height-1);
			break;
		case DOWN:
			*row = clump_row_col(*row, 1, bitmap->height-1);
			break;
		case RIGHT:
			*col = clump_row_col(*col, 1, bitmap->width-1);
			break;
		case LEFT:
			*col = clump_row_col(*col, -1, bitmap->width-1);
			break;
		// handle bit inversion
		case ACCEPT:
		case ACCEPT_ALT:
			invert_bit(bitmap, *row, *col);
			break;
		// handle setup finish
		case QUIT:
		case QUIT_ALT:
			return 1;
	}
	return 0;
}

