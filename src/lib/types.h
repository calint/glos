#pragma once
#include<inttypes.h>
#include"consts.h"
typedef uint8_t bool;
#define true 1
#define false 0
typedef uint8_t bits;
typedef uint32_t gid;
typedef void*ref;
typedef struct type{char path[8];}type;
#define type_def (type){{0,0,0,0,0,0,0,0}}
typedef struct vec2{float x,y;}vec2;
typedef struct vec3{float x,y,z;}vec3;
typedef struct vec4{float x,y,z,w;}vec4;
#define vec4_def (vec4){0,0,0,0}
typedef float mat4[4*4];
typedef float mat3[3*3];
typedef vec4 position;
typedef position point;
typedef vec4 velocity;
typedef vec4 angle;
typedef vec4 scale;
typedef vec4 angular_velocity;
typedef float bounding_radius;
typedef unsigned id;
typedef float dt;
typedef const char*path;

typedef struct framectx{
	dt dt;
	unsigned tick;
}framectx;
