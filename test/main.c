#include <generator/generator_biome.h>
#include <munit.h>
#include <noise/noise_perlin.h>
#include <stdio.h>
#include <utils/math_helper.h>
#include <utils/math_java.h>
#include <utils/random_java.h>

MunitResult test_next_int(const MunitParameter* params, void* user_data) {
	int32_t expected_42[] = {-1170105035, 234785527,  -1360544799, 205897768,
							 1325939940,  -248792245, 1190043011,  -1255373459};

	struct random_java r;
	random_java_create(&r, 42);

	for(size_t k = 0; k < sizeof(expected_42) / sizeof(*expected_42); k++)
		munit_assert_int32(random_java_next_int(&r), ==, expected_42[k]);

	return MUNIT_OK;
}

MunitResult test_next_int_bound(const MunitParameter* params, void* user_data) {
	int32_t expected_128[] = {87, 73, 6, 98, 80, 80, 9, 70};
	int32_t expected_29[] = {9, 6, 6, 2, 0, 24, 21, 9};
	int32_t expected_n36[] = {583513521, 213810482, 124696219, 355960711,
							  750838839, 539409745, 882629905, 263721828};

	struct random_java r;
	random_java_create(&r, 512);

	for(size_t k = 0; k < sizeof(expected_128) / sizeof(*expected_128); k++)
		munit_assert_int32(random_java_next_int_bound(&r, 128), ==,
						   expected_128[k]);

	for(size_t k = 0; k < sizeof(expected_29) / sizeof(*expected_29); k++)
		munit_assert_int32(random_java_next_int_bound(&r, 29), ==,
						   expected_29[k]);

	random_java_set_seed(&r, -36);

	for(size_t k = 0; k < sizeof(expected_n36) / sizeof(*expected_n36); k++)
		munit_assert_int32(random_java_next_int_bound(&r, 0x40000001), ==,
						   expected_n36[k]);

	return MUNIT_OK;
}

MunitResult test_next_long(const MunitParameter* params, void* user_data) {
	int64_t expected_n5[]
		= {4967725915334898755,	 6162916890256202832, 7026268870879647858,
		   5300084627461521827,	 6370092549091603725, 2111084189762571300,
		   -4745850910240262714, 4838130126375073285};

	struct random_java r;
	random_java_create(&r, -5);

	for(size_t k = 0; k < sizeof(expected_n5) / sizeof(*expected_n5); k++)
		munit_assert_int64(random_java_next_long(&r), ==, expected_n5[k]);

	return MUNIT_OK;
}

MunitResult test_next_bool(const MunitParameter* params, void* user_data) {
	bool expected_52[] = {true, false, true, true, false, true, false, true};

	struct random_java r;
	random_java_create(&r, 52);

	for(size_t k = 0; k < sizeof(expected_52) / sizeof(*expected_52); k++)
		munit_assert(random_java_next_bool(&r) == expected_52[k]);

	return MUNIT_OK;
}

MunitResult test_next_float(const MunitParameter* params, void* user_data) {
	float expected_17[] = {0.7323115F,	 0.7959577F,  0.69737047F, 0.5899265F,
						   0.082956076F, 0.40127832F, 0.81623644F, 0.9890971F};

	struct random_java r;
	random_java_create(&r, 17);

	for(size_t k = 0; k < sizeof(expected_17) / sizeof(*expected_17); k++)
		munit_assert_float(random_java_next_float(&r), ==, expected_17[k]);

	return MUNIT_OK;
}

MunitResult test_next_double(const MunitParameter* params, void* user_data) {
	double expected_1024[]
		= {0.6392364996736015, 0.8539982129215666, 0.6219770304307501,
		   0.6051964757188117, 0.6932235104956287, 0.3815752388811804,
		   0.7851886342815924, 0.9292702964795005};

	struct random_java r;
	random_java_create(&r, 1024);

	for(size_t k = 0; k < sizeof(expected_1024) / sizeof(*expected_1024); k++)
		munit_assert_double(random_java_next_double(&r), ==, expected_1024[k]);

	return MUNIT_OK;
}

