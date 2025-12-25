#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef unsigned char byte;
typedef signed char int8;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

typedef struct {
	byte *map; // bitmap of size height*width_bytes
	uint16 height; // height in bits
	uint16 width; // width in bits
	uint16 width_bytes; // width in bytes
} Bitmap;

