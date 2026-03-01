#include <utils/math_java.h>

double math_java_lerp(double t, double a, double b) {
	return a + t * (b - a);
}

double math_java_grad(int32_t var0, double var1, double var3, double var5) {
	var0 &= 15;
	double var8 = var0 < 8 ? var1 : var3;
	double var10 = var0 < 4 ? var3 : (var0 != 12 && var0 != 14 ? var5 : var1);
	return ((var0 & 1) == 0 ? var8 : -var8)
		+ ((var0 & 2) == 0 ? var10 : -var10);
}

double math_java_alt_grad(int32_t var1, double var2, double var4) {
	int32_t var6 = var1 & 15;
	double var7 = (double)(1 - ((var6 & 8) >> 3)) * var2;
	double var9 = var6 < 4 ? 0.0 : (var6 != 12 && var6 != 14 ? var4 : var2);
	return ((var6 & 1) == 0 ? var7 : -var7) + ((var6 & 2) == 0 ? var9 : -var9);
}

double math_java_fade(double value) {
	return value * value * value * (value * (value * 6.0 - 15.0) + 10.0);
}

int32_t math_java_hash_code(const char* value) {
	int32_t h = 0;

	while(*value)
		h = 31 * h + *(value++);

	return h;
}

int32_t math_java_abs(int32_t a) {
	return (a < 0) ? -a : a;
}