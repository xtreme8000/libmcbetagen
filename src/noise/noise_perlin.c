#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include <noise/noise_perlin.h>
#include <utils/math_java.h>

void noise_perlin_create(struct noise* np, struct random_java* rand) {
	assert(np && rand);

	np->x_coord = random_java_next_double(rand) * 256.0;
	np->y_coord = random_java_next_double(rand) * 256.0;
	np->z_coord = random_java_next_double(rand) * 256.0;

	for(size_t i = 0; i < 256; i++)
		np->permutations[i] = i;

	for(size_t i = 0; i < 256; i++) {
		int32_t j = random_java_next_int_bound(rand, 256 - i) + i;

		int32_t tmp = np->permutations[i];
		np->permutations[i] = np->permutations[j];
		np->permutations[j] = tmp;
		np->permutations[i + 256] = np->permutations[i];
	}
}

double noise_perlin_sample(struct noise* np, double x, double y, double z) {
	assert(np);

	x += np->x_coord;
	y += np->y_coord;
	z += np->z_coord;

	/* The farlands are caused by this getting cast to a 32-bit integer. Change
	 * these int32_t to int64_t to fix the farlands, however this will change
	 * beta tree generation slightly due to rounding differences. */
	int32_t x_int = (int32_t)x;
	int32_t y_int = (int32_t)y;
	int32_t z_int = (int32_t)z;

	if(x < (double)x_int)
		x_int--;
	if(y < (double)y_int)
		y_int--;
	if(z < (double)z_int)
		z_int--;

	int32_t x_idx = x_int & 255;
	int32_t y_idx = y_int & 255;
	int32_t z_idx = z_int & 255;

	x -= (double)x_int;
	y -= (double)y_int;
	z -= (double)z_int;
	double w = math_java_fade(x);
	double v = math_java_fade(y);
	double u = math_java_fade(z);
	int32_t perm_xy = np->permutations[x_idx] + y_idx;
	int32_t perm_xyz = np->permutations[perm_xy] + z_idx;

	/* Some of the following code is weird, probably because it got optimized by
	 * Java to use fewer variables or Notch did this to be efficient. */
	perm_xy = np->permutations[perm_xy + 1] + z_idx;
	x_idx = np->permutations[x_idx + 1] + y_idx;
	y_idx = np->permutations[x_idx] + z_idx;
	x_idx = np->permutations[x_idx + 1] + z_idx;

	return math_java_lerp(
		u,
		math_java_lerp(
			v,
			math_java_lerp(
				w, math_java_grad(np->permutations[perm_xyz], x, y, z),
				math_java_grad(np->permutations[y_idx], x - 1.0, y, z)),
			math_java_lerp(
				w, math_java_grad(np->permutations[perm_xy], x, y - 1.0, z),
				math_java_grad(np->permutations[x_idx], x - 1.0, y - 1.0, z))),
		math_java_lerp(
			v,
			math_java_lerp(
				w,
				math_java_grad(np->permutations[perm_xyz + 1], x, y, z - 1.0),
				math_java_grad(np->permutations[y_idx + 1], x - 1.0, y,
							   z - 1.0)),
			math_java_lerp(w,
						   math_java_grad(np->permutations[perm_xy + 1], x,
										  y - 1.0, z - 1.0),
						   math_java_grad(np->permutations[x_idx + 1], x - 1.0,
										  y - 1.0, z - 1.0))));
}