MunitResult test_hash_code(const MunitParameter* params, void* user_data) {
	const char* inputs[]
		= {"blue mountain river",	  "ancient stone bridge",
		   "quiet summer evening",	  "golden autumn leaves",
		   "deep ocean current",	  "written history records",
		   "modern computer science", "international space station"};
	int32_t expected[] = {1202086719, -716054516,  1472350339, -986882289,
						  -846243805, -1912497747, -477527744, 2119973384};

	for(size_t k = 0; k < sizeof(inputs) / sizeof(*inputs); k++)
		munit_assert_int32(math_java_hash_code(inputs[k]), ==, expected[k]);

	return MUNIT_OK;
}

MunitResult test_abs(const MunitParameter* params, void* user_data) {
	munit_assert_int32(math_java_abs(0), ==, 0);
	munit_assert_int32(math_java_abs(-3), ==, 3);
	munit_assert_int32(math_java_abs(5), ==, 5);
	return MUNIT_OK;
}

MunitResult test_lerp(const MunitParameter* params, void* user_data) {
	munit_assert_double(math_java_lerp(0, -12.5, 32.33), ==, -12.5);
	munit_assert_double(math_java_lerp(1, -48.36, 15.91), ==,
						15.909999999999997);
	munit_assert_double(math_java_lerp(0.639, -3.14, 22.08), ==, 12.97558);
	return MUNIT_OK;
}

MunitResult test_fade(const MunitParameter* params, void* user_data) {
	double inputs[]
		= {-5.57889928507729,  -6.843709289473994, -4.786261482362558,
		   0.2754633708759595, 4.83887725234813,   3.365436756308938,
		   2.4144553571581113, 1.6015723551143601};
	double expected[]
		= {-48693.07896104929,	-126185.99307579601, -24039.001154401467,
		   0.13217137557269412, 8826.743046283469,	 1047.2946720295918,
		   123.30997667035915,	5.614189382153775};

	for(size_t k = 0; k < sizeof(inputs) / sizeof(*inputs); k++)
		munit_assert_double(math_java_fade(inputs[k]), ==, expected[k]);

	return MUNIT_OK;
}

MunitResult test_grad(const MunitParameter* params, void* user_data) {
	double expected[]
		= {-12.75, -11.75, 61.83, -61.83, 11.75,  -49.08, 24.5,	 -24.5, 49.08,
		   12.75,  -37.33, 37.33, -12.75, -11.75, -37.33, 61.83, -12.75};

	for(size_t k = 0; k < sizeof(expected) / sizeof(*expected); k++)
		munit_assert_double(math_java_grad(k - 1, -36.79, 25.04, -12.29), ==,
							expected[k]);

	return MUNIT_OK;
}

MunitResult test_alt_grad(const MunitParameter* params, void* user_data) {
	double expected[]
		= {-25.04, -36.79, 36.79,  -36.79, 36.79,  -11.75, 61.83, -61.83, 11.75,
		   25.04,  25.04,  -25.04, -25.04, -36.79, 25.04,  36.79, -25.04};

	for(size_t k = 0; k < sizeof(expected) / sizeof(*expected); k++)
		munit_assert_double(math_java_alt_grad(k - 1, -36.79, 25.04), ==,
							expected[k]);

	return MUNIT_OK;
}

MunitResult test_sin_cos(const MunitParameter* params, void* user_data) {
	float inputs[] = {3.5188618F, -6.144186F, -5.473076F, 7.9782944F,
					  13.166733F, 4.550112F,  13.179491F, -0.49151802F};
	float expected_sin[] = {-0.3683777F, 0.13856967F, 0.7243793F, 0.9922906F,
							0.564941F,	 -0.9868559F, 0.5754162F, -0.47190398F};
	float expected_cos[]
		= {-0.92967623F, 0.9903527F,   0.6894017F, -0.12393297F,
		   0.8251313F,	 -0.16160256F, 0.8178607F, 0.88160473F};

	math_helper_init();

	for(size_t k = 0; k < sizeof(inputs) / sizeof(*inputs); k++) {
		munit_assert_float(math_helper_sin(inputs[k]), ==, expected_sin[k]);
		munit_assert_float(math_helper_cos(inputs[k]), ==, expected_cos[k]);
	}

	return MUNIT_OK;
}

