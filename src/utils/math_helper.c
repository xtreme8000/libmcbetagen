#include <math.h>

#include <stdbool.h>

#include <utils/math_helper.h>
#include <utils/math_java.h>

#define SIN_TABLE_SIZE 65536
static float SIN_TABLE[SIN_TABLE_SIZE];
static bool init_finished = false;

float math_helper_sin(float x) {
	return SIN_TABLE[(int32_t)(x * 10430.378f) & 0xFFFF];
}

float math_helper_cos(float x) {
	return SIN_TABLE[(int32_t)(x * 10430.378f + 16384.0f) & 0xFFFF];
}

float math_helper_sqrt_float(float x) {
	return sqrt(x);
}

float math_helper_sqrt_double(double x) {
	return sqrt(x);
}

int32_t math_helper_floor_float(float x) {
	int32_t i = x;
	return x < (float)i ? i - 1 : i;
}

int32_t math_helper_floor_double(double x) {
	int32_t i = x;
	return x < (double)i ? i - 1 : i;
}

float math_helper_abs(float x) {
	return x >= 0.0f ? x : -x;
}

double math_helper_abs_max(double a, double b) {
	if(a < 0.0)
		a = -a;
	if(b < 0.0)
		b = -b;
	return a > b ? a : b;
}

void math_helper_init() {
	if(init_finished)
		return;

	for(size_t i = 0; i < SIN_TABLE_SIZE; i++)
		SIN_TABLE[i]
			= sin((double)i * MATH_JAVA_PI * 2.0 / (double)SIN_TABLE_SIZE);

	init_finished = true;
}