void noise_perlin_sample_field(struct noise* np, double* noise_field,
							   double offset_x, double offset_y,
							   double offset_z, size_t size_x, size_t size_y,
							   size_t size_z, double scale_x, double scale_y,
							   double scale_z, double amplitude) {
	assert(np && noise_field);
	size_t idx = 0;

	if(size_y == 1) {
		for(size_t x = 0; x < size_x; x++) {
			double fx = (offset_x + x) * scale_x + np->x_coord;
			int32_t ix = fx;
			if(fx < ix)
				ix--;
			int32_t px = ix & 255;
			fx -= ix;
			double u = math_java_fade(fx);

			for(size_t z = 0; z < size_z; z++) {
				double fz = (offset_z + z) * scale_z + np->z_coord;
				int32_t iz = fz;
				if(fz < iz)
					iz--;
				int32_t pz = iz & 255;
				fz -= iz;

				int32_t a = np->permutations[px];
				int32_t aa = np->permutations[a] + pz;
				int32_t b = np->permutations[px + 1];
				int32_t ba = np->permutations[b] + pz;

				double x1 = math_java_lerp(
					u, math_java_alt_grad(np->permutations[aa], fx, fz),
					math_java_grad(np->permutations[ba], fx - 1.0, 0.0, fz));

				double x2 = math_java_lerp(
					u,
					math_java_grad(np->permutations[aa + 1], fx, 0.0, fz - 1.0),
					math_java_grad(np->permutations[ba + 1], fx - 1.0, 0.0,
								   fz - 1.0));

				noise_field[idx++] += math_java_lerp(math_java_fade(fz), x1, x2)
					* (1.0 / amplitude);
			}
		}
	} else {
		int32_t last_perm_y = -1;
		double lerp_ax = 0.0, lerp_bx = 0.0;
		double lerp_ay = 0.0, lerp_by = 0.0;

		for(size_t x = 0; x < size_x; x++) {
			double fx = (offset_x + x) * scale_x + np->x_coord;
			int32_t ix = fx;
			if(fx < ix)
				ix--;
			int32_t px = ix & 255;
			fx -= ix;
			double u = math_java_fade(fx);

			for(size_t z = 0; z < size_z; z++) {
				double fz = (offset_z + z) * scale_z + np->z_coord;
				int32_t iz = fz;
				if(fz < iz)
					iz--;
				int32_t pz = iz & 255;
				fz -= iz;
				double w = math_java_fade(fz);

				for(size_t y = 0; y < size_y; y++) {
					double fy = (offset_y + y) * scale_y + np->y_coord;
					int32_t iy = fy;
					if(fy < iy)
						iy--;
					int32_t py = iy & 255;
					fy -= iy;
					double v = math_java_fade(fy);

					if(y == 0 || py != last_perm_y) {
						last_perm_y = py;

						int32_t A = np->permutations[px] + py;
						int32_t AA = np->permutations[A] + pz;
						int32_t AB = np->permutations[A + 1] + pz;
						int32_t B = np->permutations[px + 1] + py;
						int32_t BA = np->permutations[B] + pz;
						int32_t BB = np->permutations[B + 1] + pz;

						lerp_ax = math_java_lerp(
							u, math_java_grad(np->permutations[AA], fx, fy, fz),
							math_java_grad(np->permutations[BA], fx - 1.0, fy,
										   fz));

						lerp_bx = math_java_lerp(
							u,
							math_java_grad(np->permutations[AB], fx, fy - 1.0,
										   fz),
							math_java_grad(np->permutations[BB], fx - 1.0,
										   fy - 1.0, fz));

						lerp_ay = math_java_lerp(
							u,
							math_java_grad(np->permutations[AA + 1], fx, fy,
										   fz - 1.0),
							math_java_grad(np->permutations[BA + 1], fx - 1.0,
										   fy, fz - 1.0));

						lerp_by = math_java_lerp(
							u,
							math_java_grad(np->permutations[AB + 1], fx,
										   fy - 1.0, fz - 1.0),
							math_java_grad(np->permutations[BB + 1], fx - 1.0,
										   fy - 1.0, fz - 1.0));
					}

					double i1 = math_java_lerp(v, lerp_ax, lerp_bx);
					double i2 = math_java_lerp(v, lerp_ay, lerp_by);
					noise_field[idx++]
						+= math_java_lerp(w, i1, i2) * (1.0 / amplitude);
				}
			}
		}
	}
}

void noise_perlin_octaves_create(struct noise_octaves* np,
								 struct random_java* rand, size_t octaves) {
	assert(np && rand);

	np->octaves = octaves;
	np->base = malloc(octaves * sizeof(struct noise));
	assert(np->base);

	for(size_t k = 0; k < octaves; k++)
		noise_perlin_create(np->base + k, rand);
}

void noise_perlin_octaves_destroy(struct noise_octaves* np) {
	assert(np);
	free(np->base);
}

double noise_perlin_octaves_sample3d(struct noise_octaves* np, double x_offset,
									 double y_offset, double z_offset) {
	assert(np);

	double res = 0.0;
	double s = 1.0;

	for(size_t i = 0; i < np->octaves; i++, s /= 2.0)
		res += noise_perlin_sample(np->base + i, x_offset * s, y_offset * s,
								   z_offset * s)
			/ s;

	return res;
}

double noise_perlin_octaves_sample2d(struct noise_octaves* np, double x_offset,
									 double y_offset) {
	return noise_perlin_octaves_sample3d(np, x_offset, y_offset, 0);
}

void noise_perlin_octaves_sample_field3d(struct noise_octaves* np,
										 double* noise_field, double offset_x,
										 double offset_y, double offset_z,
										 size_t size_x, size_t size_y,
										 size_t size_z, double scale_x,
										 double scale_y, double scale_z) {
	assert(np && noise_field);

	for(size_t i = 0; i < size_x * size_y * size_z; i++)
		noise_field[i] = 0;

	double s = 1.0;

	for(size_t i = 0; i < np->octaves; i++, s /= 2.0)
		noise_perlin_sample_field(np->base + i, noise_field, offset_x, offset_y,
								  offset_z, size_x, size_y, size_z, scale_x * s,
								  scale_y * s, scale_z * s, s);
}

