#include <assert.h>

#include <utils/random_java.h>

#define RANDOM_JAVA_MULT 0x5DEECE66D
#define RANDOM_JAVA_ADDEND 0xB
#define RANDOM_JAVA_MASK ((1LL << 48) - 1)

/**
 * @brief Performs a new iteration of the PRNG
 *
 * @return Pseudorandom 32-bit integer value
 */
static int32_t random_java_next(struct random_java* rand, size_t bits) {
	assert(rand);
	rand->seed = (rand->seed * RANDOM_JAVA_MULT + RANDOM_JAVA_ADDEND)
		& RANDOM_JAVA_MASK;
	return rand->seed >> (48 - bits);
}

void random_java_create(struct random_java* rand, int64_t seed) {
	assert(rand);
	random_java_set_seed(rand, seed);
}

void random_java_set_seed(struct random_java* rand, int64_t s) {
	assert(rand);
	rand->seed = (s ^ RANDOM_JAVA_MULT) & RANDOM_JAVA_MASK;
}

int32_t random_java_next_int(struct random_java* rand) {
	assert(rand);
	return random_java_next(rand, 32);
}

int32_t random_java_next_int_bound(struct random_java* rand, int32_t bound) {
	assert(rand && bound > 0);

	if((bound & -bound) == bound) // power of two
		return (int32_t)((bound * (int64_t)random_java_next(rand, 31)) >> 31);

	int32_t bits, val;
	do {
		bits = random_java_next(rand, 31);
		val = bits % bound;
	} while(bits - val + (bound - 1) < 0);
	return val;
}

int64_t random_java_next_long(struct random_java* rand) {
	assert(rand);
	return ((int64_t)random_java_next(rand, 32) << 32)
		+ random_java_next(rand, 32);
}

double random_java_next_double(struct random_java* rand) {
	assert(rand);
	return (double)(((int64_t)random_java_next(rand, 26) << 27)
					+ random_java_next(rand, 27))
		/ (double)(1LL << 53);
}

bool random_java_next_bool(struct random_java* rand) {
	assert(rand);
	return random_java_next(rand, 1) != 0;
}

float random_java_next_float(struct random_java* rand) {
	assert(rand);
	return (float)random_java_next(rand, 24) / (float)(1 << 24);
}