MunitResult test_sqrt(const MunitParameter* params, void* user_data) {
	double inputs[]
		= {156.15090678403544, 84.21539592484189, 233.33387619854363,
		   233.43594163757356, 26.2635501042611,  58.21577750034476,
		   53.97810463021631,  234.36587933039488};
	float expected_float[] = {12.496036F, 9.176894F,  15.27527F, 15.27861F,
							  5.1247973F, 7.6299267F, 7.346979F, 15.309012F};
	float expected_double[] = {12.496036F, 9.176895F, 15.27527F, 15.27861F,
							   5.1247973F, 7.629926F, 7.346979F, 15.309013F};

	math_helper_init();

	for(size_t k = 0; k < sizeof(inputs) / sizeof(*inputs); k++) {
		munit_assert_float(math_helper_sqrt_float(inputs[k]), ==,
						   expected_float[k]);
		munit_assert_float(math_helper_sqrt_double(inputs[k]), ==,
						   expected_double[k]);
	}

	return MUNIT_OK;
}

MunitResult test_abs_max(const MunitParameter* params, void* user_data) {
	munit_assert_double(math_helper_abs_max(0, 0), ==, 0);
	munit_assert_double(math_helper_abs_max(12.3, -4.5), ==, 12.3);
	munit_assert_double(math_helper_abs_max(-81.1, -16.9), ==, 81.1);
	return MUNIT_OK;
}

MunitResult test_abs_float(const MunitParameter* params, void* user_data) {
	munit_assert_float(math_helper_abs(0.0F), ==, 0);
	munit_assert_float(math_helper_abs(-3.14F), ==, 3.14F);
	munit_assert_float(math_helper_abs(5.72F), ==, 5.72F);
	return MUNIT_OK;
}

MunitResult test_floor(const MunitParameter* params, void* user_data) {
	munit_assert_int32(math_helper_floor_float(0.0F), ==, 0);
	munit_assert_int32(math_helper_floor_float(-3.14F), ==, -4);
	munit_assert_int32(math_helper_floor_float(5.72F), ==, 5);

	munit_assert_int32(math_helper_floor_double(0), ==, 0);
	munit_assert_int32(math_helper_floor_double(-3.14), ==, -4);
	munit_assert_float(math_helper_floor_double(5.72), ==, 5);
	return MUNIT_OK;
}

MunitResult test_perlin_sample(const MunitParameter* params, void* user_data) {
	struct noise np;
	struct random_java rand;
	random_java_create(&rand, 42);
	noise_perlin_create(&np, &rand);

	munit_assert_double(noise_perlin_sample(&np, 4.77, -10.21, 29.68), ==,
						-0.122099781292802);
	munit_assert_double(noise_perlin_sample(&np, -0.95, 20.72, -12.25), ==,
						-0.07272774691924322);
	munit_assert_double(noise_perlin_sample(&np, 25.07, 19.50, 0.79), ==,
						-0.3656928225137849);

	return MUNIT_OK;
}

MunitResult test_perlin_octaves3d(const MunitParameter* params,
								  void* user_data) {
	struct noise_octaves np;
	struct random_java rand;
	random_java_create(&rand, 42);
	noise_perlin_octaves_create(&np, &rand, 9);

	munit_assert_double(noise_perlin_octaves_sample3d(&np, 4.77, -10.21, 29.68),
						==, -0.16904090970375663);
	munit_assert_double(
		noise_perlin_octaves_sample3d(&np, -0.95, 20.72, -12.25), ==,
		47.12811470662307);
	munit_assert_double(noise_perlin_octaves_sample3d(&np, 25.07, 19.50, 0.79),
						==, 65.60153690199598);

	noise_perlin_octaves_destroy(&np);

	return MUNIT_OK;
}

MunitResult test_perlin_octaves2d(const MunitParameter* params,
								  void* user_data) {
	struct noise_octaves np;
	struct random_java rand;
	random_java_create(&rand, 314);
	noise_perlin_octaves_create(&np, &rand, 5);

	munit_assert_double(noise_perlin_octaves_sample2d(&np, -10.21, 4.77), ==,
						-2.4749163665400653);
	munit_assert_double(noise_perlin_octaves_sample2d(&np, 20.72, -0.95), ==,
						2.314324676555217);
	munit_assert_double(noise_perlin_octaves_sample2d(&np, 19.50, 25.07), ==,
						0.4762119605338664);

	noise_perlin_octaves_destroy(&np);

	return MUNIT_OK;
}

