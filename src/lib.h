#pragma once
#include<execinfo.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<unistd.h>
#include<inttypes.h>
#include"lib/consts.h"

typedef uint8_t bool;
typedef uint8_t bits;
typedef uint32_t gid;
typedef void*ref;
typedef struct type{char path[8];}type;
#define type_def (type){{0,0,0,0,0,0,0,0}}

typedef struct vec2{float x,y;}vec2;
typedef struct vec3{float x,y,z;}vec3;
typedef struct vec4{float x,y,z,w;}vec4;
#define vec4_def (vec4){0,0,0,0}

typedef float mat4[16];

typedef vec4 position;
typedef position point;
typedef vec4 velocity;
typedef vec4 angle;
typedef vec4 scale;
typedef vec4 angular_velocity;
typedef float bounding_radius;
//typedef float*mat4;
typedef unsigned id;
typedef float dt;
typedef float time_in_seconds;
typedef const char*path;
typedef uint32_t indx;
typedef unsigned glid;
typedef float mat;
typedef float*mat4f;

#include"lib/str.h"
#include"lib/dynf.h"
#include"lib/dyni.h"
#include"lib/dynp.h"
#include"lib/vec3.h"
#include"lib/mat4.h"
#include"lib/token.h"


inline static int bits_is_bit_set(bits*b,int bit_number_starting_at_zero){
	return(*b&(1<<bit_number_starting_at_zero))!=0;
}

//----------------------------------------------------------------------------

inline static void bits_set_bit(bits*b,int bit_number_starting_at_zero){
	*b|=(bits)(1<<bit_number_starting_at_zero);
}

//----------------------------------------------------------------------------

inline static void bits_clear(bits*b,int bit_number_starting_at_zero){
	*b&=(bits)~(1<<bit_number_starting_at_zero);
}

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

inline static float rnd(){return(float)rand()/(float)RAND_MAX;}

//----------------------------------------------------------------------------

inline static float random_range(float low,float high){
	return (float)rnd()*(high-low)+low;
}

//----------------------------------------------------------------------------

inline static float bounding_radius_for_scale(scale*s){
	return sqrtf(s->x*s->x+s->y*s->y+s->z*s->z);//?
}

//----------------------------------------------------------------------------

void stacktrace_print(){
	void*array[10];
	int size=backtrace(array,10);
	char**strings=backtrace_symbols(array,size);
	fprintf(stderr,"stacktrace %d frames:\n",size);

	for(int i=0;i<size;i++)
		fprintf(stderr,"%s\n",strings[i]);

	free (strings);
}
