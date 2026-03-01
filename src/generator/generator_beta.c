#include <assert.h>
#include <stdlib.h>

#include <generator/generator_beta.h>
#include <generator/generator_caves.h>

void generator_create(struct generator* g, int64_t seed) {
	assert(g);

	g->seed = seed;

	struct random_java r;
	random_java_create(&r, seed);

	noise_perlin_octaves_create(&g->noise_low, &r, 16);
	noise_perlin_octaves_create(&g->noise_high, &r, 16);
	noise_perlin_octaves_create(&g->noise_selector, &r, 8);
	noise_perlin_octaves_create(&g->noise_sand, &r, 4);
	noise_perlin_octaves_create(&g->noise_stone, &r, 4);
	noise_perlin_octaves_create(&g->noise_continentalness, &r, 10);
	noise_perlin_octaves_create(&g->noise_depth, &r, 16);
	noise_perlin_octaves_create(&g->noise_tree, &r, 8);

	g->continentalness_field
		= malloc(sizeof(double) * CHUNK_SAMPLES_XZ * CHUNK_SAMPLES_XZ);
	g->depth_field
		= malloc(sizeof(double) * CHUNK_SAMPLES_XZ * CHUNK_SAMPLES_XZ);
	g->selector_field = malloc(sizeof(double) * CHUNK_SAMPLES_XZ
							   * CHUNK_SAMPLES_XZ * CHUNK_SAMPLES_Y);
	g->low_field = malloc(sizeof(double) * CHUNK_SAMPLES_XZ * CHUNK_SAMPLES_XZ
						  * CHUNK_SAMPLES_Y);
	g->high_field = malloc(sizeof(double) * CHUNK_SAMPLES_XZ * CHUNK_SAMPLES_XZ
						   * CHUNK_SAMPLES_Y);
	g->terrain_field = malloc(sizeof(double) * CHUNK_SAMPLES_XZ
							  * CHUNK_SAMPLES_XZ * CHUNK_SAMPLES_Y);

	g->sand_field = malloc(sizeof(double) * CHUNK_WIDTH * CHUNK_WIDTH);
	g->gravel_field = malloc(sizeof(double) * CHUNK_WIDTH * CHUNK_WIDTH);
	g->stone_field = malloc(sizeof(double) * CHUNK_WIDTH * CHUNK_WIDTH);

	generator_biome_create(&g->biome, seed);
}

void generator_destroy(struct generator* g) {
	assert(g);

	free(g->continentalness_field);
	free(g->depth_field);
	free(g->selector_field);
	free(g->low_field);
	free(g->high_field);
	free(g->terrain_field);

	free(g->sand_field);
	free(g->gravel_field);
	free(g->stone_field);
}

/**
 * @brief Make terrain noise and updates the terrain map
 *
 * @param terrainMap The terrain map that the scaled-down terrain values will be
 * written to
 * @param chunkPos The x,y,z coordinate of the sub-chunk
 * @param max Defines the area of the terrainMap
 */
