#pragma once
#include<stdint.h>
//------------------------------------------------------------------- typedefs
typedef uint8_t bool;
typedef uint8_t bits;
typedef uint32_t gid;
typedef void*ref;
typedef struct type{char path[8];}type;

typedef struct vec2{float x,y;}vec2;
typedef struct vec3{float x,y,z;}vec3;
typedef struct vec4{float x,y,z,w;}vec4;
#define vec4_def (vec4){0,0,0,0}
//static vec4 vec4_def=(vec4){0,0,0,0};

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
#define PI 3.1415926535897932384626433832795f
//#include"dynp.h"
//#include"dynf.h"
//#include"dyni.h"
//#include"dync.h"
//typedef dync str;
typedef float mat;
typedef float*mat4f;
