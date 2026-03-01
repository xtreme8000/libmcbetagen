#ifndef GENERATOR_BIOME_H
#define GENERATOR_BIOME_H

#include <generator/generator_chunk.h>
#include <noise/noise_perlin.h>

enum biome {
	BIOME_NONE = 0,
	BIOME_RAINFOREST = 1,
	BIOME_SWAMPLAND = 2,
	BIOME_SEASONALFOREST = 3,
	BIOME_FOREST = 4,
	BIOME_SAVANNA = 5,
	BIOME_SHRUBLAND = 6,
	BIOME_TAIGA = 7,
	BIOME_DESERT = 8,
	BIOME_PLAINS = 9,
	BIOME_ICEDESERT = 10,
	BIOME_TUNDRA = 11,
	BIOME_HELL = 12,
	BIOME_SKY = 13,
};

struct generator_biome {
	struct noise_octaves noise_temperature;
	struct noise_octaves noise_humidity;
	struct noise_octaves noise_weirdness;

	enum biome biome[CHUNK_WIDTH * CHUNK_WIDTH];
	double temperature[CHUNK_WIDTH * CHUNK_WIDTH];
	double humidity[CHUNK_WIDTH * CHUNK_WIDTH];
	double weirdness[CHUNK_WIDTH * CHUNK_WIDTH];
};

/**
 * @brief Construct a new Beta 1.7.3 Biome
 *
 * @param seed The world seed that the biome-generator will use
 */
void generator_biome_create(struct generator_biome* g, int64_t seed);

enum biome generator_biome_lookup(double temperature, double humidity);

/**
 * @brief Generate Biomes based on simplex noise and updates the temperature,
 * humidity and weirdness maps
 *
 * @param blockPos The x,z block-space coordindate of the chunk
 * @param max The size of the area that'll be generated (16x16 by default)
 */
void generator_biome_sample_field(struct generator_biome* g, int32_t block_x,
								  int32_t block_z, size_t max_xz);
#endif