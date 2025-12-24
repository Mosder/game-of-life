#include <stdlib.h>
#include <string.h>

extern Bitmap init_bitmap();
extern void set_byte(Bitmap *bitmap, uint16 row, uint16 col_byte, byte val);
extern byte get_bit(Bitmap bitmap, uint16 row, uint16 col);
extern void invert_bit(Bitmap *bitmap, uint16 row, uint16 col);
extern void convolve_conway(Bitmap *bitmap);

