#pragma once
#include <stdlib.h>

//------------------------------------------------------------------ typedefs

typedef struct vec4 {
	float x, y, z, q;
} vec4;

typedef vec4 position;

typedef vec4 angle;

typedef vec4 scale;

typedef float bounding_radius;

typedef vec4 velocity;

typedef vec4 angular_velocity;

typedef Uint32 color;

typedef int id;

typedef Uint8 bits;

typedef int8_t byte;

typedef int16_t word;

typedef int32_t dword;

typedef int64_t quad;

typedef float dt;

//---------------------------------------------------------------- baba struct

#define baba_count 1

struct baba{
	bits bits;
	byte byte;
	word word;
	dword dword;
	quad quad;
	char string[32];
}babas[baba_count]__attribute__((align(64)));

//----------------------------------------------------------------------- type
typedef struct type {
	char path[8];
} type;

//------------------------------------------------------------------ library

inline static int bits_is_set(bits*b, int bit_number_starting_at_zero) {
	bits bb = (bits) (1 << bit_number_starting_at_zero);
	return (*b & bb) != 0;
}

//--------------------------------------------------------------------------

inline static void bits_set(bits*b, int bit_number_starting_at_zero) {
	bits bb = (bits) (1 << bit_number_starting_at_zero);
	*b |= bb;
}

//--------------------------------------------------------------------------

inline static void bits_unset(bits*b, int bit_number_starting_at_zero) {
	bits bb = (bits) (1 << bit_number_starting_at_zero);
	*b &= (bits) ~bb;
}

//--------------------------------------------------------------------------

inline static double random() {
	double r = (double) rand() / RAND_MAX;
	return r;
}

//--------------------------------------------------------------------------

inline static double random_range(double low, double high) {
	double r = random() * (high - low) + low;
	return r;
}

//------------------------------------------------------------------ library
