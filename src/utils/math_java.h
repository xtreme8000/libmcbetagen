#ifndef MATH_JAVA_H
#define MATH_JAVA_H

#include <stddef.h>
#include <stdint.h>

#define MATH_JAVA_PI 3.141592653589793
#define MATH_JAVA_PI_F ((float)MATH_JAVA_PI)

// Library for emulating Java/Java Edition math functions

/**
 * @brief Linear interpolation function
 *
 * @param t Interpolation factor
 * @param a Start value (t = 0.0)
 * @param b End value (t = 1.0)
 * @return Interpolated value between a and b
 */
double math_java_lerp(double t, double a, double b);

double math_java_grad(int32_t var0, double var1, double var3, double var5);

double math_java_alt_grad(int32_t var1, double var2, double var4);

/**
 * @brief Perlin-noise easing function
 *
 * @param value Input value
 * @return Eased output value
 */
double math_java_fade(double value);

/**
 * @brief Java-equivalent hashing function
 *
 * @param value The input string
 * @return Hashed string expressed as an integer
 */
int32_t math_java_hash_code(const char* value);

int32_t math_java_abs(int32_t a);

#endif