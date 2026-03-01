A library to generate almost perfect Beta 1.7.3 terrain, ported from [BetrockServer](https://github.com/OfficialPixelBrush/BetrockServer) to C99.

Example usage:

```c
#include <generator/generator_beta.h>
#include <utils/math_helper.h>
#include <utils/math_java.h>

math_helper_init();

struct generator g;
generator_create(&g, math_java_hash_code("Glacier"));

uint8_t get_block(struct generator_chunk* c, int32_t x, int32_t y, int32_t z);
void set_block(struct generator_chunk* c, int32_t x, int32_t y, int32_t z, uint8_t type);

struct generator_chunk c = {
    .x = 0,
    .z = 0,
    .set_block = set_block,
    .get_block = get_block,
};

generator_sample(&g, &c);
generator_destroy(&g);
```