static void generator_terrain_noise(struct generator* g, double* terrain,
									int32_t chunk_x, int32_t chunk_y,
									int32_t chunk_z, size_t max_x, size_t max_y,
									size_t max_z) {
	assert(g && terrain);

	for(size_t k = 0; k < max_x * max_y * max_z; k++)
		terrain[k] = 0;

	// we do this to not need to generate noise as often
	noise_perlin_octaves_sample_field2d(&g->noise_continentalness,
										g->continentalness_field, chunk_x,
										chunk_z, max_x, max_z, 1.121, 1.121);
	noise_perlin_octaves_sample_field2d(&g->noise_depth, g->depth_field,
										chunk_x, chunk_z, max_x, max_z, 200.0,
										200.0);
	noise_perlin_octaves_sample_field3d(
		&g->noise_selector, g->selector_field, chunk_x, chunk_y, chunk_z, max_x,
		max_y, max_z, TERRAIN_SCALE / 80.0, TERRAIN_SCALE / 160.0,
		TERRAIN_SCALE / 80.0);
	noise_perlin_octaves_sample_field3d(
		&g->noise_low, g->low_field, chunk_x, chunk_y, chunk_z, max_x, max_y,
		max_z, TERRAIN_SCALE, TERRAIN_SCALE, TERRAIN_SCALE);
	noise_perlin_octaves_sample_field3d(
		&g->noise_high, g->high_field, chunk_x, chunk_y, chunk_z, max_x, max_y,
		max_z, TERRAIN_SCALE, TERRAIN_SCALE, TERRAIN_SCALE);

	// used to iterate 3D noise maps (low, high, selector)
	size_t xyz_idx = 0;
	// used to iterate 2D Noise maps (depth, continentalness)
	size_t xz_idx = 0;
	size_t s_frac = 16 / max_x;

	for(size_t iX = 0; iX < max_x; iX++) {
		size_t sample_x = iX * s_frac + s_frac / 2;

		for(size_t iZ = 0; iZ < max_z; iZ++) {
			// sample 2D noises
			size_t sample_z = iZ * s_frac + s_frac / 2;

			// apply biome-noise-dependent variety
			double temp
				= g->biome.temperature[sample_x * CHUNK_WIDTH + sample_z];
			double humi
				= g->biome.humidity[sample_x * CHUNK_WIDTH + sample_z] * temp;
			humi = 1.0 - (1.0 - humi) * humi * humi;

			// apply contientalness
			double continentalness
				= (g->continentalness_field[xz_idx] + 256.0) / 512.0;
			continentalness *= humi;
			if(continentalness > 1.0)
				continentalness = 1.0;

			double depth_noise = g->depth_field[xz_idx] / 8000.0;
			if(depth_noise < 0.0)
				depth_noise = -depth_noise * 0.3;

			depth_noise = depth_noise * 3.0 - 2.0;
			if(depth_noise < 0.0) {
				depth_noise /= 2.0;
				if(depth_noise < -1.0)
					depth_noise = -1.0;

				depth_noise /= 1.4;
				depth_noise /= 2.0;
				continentalness = 0.0;
			} else {
				if(depth_noise > 1.0)
					depth_noise = 1.0;
				depth_noise /= 8.0;
			}

			if(continentalness < 0.0)
				continentalness = 0.0;

			continentalness += 0.5;
			depth_noise = depth_noise * (double)max_y / 16.0;
			double elevation_offset = (double)max_y / 2.0 + depth_noise * 4.0;
			xz_idx++;

			for(size_t iY = 0; iY < max_y; iY++) {
				// sample 3D noises
				double terrain_density = 0.0;
				double density_offset
					= ((double)iY - elevation_offset) * 12.0 / continentalness;
				if(density_offset < 0.0)
					density_offset *= 4.0;

				double low_noise = g->low_field[xyz_idx] / 512.0;
				double high_noise = g->high_field[xyz_idx] / 512.0;
				double selector_noise
					= (g->selector_field[xyz_idx] / 10.0 + 1.0) / 2.0;
				if(selector_noise < 0.0) {
					terrain_density = low_noise;
				} else if(selector_noise > 1.0) {
					terrain_density = high_noise;
				} else {
					terrain_density
						= low_noise + (high_noise - low_noise) * selector_noise;
				}

				terrain_density -= density_offset;

				// reduce density towards max height
				if(iY + 4 > max_y) {
					double height_edge_fade
						= (double)((float)(iY + 4 - max_y) / 3.0F);
					terrain_density = terrain_density * (1.0 - height_edge_fade)
						+ -10.0 * height_edge_fade;
				}

				terrain[xyz_idx] = terrain_density;
				xyz_idx++;
			}
		}
	}
}

