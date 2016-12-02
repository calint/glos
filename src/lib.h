#pragma once
#include <stdlib.h>

//------------------------------------------------------------------- typedefs

typedef uint8_t bits;

typedef int8_t byte;

typedef int16_t word;

typedef int32_t dword;

typedef int64_t quad;

typedef struct baba{
	bits bits;
	byte byte;
	word word;
	dword dword;
	quad quad;
	char string[32];
} baba;


typedef struct class {
	char path[8];
} class;

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

typedef float dt_in_seconds;

typedef dt_in_seconds time_in_seconds;

//-------------------------------------------------------------------- library
//
//inline static int bits_is_set(bits*b, int bit_number_starting_at_zero) {
//	bits bb = (bits) (1 << bit_number_starting_at_zero);
//	return (*b & bb) != 0;
//}
//
////----------------------------------------------------------------------------
//
//inline static void bits_set(bits*b, int bit_number_starting_at_zero) {
//	bits bb = (bits) (1 << bit_number_starting_at_zero);
//	*b |= bb;
//}
//
////----------------------------------------------------------------------------
//
//inline static void bits_unset(bits*b, int bit_number_starting_at_zero) {
//	bits bb = (bits) (1 << bit_number_starting_at_zero);
//	*b &= (bits) ~bb;
//}
//
//----------------------------------------------------------------------------

inline static float random(){return(float)rand()/(float)RAND_MAX;}

//----------------------------------------------------------------------------

inline static double random_range(float low,float high){
	return random()*(high-low)+low;
}

//----------------------------------------------------------------------------

static inline float bounding_radius_for_scale(scale*s){
	return sqrtf(s->x*s->x+s->y*s->y+s->z*s->z);
}

//----------------------------------------------------------------------------

