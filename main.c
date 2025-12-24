#include "common.h"
#include "bitmap.h"
#include "setup.h"
#include "render.h"

void setup_bitmap(Bitmap *bitmap) {
	uint16 r = 0;
	uint16 c = 0;
	while (1) {
		clear_terminal();
		print_setup_bitmap(*bitmap, r, c);
		print_setup_keys();
		if (setup_step(bitmap, &r, &c))
			break;
	}
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