void generator_terrain(struct generator* g, struct generator_chunk* c) {
	assert(g);

	// generate 4x16x4 low resolution noise map
	generator_terrain_noise(g, g->terrain_field, c->x * 4, 0, c->z * 4,
							CHUNK_SAMPLES_XZ, CHUNK_SAMPLES_Y,
							CHUNK_SAMPLES_XZ);

	// terrain noise is interpolated and only sampled every 4 blocks
	for(size_t macroX = 0; macroX < CHUNK_SAMPLES_XZ - 1; macroX++) {
		for(size_t macroZ = 0; macroZ < CHUNK_SAMPLES_XZ - 1; macroZ++) {
			for(size_t macroY = 0; macroY < CHUNK_SAMPLES_Y - 1; macroY++) {
				double verticalLerpStep = 0.125;

				// get noise cube corners
				double corner000
					= g->terrain_field[((macroX + 0) * CHUNK_SAMPLES_XZ + macroZ
										+ 0)
										   * CHUNK_SAMPLES_Y
									   + macroY + 0];
				double corner010
					= g->terrain_field[((macroX + 0) * CHUNK_SAMPLES_XZ + macroZ
										+ 1)
										   * CHUNK_SAMPLES_Y
									   + macroY + 0];
				double corner100
					= g->terrain_field[((macroX + 1) * CHUNK_SAMPLES_XZ + macroZ
										+ 0)
										   * CHUNK_SAMPLES_Y
									   + macroY + 0];
				double corner110
					= g->terrain_field[((macroX + 1) * CHUNK_SAMPLES_XZ + macroZ
										+ 1)
										   * CHUNK_SAMPLES_Y
									   + macroY + 0];
				double corner001
					= (g->terrain_field[((macroX + 0) * CHUNK_SAMPLES_XZ
										 + macroZ + 0)
											* CHUNK_SAMPLES_Y
										+ macroY + 1]
					   - corner000)
					* verticalLerpStep;
				double corner011
					= (g->terrain_field[((macroX + 0) * CHUNK_SAMPLES_XZ
										 + macroZ + 1)
											* CHUNK_SAMPLES_Y
										+ macroY + 1]
					   - corner010)
					* verticalLerpStep;
				double corner101
					= (g->terrain_field[((macroX + 1) * CHUNK_SAMPLES_XZ
										 + macroZ + 0)
											* CHUNK_SAMPLES_Y
										+ macroY + 1]
					   - corner100)
					* verticalLerpStep;
				double corner111
					= (g->terrain_field[((macroX + 1) * CHUNK_SAMPLES_XZ
										 + macroZ + 1)
											* CHUNK_SAMPLES_Y
										+ macroY + 1]
					   - corner110)
					* verticalLerpStep;

				// interpolate the 1/4th scale noise
				for(size_t subY = 0; subY < 8; subY++) {
					double horizontalLerpStep = 0.25;
					double terrainX0 = corner000;
					double terrainX1 = corner010;
					double terrainStepX0
						= (corner100 - corner000) * horizontalLerpStep;
					double terrainStepX1
						= (corner110 - corner010) * horizontalLerpStep;

					for(size_t subX = 0; subX < 4; subX++) {
						double terrainDensity = terrainX0;
						double densityStepZ
							= (terrainX1 - terrainX0) * horizontalLerpStep;

						for(size_t subZ = 0; subZ < 4; subZ++) {
							uint8_t blockType = BLOCK_AIR;

							// if water is too cold, turn into ice
							double temp
								= g->biome.temperature[(macroX * 4 + subX) * 16
													   + macroZ * 4 + subZ];
							int yLevel = macroY * 8 + subY;
							if(yLevel < WATER_LEVEL) {
								if(temp < 0.5 && yLevel >= WATER_LEVEL - 1) {
									blockType = BLOCK_ICE;
								} else {
									blockType = BLOCK_WATER_STILL;
								}
							}

							if(terrainDensity > 0.0)
								blockType = BLOCK_STONE;

							c->set_block(c, macroX * 4 + subX,
										 (macroY * 8) + subY, macroZ * 4 + subZ,
										 blockType);

							// prep for next iteration
							terrainDensity += densityStepZ;
						}

						terrainX0 += terrainStepX0;
						terrainX1 += terrainStepX1;
					}

					corner000 += corner001;
					corner010 += corner011;
					corner100 += corner101;
					corner110 += corner111;
				}
			}
		}
	}
}

uint8_t GetTopBlock(enum biome biome) {
	if(biome == BIOME_DESERT || biome == BIOME_ICEDESERT) {
		return BLOCK_SAND;
	}
	return BLOCK_GRASS;
}

uint8_t GetFillerBlock(enum biome biome) {
	if(biome == BIOME_DESERT || biome == BIOME_ICEDESERT) {
		return BLOCK_SAND;
	}
	return BLOCK_DIRT;
}

