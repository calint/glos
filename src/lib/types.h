#pragma once
#include "consts.h"
#include <inttypes.h>
typedef struct vec2 {
  float x, y;
} vec2;
typedef struct vec3 {
  float x, y, z;
} vec3;
typedef struct vec4 {
  float x, y, z, w;
} vec4;
#define vec4_def                                                               \
  (vec4) { 0, 0, 0, 0 }
typedef float mat4[4 * 4];
typedef float mat3[3 * 3];
typedef vec4 position;
typedef position point;
typedef vec4 velocity;
typedef vec4 angle;
typedef vec4 scale;
typedef vec4 angular_velocity;
typedef float radius;
typedef const char *path;

class frame_ctx {
public:
  float dt;
  unsigned tick;
};
