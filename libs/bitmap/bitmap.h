extern Bitmap init_bitmap();
extern byte get_bit(Bitmap bitmap, uint16 row, uint16 col);
extern void invert_bit(Bitmap *bitmap, uint16 row, uint16 col);
extern void convolve_conway(Bitmap *bitmap);

