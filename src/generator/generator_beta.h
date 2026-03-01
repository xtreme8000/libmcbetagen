#ifndef GENERATOR_BETA_H
#define GENERATOR_BETA_H

#include <generator/generator_biome.h>
#include <generator/generator_chunk.h>
#include <noise/noise_perlin.h>

#define CHUNK_SAMPLES_XZ (CHUNK_WIDTH / 4 + 1)
#define CHUNK_SAMPLES_Y (CHUNK_HEIGHT / 8 + 1)

#define TERRAIN_SCALE 684.412

struct generator {
	int64_t seed;

	struct generator_biome biome;

	struct noise_octaves noise_continentalness;
	struct noise_octaves noise_depth;
	struct noise_octaves noise_selector;
	struct noise_octaves noise_low;
	struct noise_octaves noise_high;

	struct noise_octaves noise_sand;
	struct noise_octaves noise_stone;
	struct noise_octaves noise_tree;

	double* continentalness_field;
	double* depth_field;
	double* selector_field;
	double* low_field;
	double* high_field;
	double* terrain_field;

	double* sand_field;
	double* gravel_field;
	double* stone_field;
};

void generator_create(struct generator* g, int64_t seed);

void generator_destroy(struct generator* g);

void generator_sample(struct generator* g, struct generator_chunk* c);

#endif