void generator_replace_biome(struct generator* g, struct random_java* rand,
							 struct generator_chunk* c) {
	const double oneThirtySecond = 1.0 / 32.0;

	noise_perlin_octaves_sample_field3d(&g->noise_sand, g->sand_field,
										c->x * CHUNK_WIDTH, c->z * CHUNK_WIDTH,
										0.0, CHUNK_WIDTH, CHUNK_WIDTH, 1,
										oneThirtySecond, oneThirtySecond, 1.0);
	noise_perlin_octaves_sample_field3d(
		&g->noise_sand, g->gravel_field, c->x * CHUNK_WIDTH, 109.0134,
		c->z * CHUNK_WIDTH, CHUNK_WIDTH, 1, CHUNK_WIDTH, oneThirtySecond, 1.0,
		oneThirtySecond);
	noise_perlin_octaves_sample_field3d(
		&g->noise_stone, g->stone_field, c->x * CHUNK_WIDTH, c->z * CHUNK_WIDTH,
		0.0, CHUNK_WIDTH, CHUNK_WIDTH, 1, oneThirtySecond * 2.0,
		oneThirtySecond * 2.0, oneThirtySecond * 2.0);

	// Iterate through entire chunk
	for(int32_t x = 0; x < CHUNK_WIDTH; x++) {
		for(int32_t z = 0; z < CHUNK_WIDTH; z++) {
			// Get values from noise maps
			enum biome biome = g->biome.biome[x + z * CHUNK_WIDTH];
			bool sandActive = g->sand_field[x + z * CHUNK_WIDTH]
					+ random_java_next_double(rand) * 0.2
				> 0.0;
			bool gravelActive = g->gravel_field[x + z * CHUNK_WIDTH]
					+ random_java_next_double(rand) * 0.2
				> 3.0;
			int32_t stoneActive = g->stone_field[x + z * CHUNK_WIDTH] / 3.0
				+ 3.0 + random_java_next_double(rand) * 0.25;
			int32_t stoneDepth = -1;
			// Get biome-appropriate top and filler blocks
			uint8_t topBlock = GetTopBlock(biome);
			uint8_t fillerBlock = GetFillerBlock(biome);

			// Iterate over column top to bottom
			for(int32_t y = CHUNK_HEIGHT - 1; y >= 0; y--) {
				// place bedrock at bottom with some randomness
				if(y <= random_java_next_int_bound(rand, 5)) {
					c->set_block(c, x, y, z, BLOCK_BEDROCK);
					continue;
				}

				uint8_t currentBlock = c->get_block(c, x, y, z);
				// Ignore air
				if(currentBlock == BLOCK_AIR) {
					stoneDepth = -1;
					continue;
				}

				// If we counter stone, start replacing it
				if(currentBlock == BLOCK_STONE) {
					if(stoneDepth == -1) {
						if(stoneActive <= 0) {
							topBlock = BLOCK_AIR;
							fillerBlock = BLOCK_STONE;
						} else if(y >= WATER_LEVEL - 4
								  && y <= WATER_LEVEL + 1) {
							// If we're close to the water level, apply gravel
							// and sand
							topBlock = GetTopBlock(biome);
							fillerBlock = GetFillerBlock(biome);

							if(gravelActive) {
								topBlock = BLOCK_AIR;
								fillerBlock = BLOCK_GRAVEL;
							}

							if(sandActive) {
								topBlock = BLOCK_SAND;
								fillerBlock = BLOCK_SAND;
							}
						}

						// Add water if we're below water level
						if(y < WATER_LEVEL && topBlock == BLOCK_AIR)
							topBlock = BLOCK_WATER_STILL;

						stoneDepth = stoneActive;
						c->set_block(c, x, y, z,
									 y >= WATER_LEVEL - 1 ? topBlock :
															fillerBlock);
					} else if(stoneDepth > 0) {
						stoneDepth--;
						c->set_block(c, x, y, z, fillerBlock);
						if(stoneDepth == 0 && fillerBlock == BLOCK_SAND) {
							stoneDepth = random_java_next_int_bound(rand, 4);
							fillerBlock = BLOCK_SANDSTONE;
						}
					}
				}
			}
		}
	}
}

void generator_sample(struct generator* g, struct generator_chunk* c) {
	generator_biome_sample_field(&g->biome, c->x * CHUNK_WIDTH,
								 c->z * CHUNK_WIDTH, CHUNK_WIDTH);
	generator_terrain(g, c);

	struct random_java r;
	random_java_create(
		&r, (int64_t)c->x * 341873128712LL + (int64_t)c->z * 132897987541LL);
	generator_replace_biome(g, &r, c);

	generator_caves_sample(c, g->seed);
}