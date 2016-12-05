#pragma once
//------------------------------------------------------------------- typedefs
#include"dynp.h"
#include"dynf.h"
#include"dyni.h"
#include"dync.h"
typedef dync str;
typedef GLuint gid;
typedef void*ref;
typedef struct class{char path[8];}type;
typedef struct vec4{float x,y,z,q;}vec4;
typedef vec4 position;
typedef vec4 velocity;
typedef vec4 angle;
typedef vec4 scale;
typedef vec4 angular_velocity;
typedef float bounding_radius;
typedef float*mat4;
typedef size_t id;
typedef float dt;
typedef float time_in_seconds;
typedef const char*path;
typedef size_t arrayix;

#define PI 3.1415926535897932384626433832795f

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
}baba;

