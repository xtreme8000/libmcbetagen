#include <assert.h>

#include <generator/generator_biome.h>

void generator_biome_create(struct generator_biome* g, int64_t seed) {
	assert(g);

	struct random_java r_temp, r_humi, r_weird;
	random_java_create(&r_temp, seed * 9871);
	random_java_create(&r_humi, seed * 39811);
	random_java_create(&r_weird, seed * 543321);

	noise_perlin_octaves_create(&g->noise_temperature, &r_temp, 4);
	noise_perlin_octaves_create(&g->noise_humidity, &r_humi, 4);
	noise_perlin_octaves_create(&g->noise_weirdness, &r_weird, 2);
}

enum biome generator_biome_lookup(double temperature, double humidity) {
	// simulate lookup table
	temperature = (float)(int32_t)(temperature * 63.0) / 63.0F;
	humidity = (float)(int32_t)(humidity * 63.0) / 63.0F;

	humidity *= temperature;

	if(temperature < 0.1F)
		return BIOME_TUNDRA;

	if(humidity < 0.2F) {
		if(temperature < 0.5F)
			return BIOME_TUNDRA;
		if(temperature < 0.95F)
			return BIOME_SAVANNA;
		return BIOME_DESERT;
	}

	if(humidity > 0.5F && temperature < 0.7F)
		return BIOME_SWAMPLAND;

	if(temperature < 0.5F)
		return BIOME_TAIGA;

	if(temperature < 0.97F) {
		if(humidity < 0.35F)
			return BIOME_SHRUBLAND;
		return BIOME_FOREST;
	}
	if(humidity < 0.45F)
		return BIOME_PLAINS;

	if(humidity < 0.9F)
		return BIOME_SEASONALFOREST;

	return BIOME_RAINFOREST;
}

static double clip(double x) {
	if(x < 0)
		return 0;
	if(x > 1)
		return 1;
	return x;
}

void generator_biome_sample_field(struct generator_biome* g, int32_t offset_x,
								  int32_t offset_z, size_t max_xz) {
	assert(g);

	noise_simplex_octaves_sample_field2d(&g->noise_temperature, g->temperature,
										 offset_x, offset_z, max_xz, max_xz,
										 0.025F, 0.025F, 0.25);
	noise_simplex_octaves_sample_field2d(&g->noise_humidity, g->humidity,
										 offset_x, offset_z, max_xz, max_xz,
										 0.05F, 0.05F, 1.0 / 3.0);
	noise_simplex_octaves_sample_field2d(&g->noise_weirdness, g->weirdness,
										 offset_x, offset_z, max_xz, max_xz,
										 0.25, 0.25, 0.5882352941176471);

	for(size_t idx = 0; idx < max_xz * max_xz; idx++) {
		double weird = g->weirdness[idx] * 1.1 + 0.5;
		double humi = clip((g->humidity[idx] * 0.15 + 0.5) * (1.0 - 0.002)
						   + weird * 0.002);
		double temp
			= (g->temperature[idx] * 0.15 + 0.7) * (1.0 - 0.01) + weird * 0.01;
		temp = clip(1.0 - (1.0 - temp) * (1.0 - temp));

		g->temperature[idx] = temp;
		g->humidity[idx] = humi;
		g->biome[idx] = generator_biome_lookup(temp, humi);
	}
}
