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

//---------------------------------------------------------------------- lib

inline static int is_bit_set(bits*b,int bit_number_starting_at_zero){
	return(*b&(1<<bit_number_starting_at_zero))!=0;
}

inline static void set_bit(bits*b,int bit_number_starting_at_zero){
	*b|=(bits)(1<<bit_number_starting_at_zero);
}

inline static void clear_bit(bits*b,int bit_number_starting_at_zero){
	*b&=(bits)~(1<<bit_number_starting_at_zero);
}

inline static void add_dt(vec4*this,vec4*other,dt_in_seconds dt){
	this->x+=other->x*dt;
	this->y+=other->y*dt;
	this->z+=other->z*dt;
}
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

