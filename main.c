#include "libs/common.h"
#include "libs/bitmap/bitmap.h"
#include "libs/setup/setup.h"
#include "libs/render/render.h"
#include <time.h>

void sleep_ms(uint16 sleep_time_ms) {
	struct timespec sleep_time_struct;
	sleep_time_struct.tv_sec = sleep_time_ms / 1000;
	sleep_time_struct.tv_nsec = (sleep_time_ms % 1000) * 1e6;
	nanosleep(&sleep_time_struct, NULL);
}

void setup_bitmap(Bitmap *bitmap) {
	uint16 row = 0;
	uint16 col = 0;
	do {
		clear_terminal();
		print_setup_bitmap(*bitmap, row, col);
		print_setup_keys();
	} while (!setup_step(bitmap, &row, &col));
}

void play(Bitmap bitmap, uint16 sleep_time_ms) {
	while (1) {
		clear_terminal();
		print_bitmap(bitmap);
		sleep_ms(sleep_time_ms);
		convolve_conway(&bitmap);
	}
}

int main() {
	clear_terminal();
	Bitmap bitmap = init_bitmap();

	uint16 sleep_time_ms = 0;
	while (sleep_time_ms == 0) {
		printf("Delay between steps [ms] (1-65535): ");
		scanf("%hu", &sleep_time_ms);
	}

	setup_bitmap(&bitmap);
	play(bitmap, sleep_time_ms);
}

