#ifndef MATH_HELPER_H
#define MATH_HELPER_H

#include <stdint.h>

float math_helper_sin(float x);
float math_helper_cos(float x);
float math_helper_sqrt_float(float x);
float math_helper_sqrt_double(double x);
int32_t math_helper_floor_float(float x);
int32_t math_helper_floor_double(double x);
float math_helper_abs(float x);
double math_helper_abs_max(double a, double b);
void math_helper_init(void);

#endif