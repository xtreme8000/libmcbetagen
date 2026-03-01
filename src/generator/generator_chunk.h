#ifndef GENERATOR_CHUNK_H
#define GENERATOR_CHUNK_H

#include <stdint.h>

#define CHUNK_HEIGHT 128
#define CHUNK_WIDTH 16

#define WATER_LEVEL (CHUNK_HEIGHT / 2)

#define BLOCK_AIR 0
#define BLOCK_STONE 1
#define BLOCK_GRASS 2
#define BLOCK_DIRT 3
#define BLOCK_WATER_FLOWING 8
#define BLOCK_WATER_STILL 9
#define BLOCK_LAVA_FLOWING 10
#define BLOCK_LAVA_STILL 11
#define BLOCK_BRICKS 45
#define BLOCK_OBSIDIAN 49
#define BLOCK_ICE 79

#define BLOCK_BEDROCK 7
#define BLOCK_SAND 12
#define BLOCK_GRAVEL 13
#define BLOCK_SANDSTONE 24

struct generator_chunk {
	void* user;
	int32_t x, z;
	void (*set_block)(struct generator_chunk* g, int32_t x, int32_t y,
					  int32_t z, uint8_t type);
	void (*set_block_meta)(struct generator_chunk* g, int32_t x, int32_t y,
						   int32_t z, uint8_t type, uint8_t meta);
	uint8_t (*get_block)(struct generator_chunk* g, int32_t x, int32_t y,
						 int32_t z);
};

#endif