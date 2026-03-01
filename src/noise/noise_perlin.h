#ifndef NOISE_PERLIN_H
#define NOISE_PERLIN_H

#include <utils/random_java.h>

struct noise {
	double x_coord, y_coord, z_coord;
	int32_t permutations[512];
};

struct noise_octaves {
	size_t octaves;
	struct noise* base;
};

/**
 * @brief A faithful reimplementation of the Infdev and Beta perlin noise
 * generator
 */
void noise_perlin_create(struct noise* np, struct random_java* rand);

/**
 * @brief This is a rather standard implementation of "improved perlin noise",
 * as described by Ken Perlin in 2002. This version is mainly used by the infdev
 * generator, but beta still implements and uses it.
 */
double noise_perlin_sample(struct noise* np, double x, double y, double z);

void noise_perlin_sample_field(struct noise* np, double* noise_field,
							   double offset_x, double offset_y,
							   double offset_z, size_t size_x, size_t size_y,
							   size_t size_z, double scale_x, double scale_y,
							   double scale_z, double amplitude);

void noise_perlin_octaves_create(struct noise_octaves* np,
								 struct random_java* rand, size_t octaves);

void noise_perlin_octaves_destroy(struct noise_octaves* np);

double noise_perlin_octaves_sample3d(struct noise_octaves* np, double x_offset,
									 double y_offset, double z_offset);

double noise_perlin_octaves_sample2d(struct noise_octaves* np, double x_offset,
									 double y_offset);

void noise_perlin_octaves_sample_field3d(struct noise_octaves* np,
										 double* noise_field, double offset_x,
										 double offset_y, double offset_z,
										 size_t size_x, size_t size_y,
										 size_t size_z, double scale_x,
										 double scale_y, double scale_z);

void noise_perlin_octaves_sample_field2d(struct noise_octaves* np,
										 double* noise_field, double offset_x,
										 double offset_z, size_t size_x,
										 size_t size_z, double scale_x,
										 double scale_z);

void noise_simplex_sample_field(struct noise* np, double* noise_field,
								double offset_x, double offset_y, size_t size_x,
								size_t size_y, double scale_x, double scale_y,
								double amplitude);

void noise_simplex_octaves_sample_field2d(struct noise_octaves* np,
										  double* noise_field, double offset_x,
										  double offset_z, size_t size_x,
										  size_t size_z, double scale_x,
										  double scale_z, double scale);

#endif