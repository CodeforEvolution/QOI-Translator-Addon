/*
 * Copyright 2022 Jacob Secunda, secundaja@gmail.com
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef QOI_DEFINITIONS_H
#define QOI_DEFINITIONS_H

/**************************************************************************************************
My primary source for creating this fine QOI Image Translator:
The Quite Ok Image (QOI) Format Specification: https://qoiformat.org/qoi-specification.pdf
***************************************************************************************************/

/** QOI File Header **/

// 'magic' Field -> qoi_header
#define QOI_MAGIC_LENGTH 4 // Bytes
static const char[QOI_MAGIC_LENGTH] QOI_MAGIC = "qoif";

// 'channels' Field -> qoi_header
#define QOI_RGB		3
#define QOI_RGBA	4

// 'colorspace' Field -> qoi_header
#define QOI_SRGB_WITH_LINEAR_ALPHA	0
#define QOI_ALL_CHANNELS_LINEAR		1

typedef struct {
	char	magic[QOI_MAGIC_LENGTH];
	uint32	width;
		// Width in pixels - Big endian
	uint32	height;
		// Height in pixels - Big endian
	uint8	channels;
		// Pixel channel count in image - Informative only
	uint8	colorspace;
		// Pixel channel format - Informative only
} qoi_header;


/** QOI Miscellaneous Types & Definitions **/

#define PREVIOUS_PIXELS_LENGTH 64
typedef uint8 previous_pixels[PREVIOUS_PIXELS_LENGTH];

static inline uint8 pixel_to_index_hash(uint8 red, uint8 green, uint8 blue, uint8 alpha) {
	return ((red * 3) + (green * 5) + (blue * 7) + (alpha * 11)) % PREVIOUS_PIXELS_LENGTH;
}


/** QOI_OP_RGB Chunk **/

#define QOI_OP_RGB_TAG 0b11111110

typedef struct {
	uint8	tag;
	uint8	red;
	uint8	green;
	uint8	blue;
} qoi_op_rgb;


/** QOI_OP_RGBA Chunk **/

#define QOI_OP_RGBA_TAG 0b11111111

typedef struct {
	uint8	tag;
	uint8	red;
	uint8	green;
	uint8	blue;
	uint8	alpha;
} qoi_op_rgba;


/** QOI_OP_INDEX Chunk **/

#define QOI_OP_INDEX_TAG 0b00

typedef struct {
	uint8 tag	: 2;
	uint8 index	: 6;
} qoi_op_index;


/** QOI_OP_DIFF Chunk **/

#define QOI_OP_DIFF_TAG 0b01

typedef struct {
	uint8 tag			: 2;
	uint8 red_diff		: 2;
	uint8 green_diff	: 2;
	uint8 blue_diff		: 2;
} qoi_op_diff;


/** QOI_OP_LUMA Chunk **/

#define QOI_OP_LUMA_TAG 0b10

typedef struct {
	uint8 tag				: 2;
	uint8 green_diff		: 6;
	uint8 red_green_diff	: 4;
	uint8 blue_green_diff	: 4;
} qoi_op_luma;


/** QOI_OP_RUN Chunk **/

#define QOI_OP_RUN_TAG 0b11

typedef struct {
	uint8 tag : 2;
	uint8 run : 6;
} qoi_op_run;

#endif // QOI_DEFINITIONS_H
