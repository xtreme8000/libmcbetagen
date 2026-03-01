cmake_minimum_required(VERSION 3.2)
project(mcbetagen LANGUAGES C)

add_library(mcbetagen STATIC
    src/generator/generator_beta.c
    src/generator/generator_biome.c
    src/generator/generator_caves.c

    src/utils/math_helper.c
    src/utils/math_java.c
    src/utils/random_java.c

    src/noise/noise_perlin.c
)

target_compile_features(mcbetagen PUBLIC c_std_99)
target_compile_options(mcbetagen PRIVATE -Wall -Wextra -pedantic)
target_include_directories(mcbetagen PUBLIC src/)

add_executable(test EXCLUDE_FROM_ALL
    test/munit.c
    test/main.c
)
target_include_directories(test PRIVATE test/)
target_link_libraries(test PRIVATE mcbetagen m)