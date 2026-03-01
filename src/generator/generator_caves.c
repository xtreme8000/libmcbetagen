#include <stdbool.h>
#include <stddef.h>

#include <generator/generator_caves.h>
#include <utils/math_helper.h>
#include <utils/math_java.h>
#include <utils/random_java.h>

#include <stdio.h>

#define CARVE_EXTENT_LIMIT 8
#define TUNNEL_LAVA_LEVEL 10

static void carve_cave(struct generator_chunk* g, struct random_java* rand,
					   double offset_x, double offset_y, double offset_z,
					   float tunnelRadius, float carveYaw, float carvePitch,
					   int32_t tunnelStep, int32_t tunnelLength,
					   double verticalScale) {
	double chunkCenterX = g->x * CHUNK_WIDTH + 8;
	double chunkCenterZ = g->z * CHUNK_WIDTH + 8;
	float var21 = 0.0F;
	float var22 = 0.0F;
	struct random_java rand2;
	random_java_create(&rand2, random_java_next_long(rand));

	if(tunnelLength <= 0) {
		int32_t max_tunnel_length = CARVE_EXTENT_LIMIT * 16 - 16;
		tunnelLength = max_tunnel_length
			- random_java_next_int_bound(&rand2, max_tunnel_length / 4);
	}

	bool branch_tunnel = tunnelStep == -1;
	tunnelStep = branch_tunnel ? tunnelLength / 2 : tunnelStep;

	int32_t var25 = random_java_next_int_bound(&rand2, tunnelLength / 2)
		+ tunnelLength / 4;

	bool tunnel_steepness = random_java_next_int_bound(&rand2, 6) == 0;
	while(tunnelStep < tunnelLength) {
		double radius_xz = 1.5
			+ (double)(math_helper_sin((float)tunnelStep * MATH_JAVA_PI_F
									   / (float)tunnelLength)
					   * tunnelRadius * 1.0F);
		double radius_y = radius_xz * verticalScale;
		offset_x += math_helper_cos(carveYaw) * math_helper_cos(carvePitch);
		offset_y += math_helper_sin(carvePitch);
		offset_z += math_helper_sin(carveYaw) * math_helper_cos(carvePitch);
		carvePitch *= tunnel_steepness ? 0.92F : 0.7F;
		carvePitch += var22 * 0.1F;
		carveYaw += var21 * 0.1F;
		var22 *= 0.9F;
		var21 *= 12.0F / 16.0F;
		var22
			+= (random_java_next_float(&rand2) - random_java_next_float(&rand2))
			* random_java_next_float(&rand2) * 2.0F;
		var21
			+= (random_java_next_float(&rand2) - random_java_next_float(&rand2))
			* random_java_next_float(&rand2) * 4.0F;
		if(!branch_tunnel && tunnelStep == var25 && tunnelRadius > 1.0F) {
			carve_cave(g, rand, offset_x, offset_y, offset_z,
					   random_java_next_float(&rand2) * 0.5F + 0.5F,
					   carveYaw - MATH_JAVA_PI_F * 0.5F, carvePitch / 3.0F,
					   tunnelStep, tunnelLength, 1.0);
			carve_cave(g, rand, offset_x, offset_y, offset_z,
					   random_java_next_float(&rand2) * 0.5F + 0.5F,
					   carveYaw + MATH_JAVA_PI_F * 0.5F, carvePitch / 3.0F,
					   tunnelStep, tunnelLength, 1.0);
			return;
		}

		if(branch_tunnel || random_java_next_int_bound(&rand2, 4) != 0) {
			double var33 = offset_x - chunkCenterX;
			double var35 = offset_z - chunkCenterZ;
			double var37 = tunnelLength - tunnelStep;
			double var39 = tunnelRadius + 2.0F + CHUNK_WIDTH;
			if(var33 * var33 + var35 * var35 - var37 * var37 > var39 * var39)
				return;

			if(offset_x >= chunkCenterX - CHUNK_WIDTH - radius_xz * 2.0
			   && offset_z >= chunkCenterZ - CHUNK_WIDTH - radius_xz * 2.0
			   && offset_x <= chunkCenterX + CHUNK_WIDTH + radius_xz * 2.0
			   && offset_z <= chunkCenterZ + CHUNK_WIDTH + radius_xz * 2.0) {
				int32_t xMin = math_helper_floor_double(offset_x - radius_xz)
					- g->x * CHUNK_WIDTH - 1;
				int32_t xMax = math_helper_floor_double(offset_x + radius_xz)
					- g->x * CHUNK_WIDTH + 1;
				int32_t yMin
					= math_helper_floor_double(offset_y - radius_y) - 1;
				int32_t yMax
					= math_helper_floor_double(offset_y + radius_y) + 1;
				int32_t zMin = math_helper_floor_double(offset_z - radius_xz)
					- g->z * CHUNK_WIDTH - 1;
				int32_t zMax = math_helper_floor_double(offset_z + radius_xz)
					- g->z * CHUNK_WIDTH + 1;
				// limit to chunk boundaries
				if(xMin < 0)
					xMin = 0;
				if(xMax > CHUNK_WIDTH)
					xMax = CHUNK_WIDTH;
				if(yMin < 1)
					yMin = 1;
				if(yMax > 120)
					yMax = 120;
				if(zMin < 0)
					zMin = 0;
				if(zMax > CHUNK_WIDTH)
					zMax = CHUNK_WIDTH;

				// test if there is water anywhere in the chunk
				bool water_present = false;
				for(int32_t blockX = xMin; !water_present && blockX < xMax;
					blockX++) {
					for(int32_t blockZ = zMin; !water_present && blockZ < zMax;
						blockZ++) {
						for(int32_t blockY = yMax + 1;
							!water_present && blockY >= yMin - 1; blockY--) {
							if(blockY >= 0 && blockY < CHUNK_HEIGHT) {
								if(!water_present) {
									uint8_t type = g->get_block(g, blockX,
																blockY, blockZ);
									water_present = type == BLOCK_WATER_FLOWING
										|| type == BLOCK_WATER_STILL;
								}

								if(blockY != yMin - 1 && blockX != xMin
								   && blockX != xMax - 1 && blockZ != zMin
								   && blockZ != zMax - 1)
									blockY = yMin;
							}
						}
					}
				}

				if(!water_present) {
					for(int32_t blockX = xMin; blockX < xMax; blockX++) {
						double center_dx
							= ((double)(blockX + g->x * CHUNK_WIDTH) + 0.5
							   - offset_x)
							/ radius_xz;

						for(int32_t blockZ = zMin; blockZ < zMax; blockZ++) {
							double center_dz
								= ((double)(blockZ + g->z * CHUNK_WIDTH) + 0.5
								   - offset_z)
								/ radius_xz;
							bool found_grass = false;
							if(center_dx * center_dx + center_dz * center_dz
							   < 1.0) {
								for(int32_t blockY = yMax - 1; blockY >= yMin;
									blockY--) {
									double center_dy
										= ((double)blockY + 0.5 - offset_y)
										/ radius_y;
									if(center_dy > -0.7
									   && center_dx * center_dx
											   + center_dy * center_dy
											   + center_dz * center_dz
										   < 1.0) {
										uint8_t type = g->get_block(
											g, blockX, blockY + 1, blockZ);
										found_grass = found_grass
											|| type == BLOCK_GRASS;

										if(type == BLOCK_STONE
										   || type == BLOCK_DIRT
										   || type == BLOCK_GRASS) {
											if(blockY < TUNNEL_LAVA_LEVEL) {
												g->set_block(
													g, blockX, blockY + 1,
													blockZ, BLOCK_LAVA_FLOWING);
											} else {
												g->set_block(g, blockX,
															 blockY + 1, blockZ,
															 BLOCK_AIR);
												if(found_grass
												   && g->get_block(g, blockX,
																   blockY,
																   blockZ)
													   == BLOCK_DIRT) {
													g->set_block(g, blockX,
																 blockY, blockZ,
																 BLOCK_GRASS);
												}
											}
										}
									}
								}
							}
						}
					}

					if(branch_tunnel)
						break;
				}
			}
		}
		tunnelStep++;
	}
}

