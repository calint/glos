#pragma once
//------------------------------------------------------------------- typedefs
typedef uint8_t bool;
typedef uint8_t bits;
typedef uint32_t gid;
typedef void*ref;
typedef struct type{char path[8];}type;
typedef struct vec4{float x,y,z,q;}vec4;
typedef vec4 position;
typedef vec4 velocity;
typedef vec4 angle;
typedef vec4 scale;
typedef vec4 angular_velocity;
typedef float bounding_radius;
typedef float*mat4;
typedef unsigned id;
typedef float dt;
typedef float time_in_seconds;
typedef const char*path;
typedef uint32_t arrayix;
#define PI 3.1415926535897932384626433832795f
//#include"dynp.h"
//#include"dynf.h"
//#include"dyni.h"
//#include"dync.h"
//typedef dync str;
