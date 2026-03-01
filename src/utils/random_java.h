#ifndef RANDOM_JAVA_H
#define RANDOM_JAVA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief A faithful reimplementation of the Java pseudorandom number generator
 */
struct random_java {
	int64_t seed;
};

/**
 * @brief Construct a new Java Random object
 *
 * @param initialSeed The initial seed value (defaults to current time)
 */
void random_java_create(struct random_java* rand, int64_t seed);
/**
 * @brief Set the Seed value that'll be used for all subsequently generated
 * values
 *
 * @param s Seed value
 */
void random_java_set_seed(struct random_java* rand, int64_t s);

/**
 * @brief Returns the next int (32-bit integer)
 *
 * @return Pseudorandom 32-bit integer value
 */
int32_t random_java_next_int(struct random_java* rand);
/**
 * @brief Returns the next bound int (32-bit integer)
 *
 * @return Pseudorandom 32-bit integer value
 */
int32_t random_java_next_int_bound(struct random_java* rand, int32_t bound);

/**
 * @brief Returns the next long (64-bit integer)
 *
 * @return Pseudorandom 64-bit long value
 */
int64_t random_java_next_long(struct random_java* rand);

/**
 * @brief Returns the next pseudorandom double
 *
 * @return Pseudorandom double value between 0.0 (inclusive) and 1.0 (exclusive)
 */
double random_java_next_double(struct random_java* rand);

/**
 * @brief Returns the next pseudorandom boolean
 *
 * @return Pseudorandom boolean
 */
bool random_java_next_bool(struct random_java* rand);

/**
 * @brief Returns the next pseudorandom float
 *
 * @return Pseudorandom float value between 0.0 (inclusive) and 1.0 (exclusive)
 */
float random_java_next_float(struct random_java* rand);

#endif