MunitResult test_perlin_field_octaves2d(const MunitParameter* params,
										void* user_data) {
	struct {
		size_t idx;
		double value;
	} expected[] = {
		{0, 21.55162341520365},	   {25, 16.09267471054794},
		{41, 14.165460509057674},  {76, 13.657213641751216},
		{114, 15.133493266279025}, {133, 13.29278281413545},
		{194, 15.796289047523643}, {245, 19.136020436553636},
	};

	struct noise_octaves np;
	struct random_java rand;
	double field[16 * 16];
	random_java_create(&rand, 42);
	noise_perlin_octaves_create(&np, &rand, 8);
	noise_perlin_octaves_sample_field2d(&np, field, 67, -8, 16, 16, 0.6, 0.9);

	for(size_t k = 0; k < sizeof(expected) / sizeof(*expected); k++)
		munit_assert_double(field[expected[k].idx], ==, expected[k].value);

	noise_perlin_octaves_destroy(&np);

	return MUNIT_OK;
}

MunitResult test_perlin_field_octaves3d(const MunitParameter* params,
										void* user_data) {
	struct {
		size_t idx;
		double value;
	} expected[] = {
		{41, 2.9216798051879493},  {66, 4.7209988221080526},
		{138, 0.9696770299732},	   {142, 7.711248710410965},
		{173, 0.7007935222658969}, {211, 1.9829575223735145},
		{252, 1.6115537377273759}, {311, 3.06531804022195},
	};

	struct noise_octaves np;
	struct random_java rand;
	double field[4 * 13 * 8];
	random_java_create(&rand, 42);
	noise_perlin_octaves_create(&np, &rand, 5);
	noise_perlin_octaves_sample_field3d(&np, field, 67.2, 12.7, -8.1, 4, 13, 8,
										0.6, 1.1, 0.9);

	for(size_t k = 0; k < sizeof(expected) / sizeof(*expected); k++)
		munit_assert_double(field[expected[k].idx], ==, expected[k].value);

	noise_perlin_octaves_destroy(&np);

	return MUNIT_OK;
}

MunitResult test_biome_lookup(const MunitParameter* params, void* user_data) {
	struct {
		float temp, humi;
		enum biome biome;
	} expected[] = {
		{0.0, 0.0, BIOME_TUNDRA},	 {0.4, 0.1, BIOME_TUNDRA},
		{0.9, 0.1, BIOME_SAVANNA},	 {1.0, 0.1, BIOME_DESERT},
		{0.5, 0.6, BIOME_TAIGA},	 {0.6, 0.6, BIOME_SHRUBLAND},
		{1.0, 0.4, BIOME_PLAINS},	 {1.0, 0.7, BIOME_SEASONALFOREST},
		{0.6, 0.7, BIOME_FOREST},	 {1.0, 1.0, BIOME_RAINFOREST},
		{0.6, 1.0, BIOME_SWAMPLAND},
	};

	for(size_t k = 0; k < sizeof(expected) / sizeof(*expected); k++)
		munit_assert_size(
			generator_biome_lookup(expected[k].temp, expected[k].humi), ==,
			expected[k].biome);

	return MUNIT_OK;
}

MunitResult test_biome_sample_field(const MunitParameter* params,
									void* user_data) {
	struct generator_biome g;
	generator_biome_create(&g, math_java_hash_code("Glacier"));
	generator_biome_sample_field(&g, (22 - 16) * 16, (8 - 16) * 16, 16);

	struct {
		size_t idx;
		double temp, humi;
	} expected[] = {
		{0, 0.9648219492035848, 0.6944917819460102},
		{26, 0.9641136291421338, 0.6229047999439226},
		{67, 0.9681150653776864, 0.719206714541733},
		{125, 0.9730375998321794, 0.6429554050700489},
		{203, 0.9807855941392649, 0.6941915528577824},
		{226, 0.9788729877346822, 0.810845118525562},
		{248, 0.9828423323726483, 0.7543704303621335},
		{255, 0.9816176847955945, 0.607876097330199},
	};

	for(size_t k = 0; k < sizeof(expected) / sizeof(*expected); k++) {
		munit_assert_size(g.biome[expected[k].idx], ==, BIOME_FOREST);
		munit_assert_double(g.temperature[expected[k].idx], ==,
							expected[k].temp);
		munit_assert_double(g.humidity[expected[k].idx], ==, expected[k].humi);
	}

	return MUNIT_OK;
}

