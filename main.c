#include "common.h"
#include "bitmap.h"
#include "render.h"
#include <unistd.h>

int main() {
	Bitmap bitmap = init_bitmap();
	set_byte(&bitmap, 1, 0, 0b00011100);
	set_byte(&bitmap, 3, 0, 0b00011000);
	set_byte(&bitmap, 4, 0, 0b00011000);
	while (1) {
		clear_terminal();
		print_bitmap(bitmap);
		sleep(1);
		convolve_conway(&bitmap);
	}
}
