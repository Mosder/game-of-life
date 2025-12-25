#include "libs/common.h"
#include "libs/bitmap/bitmap.h"
#include "libs/setup/setup.h"
#include "libs/render/render.h"

void setup_bitmap(Bitmap *bitmap) {
	uint16 row = 0;
	uint16 col = 0;
	do {
		clear_terminal();
		print_setup_bitmap(*bitmap, row, col);
		print_setup_keys();
	} while (!setup_step(bitmap, &row, &col));
}

void play(Bitmap bitmap) {
	while (1) {
		clear_terminal();
		print_bitmap(bitmap);
		sleep(1);
		convolve_conway(&bitmap);
	}
}

int main() {
	clear_terminal();
	Bitmap bitmap = init_bitmap();
	setup_bitmap(&bitmap);
	play(bitmap);
}