void noise_perlin_octaves_sample_field2d(struct noise_octaves* np,
										 double* noise_field, double offset_x,
										 double offset_z, size_t size_x,
										 size_t size_z, double scale_x,
										 double scale_z) {
	noise_perlin_octaves_sample_field3d(np, noise_field, offset_x, 10.0,
										offset_z, size_x, 1, size_z, scale_x,
										1.0, scale_z);
}

static int32_t wrap(double grad) {
	return grad > 0.0 ? grad : ((int32_t)grad - 1);
}

static double dotProd(const double grad[2], double x, double y) {
	return grad[0] * x + grad[1] * y;
}

void noise_simplex_sample_field(struct noise* np, double* noise_field,
								double offset_x, double offset_y, size_t size_x,
								size_t size_y, double scale_x, double scale_y,
								double amplitude) {
	assert(np && noise_field);
	size_t idx = 0;

	const double skewing = 0.5 * (sqrt(3.0) - 1.0);
	const double unskewing = (3.0 - sqrt(3.0)) / 6.0;
	const double gradients[12][2] = {
		{1, 1}, {-1, 1}, {1, -1}, {-1, -1}, {1, 0}, {-1, 0},
		{1, 0}, {-1, 0}, {0, 1},  {0, -1},	{0, 1}, {0, -1},
	};

	for(size_t x = 0; x < size_x; x++) {
		double x_pos = (offset_x + (double)x) * scale_x + np->x_coord;

		for(size_t y = 0; y < size_y; y++) {
			double y_pos = (offset_y + (double)y) * scale_y + np->y_coord;
			double skew = (x_pos + y_pos) * skewing;
			int32_t x0 = wrap(x_pos + skew);
			int32_t y0 = wrap(y_pos + skew);
			double unskewed = (double)(x0 + y0) * unskewing;
			double x0a = (double)x0 - unskewed;
			double y0a = (double)y0 - unskewed;
			double x0b = x_pos - x0a;
			double y0b = y_pos - y0a;
			size_t i = x0b > y0b ? 1 : 0;
			size_t j = 1 - i;

			double x0c = x0b - (double)i + unskewing;
			double y0c = y0b - (double)j + unskewing;
			double x1c = x0b - 1.0 + 2.0 * unskewing;
			double y1c = y0b - 1.0 + 2.0 * unskewing;
			int32_t x_int = x0 & 255;
			int32_t y_int = y0 & 255;
			int32_t grad0
				= np->permutations[x_int + np->permutations[y_int]] % 12;
			int32_t grad1
				= np->permutations[x_int + i + np->permutations[y_int + j]]
				% 12;
			int32_t grad2
				= np->permutations[x_int + 1 + np->permutations[y_int + 1]]
				% 12;

			double term0 = 0.5 - x0b * x0b - y0b * y0b;
			double contrib0;
			if(term0 < 0.0) {
				contrib0 = 0.0;
			} else {
				term0 *= term0;
				contrib0 = term0 * term0 * dotProd(gradients[grad0], x0b, y0b);
			}

			double term1 = 0.5 - x0c * x0c - y0c * y0c;
			double contrib1;
			if(term1 < 0.0) {
				contrib1 = 0.0;
			} else {
				term1 *= term1;
				contrib1 = term1 * term1 * dotProd(gradients[grad1], x0c, y0c);
			}

			double term2 = 0.5 - x1c * x1c - y1c * y1c;
			double contrib2;
			if(term2 < 0.0) {
				contrib2 = 0.0;
			} else {
				term2 *= term2;
				contrib2 = term2 * term2 * dotProd(gradients[grad2], x1c, y1c);
			}

			noise_field[idx++]
				+= 70.0 * (contrib0 + contrib1 + contrib2) * amplitude;
		}
	}
}

void noise_simplex_octaves_sample_field2d(struct noise_octaves* np,
										  double* noise_field, double offset_x,
										  double offset_z, size_t size_x,
										  size_t size_z, double scale_x,
										  double scale_z, double scale) {
	assert(np && noise_field);

	scale_x /= 1.5;
	scale_z /= 1.5;

	for(size_t i = 0; i < size_x * size_z; i++)
		noise_field[i] = 0;

	double s = 1.0, t = 1.0;

	for(size_t i = 0; i < np->octaves; i++, s *= scale, t *= 0.5)
		noise_simplex_sample_field(np->base + i, noise_field, offset_x,
								   offset_z, size_x, size_z, scale_x * s,
								   scale_z * s, 0.55 / t);
}