// this is only the cave generator for the overworld, nether is different
static void generate_caves(struct generator_chunk* g, struct random_java* rand,
						   int32_t chunk_off_x, int32_t chunk_off_z) {
	size_t cave_count = random_java_next_int_bound(
		rand,
		random_java_next_int_bound(rand,
								   random_java_next_int_bound(rand, 40) + 1)
			+ 1);

	if(random_java_next_int_bound(rand, 15) != 0)
		cave_count = 0;

	for(size_t k = 0; k < cave_count; k++) {
		double offset_x = chunk_off_x * CHUNK_WIDTH
			+ random_java_next_int_bound(rand, CHUNK_WIDTH);
		double offset_y = random_java_next_int_bound(
			rand, random_java_next_int_bound(rand, 120) + 8);
		double offset_z = chunk_off_z * CHUNK_WIDTH
			+ random_java_next_int_bound(rand, CHUNK_WIDTH);

		size_t node_count = 1;
		if(random_java_next_int_bound(rand, 4) == 0) {
			carve_cave(g, rand, offset_x, offset_y, offset_z,
					   1.0F + random_java_next_float(rand) * 6.0F, 0.0F, 0.0F,
					   -1, -1, 0.5);
			node_count += random_java_next_int_bound(rand, 4);
		}

		for(size_t i = 0; i < node_count; i++) {
			float carveYaw
				= random_java_next_float(rand) * MATH_JAVA_PI_F * 2.0F;
			float carvePitch
				= (random_java_next_float(rand) - 0.5F) * 2.0F / 8.0F;
			float tunnelRadius = random_java_next_float(rand) * 2.0F
				+ random_java_next_float(rand);
			carve_cave(g, rand, offset_x, offset_y, offset_z, tunnelRadius,
					   carveYaw, carvePitch, 0, 0, 1.0);
		}
	}
}

// attempts to generate a cave in the current chunk
void generator_caves_sample(struct generator_chunk* g, int64_t seed) {
	struct random_java rand;
	random_java_create(&rand, seed);

	int64_t xOffset = random_java_next_long(&rand) / 2 * 2 + 1;
	int64_t zOffset = random_java_next_long(&rand) / 2 * 2 + 1;

	// iterate beyond the current chunk by 8 chunks in every direction
	for(int32_t cXoffset = g->x - CARVE_EXTENT_LIMIT;
		cXoffset <= g->x + CARVE_EXTENT_LIMIT; cXoffset++) {
		for(int32_t cZoffset = g->z - CARVE_EXTENT_LIMIT;
			cZoffset <= g->z + CARVE_EXTENT_LIMIT; cZoffset++) {
			random_java_set_seed(
				&rand, (cXoffset * xOffset + cZoffset * zOffset) ^ seed);
			generate_caves(g, &rand, cXoffset, cZoffset);
		}
	}
}