const MunitSuite suite_tests = {
	NULL,
	NULL,
	(MunitSuite[]) {
		{
			"random_java/",
			(MunitTest[]) {
				{"next_int", test_next_int, NULL, NULL, MUNIT_TEST_OPTION_NONE,
				 NULL},
				{"next_int_bound", test_next_int_bound, NULL, NULL,
				 MUNIT_TEST_OPTION_NONE, NULL},
				{"next_long", test_next_long, NULL, NULL,
				 MUNIT_TEST_OPTION_NONE, NULL},
				{"next_bool", test_next_bool, NULL, NULL,
				 MUNIT_TEST_OPTION_NONE, NULL},
				{"next_float", test_next_float, NULL, NULL,
				 MUNIT_TEST_OPTION_NONE, NULL},
				{"next_double", test_next_double, NULL, NULL,
				 MUNIT_TEST_OPTION_NONE, NULL},
				{NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
			},
			NULL,
			1,
			MUNIT_SUITE_OPTION_NONE,
		},
		{
			"math_java/",
			(MunitTest[]) {
				{"hash_code", test_hash_code, NULL, NULL,
				 MUNIT_TEST_OPTION_NONE, NULL},
				{"abs", test_abs, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
				{"lerp", test_lerp, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
				{"fade", test_fade, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
				{"grad", test_grad, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
				{"alt_grad", test_alt_grad, NULL, NULL, MUNIT_TEST_OPTION_NONE,
				 NULL},
				{NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
			},
			NULL,
			1,
			MUNIT_SUITE_OPTION_NONE,
		},
		{
			"math_helper/",
			(MunitTest[]) {
				{"sin_cos", test_sin_cos, NULL, NULL, MUNIT_TEST_OPTION_NONE,
				 NULL},
				{"sqrt", test_sqrt, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
				{"abs_max", test_abs_max, NULL, NULL, MUNIT_TEST_OPTION_NONE,
				 NULL},
				{"abs_float", test_abs_float, NULL, NULL,
				 MUNIT_TEST_OPTION_NONE, NULL},
				{"floor", test_floor, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
				{NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
			},
			NULL,
			1,
			MUNIT_SUITE_OPTION_NONE,
		},
		{
			"noise/",
			(MunitTest[]) {
				{"sample", test_perlin_sample, NULL, NULL,
				 MUNIT_TEST_OPTION_NONE, NULL},
				{"octaves3d", test_perlin_octaves3d, NULL, NULL,
				 MUNIT_TEST_OPTION_NONE, NULL},
				{"octaves2d", test_perlin_octaves2d, NULL, NULL,
				 MUNIT_TEST_OPTION_NONE, NULL},
				{"field_octaves2d", test_perlin_field_octaves2d, NULL, NULL,
				 MUNIT_TEST_OPTION_NONE, NULL},
				{"field_octaves3d", test_perlin_field_octaves3d, NULL, NULL,
				 MUNIT_TEST_OPTION_NONE, NULL},
				{NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
			},
			NULL,
			1,
			MUNIT_SUITE_OPTION_NONE,
		},
		{
			"biome/",
			(MunitTest[]) {
				{"lookup", test_biome_lookup, NULL, NULL,
				 MUNIT_TEST_OPTION_NONE, NULL},
				{"sample_field", test_biome_sample_field, NULL, NULL,
				 MUNIT_TEST_OPTION_NONE, NULL},
				{NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
			},
			NULL,
			1,
			MUNIT_SUITE_OPTION_NONE,
		},
		{NULL, NULL, NULL, 0, MUNIT_SUITE_OPTION_NONE},
	},
	1,
	MUNIT_SUITE_OPTION_NONE,
};

int main(int argc, char** argv) {
	return munit_suite_main(&suite_tests, NULL, argc, argv);
}
