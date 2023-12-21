#pragma once
#include "../lib.h"
#include <math.h>
#include <string.h>

#define mat4_identity                                                          \
  (mat4) { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }
#define mat3_identity                                                          \
  (mat3) { 1, 0, 0, 0, 1, 0, 0, 0, 1 }

inline static void mat4_assign(mat4 o, mat4 src) {
  memcpy(o, src, 16 * sizeof(float));
}

inline static void mat4_set_identity(mat4 o) { mat4_assign(o, mat4_identity); }

inline static void mat4_set_ortho_projection(mat4 o, const float left,
                                             const float right,
                                             const float bottom,
                                             const float top, const float nearz,
                                             const float farz) {
  o[0] = 2.f / (right - left);
  o[1] = 0;
  o[2] = 0;
  o[3] = 0;

  o[4] = 0;
  o[5] = 2.f / (top - bottom);
  o[6] = 0;
  o[7] = 0;

  o[8] = 0;
  o[9] = 0;
  o[10] = -2.f / (farz - nearz);
  o[11] = 0;

  o[12] = -(right + left) / (right - left);
  o[13] = -(top + bottom) / (top - bottom);
  o[14] = -(farz + nearz) / (farz - nearz);
  o[15] = 1;
}

inline static void mat4_set_look_at(mat4 o, const position *eye,
                                    const position *lookat, const vec4 *up) {

  vec4 zaxis;
  vec3_minus(&zaxis, lookat, eye);
  vec3_normalize(&zaxis); // zaxis

  vec4 xaxis;
  vec3_cross(&xaxis, up, &zaxis); // xaxis
  vec3_normalize(&xaxis);

  vec4 yaxis;
  vec3_cross(&yaxis, &zaxis, &xaxis);

  // invert by transposing orthonorm mat3
  //	 X
  o[0] = xaxis.x;
  o[1] = yaxis.x;
  o[2] = -zaxis.x;
  o[3] = 0;
  // Y
  o[4] = xaxis.y;
  o[5] = yaxis.y;
  o[6] = -zaxis.y;
  o[7] = 0;
  // Z
  o[8] = xaxis.z;
  o[9] = yaxis.z;
  o[10] = -zaxis.z;
  o[11] = 0;
  // T
  o[12] = 0;
  o[13] = 0;
  o[14] = 0;
  o[15] = 1;
}

inline static void mat4_get_axis_x(mat4 o, vec4 *result) {
  result->x = o[0];
  result->y = o[1];
  result->z = o[2];
  result->w = 0;
}

inline static void mat4_get_axis_y(mat4 o, vec4 *result) {
  result->x = o[4];
  result->y = o[5];
  result->z = o[6];
  result->w = 0;
}

inline static void mat4_get_axis_z(mat4 o, vec4 *result) {
  result->x = o[8];
  result->y = o[9];
  result->z = o[10];
  result->w = 0;
}

// from mesa
static void mat4_glFrustum(mat4 o, float left, float right, float bottom,
                           float top, float near, float far) {
  float RsubL = right - left;
  float TsubB = top - bottom;
  float FsubN = far - near;
  float Nmul2 = 2.0f * near;

  o[0] = Nmul2 / RsubL;
  o[1] = 0;
  o[2] = (right + left) / RsubL;
  o[3] = 0;

  o[4] = 0;
  o[5] = Nmul2 / TsubB;
  o[6] = (top + bottom) / TsubB;
  o[7] = 0;

  o[8] = 0;
  o[9] = 0;
  o[10] = -(far + near) / FsubN;
  o[11] = (-far * Nmul2) / FsubN;

  o[12] = 0;
  o[13] = 0;
  o[14] = -1.0f;
  o[15] = 0;
}

static void mat4_gluPerspective(mat4 o, float fov, float aspect, float near,
                                float far) {
  float ymax = near * tanf(0.5f * fov);
  float ymin = -ymax;
  float xmin = ymin * aspect;
  float xmax = ymax * aspect;

  mat4_glFrustum(o, xmin, xmax, ymin, ymax, near, far);
}

#define PI_OVER_180 0.0174532925199432957692369076849f
// #define 180_OVER_PI 57.2957795130823208767981548141f

#define DEG_TO_RAD(x) (x * PI_OVER_180)
// #define RAD_TO_DEG(x) (x * 180_OVER_PI)

// from stackoverflow

//--------------------------------------------------------------------------------
// set a perspective frustum (right hand)
// (left, right, bottom, top, near, far)
//--------------------------------------------------------------------------------
void perspective(mat4 o, float l, float r, float b, float t, float n, float f) {
  mat4_set_identity(o);
  o[0] = 2.0f * n / (r - l);
  o[2] = (r + l) / (r - l);
  o[5] = 2.0f * n / (t - b);
  o[6] = (t + b) / (t - b);
  o[10] = -(f + n) / (f - n);
  o[11] = -(2.0f * f * n) / (f - n);
  o[14] = -1.0f;
  o[15] = 0.0f;
}

//--------------------------------------------------------------------------------
// set a symmetric perspective frustum
// ((vertical, degrees) field of view, (width/height) aspect ratio, near, far)
//--------------------------------------------------------------------------------
void perspective_vertical(mat4 o, float fov, float aspect, float front,
                          float back) {
  fov = DEG_TO_RAD(fov); // transform fov from degrees to radians

  float tangent = tanf(fov / 2.0f); // tangent of half vertical fov
  float height = front * tangent;   // half height of near plane
  float width = height * aspect;    // half width of near plane

  perspective(o, -width, width, -height, height, front, back);
}
//

inline static void mat4_set_rotation_y(mat4 o, float angle_rad) {
  // X
  o[0] = cosf(angle_rad);
  o[1] = 0;
  o[2] = -sinf(angle_rad);
  o[3] = 0;

  // Y
  o[4] = 0;
  o[5] = 1;
  o[6] = 0;
  o[7] = 0;

  // Z
  o[8] = sinf(angle_rad);
  o[9] = 0;
  o[10] = cosf(angle_rad);
  o[11] = 0;

  // T
  o[12] = 0;
  o[13] = 0;
  o[14] = 0;
  o[15] = 1;
}

inline static void mat4_set_rotation_z(mat4 o, float angle_rad) {
  // X
  o[0] = cosf(angle_rad);
  o[1] = sinf(angle_rad);
  o[2] = 0;
  o[3] = 0;

  // Y
  o[4] = -sinf(angle_rad);
  o[5] = cosf(angle_rad);
  o[6] = 0;
  o[7] = 0;

  // Z
  o[8] = 0;
  o[9] = 0;
  o[10] = 1;
  o[11] = 0;

  // T
  o[12] = 0;
  o[13] = 0;
  o[14] = 0;
  o[15] = 1;
}

inline static void mat4_set_translation(mat4 o, const position *p) {
  // [ 0 4  8  x ]
  // [ 1 5  9  y ]
  // [ 2 6 10  z ]
  // [ 3 7 11 15 ]
  o[1] = o[2] = o[3] = o[4] = o[6] = o[7] = o[8] = o[9] = o[11] = 0;
  o[0] = o[5] = o[10] = o[15] = 1;
  o[12] = p->x;
  o[13] = p->y;
  o[14] = p->z;
}

inline static void mat4_append_rotation_about_z_axis(mat4 o, float degrees) {
  // [ 0 4  8 12 ]   [ cos -sin 0  0 ]
  // [ 1 5  9 13 ] x [ sin cos  0  0 ]
  // [ 2 6 10 14 ]   [ 0   0    1  0 ]
  // [ 3 7 11 15 ]   [ 0   0    0  1 ]
  const float rad = degrees * (float)(PI / 180.0f);
  const float cosrad = cosf(rad);
  const float sinrad = sinf(rad);

  const float mtx00 = o[0];
  const float mtx01 = o[1];
  const float mtx02 = o[2];
  const float mtx03 = o[3];

  o[0] = o[4] * sinrad + mtx00 * cosrad;
  o[4] = o[4] * cosrad - mtx00 * sinrad;

  o[1] = o[5] * sinrad + mtx01 * cosrad;
  o[5] = o[5] * cosrad - mtx01 * sinrad;

  o[2] = o[6] * sinrad + mtx02 * cosrad;
  o[6] = o[6] * cosrad - mtx02 * sinrad;

  o[3] = o[7] * sinrad + mtx03 * cosrad;
  o[7] = o[7] * cosrad - mtx03 * sinrad;
}

inline static void mat4_set_scale(mat4 o, scale *s) {
  // [ 0 4  8 12 ]   [ x 0 0 0 ]
  // [ 1 5  9 13 ] x [ 0 y 0 0 ]
  // [ 2 6 10 14 ]   [ 0 0 z 0 ]
  // [ 3 7 11 15 ]   [ 0 0 0 1 ]

  memset(o, 0, 16);

  o[0] = s->x;
  o[4] = s->y;
  o[8] = s->z;

  o[1] = s->x;
  o[5] = s->y;
  o[9] = s->z;

  o[2] = s->x;
  o[6] = s->y;
  o[10] = s->z;

  o[3] = s->x;
  o[7] = s->y;
  o[11] = s->z;
}

inline static void mat3_scale(mat4 o, scale *s) {
  // [ 0 4  8 12 ]   [ x 0 0 0 ]
  // [ 1 5  9 13 ] x [ 0 y 0 0 ]
  // [ 2 6 10 14 ]   [ 0 0 z 0 ]
  // [ 3 7 11 15 ]   [ 0 0 0 1 ]
  o[0] *= s->x;
  o[4] *= s->y;
  o[8] *= s->z;

  o[1] *= s->x;
  o[5] *= s->y;
  o[9] *= s->z;

  o[2] *= s->x;
  o[6] *= s->y;
  o[10] *= s->z;

  o[3] *= s->x;
  o[7] *= s->y;
  o[11] *= s->z;
}

inline static /*gives*/ float *mat4_multiply(mat4 ret, const mat4 lhs,
                                             const mat4 rhs) {
  // [ 0 4  8 12 ]   [ 0 4  8 12 ]
  // [ 1 5  9 13 ] x [ 1 5  9 13 ]
  // [ 2 6 10 14 ]   [ 2 6 10 14 ]
  // [ 3 7 11 15 ]   [ 3 7 11 15 ]

  //	float*ret=malloc(sizeof(float)*16);
  ret[0] =
      lhs[0] * rhs[0] + lhs[4] * rhs[1] + lhs[8] * rhs[2] + lhs[12] * rhs[3];
  ret[1] =
      lhs[1] * rhs[0] + lhs[5] * rhs[1] + lhs[9] * rhs[2] + lhs[13] * rhs[3];
  ret[2] =
      lhs[2] * rhs[0] + lhs[6] * rhs[1] + lhs[10] * rhs[2] + lhs[14] * rhs[3];
  ret[3] =
      lhs[3] * rhs[0] + lhs[7] * rhs[1] + lhs[11] * rhs[2] + lhs[15] * rhs[3];

  ret[4] =
      lhs[0] * rhs[4] + lhs[4] * rhs[5] + lhs[8] * rhs[6] + lhs[12] * rhs[7];
  ret[5] =
      lhs[1] * rhs[4] + lhs[5] * rhs[5] + lhs[9] * rhs[6] + lhs[13] * rhs[7];
  ret[6] =
      lhs[2] * rhs[4] + lhs[6] * rhs[5] + lhs[10] * rhs[6] + lhs[14] * rhs[7];
  ret[7] =
      lhs[3] * rhs[4] + lhs[7] * rhs[5] + lhs[11] * rhs[6] + lhs[15] * rhs[7];

  ret[8] =
      lhs[0] * rhs[8] + lhs[4] * rhs[9] + lhs[8] * rhs[10] + lhs[12] * rhs[11];
  ret[9] =
      lhs[1] * rhs[8] + lhs[5] * rhs[9] + lhs[9] * rhs[10] + lhs[13] * rhs[11];
  ret[10] =
      lhs[2] * rhs[8] + lhs[6] * rhs[9] + lhs[10] * rhs[10] + lhs[14] * rhs[11];
  ret[11] =
      lhs[3] * rhs[8] + lhs[7] * rhs[9] + lhs[11] * rhs[10] + lhs[15] * rhs[11];

  ret[12] = lhs[0] * rhs[12] + lhs[4] * rhs[13] + lhs[8] * rhs[14] +
            lhs[12] * rhs[15];
  ret[13] = lhs[1] * rhs[12] + lhs[5] * rhs[13] + lhs[9] * rhs[14] +
            lhs[13] * rhs[15];
  ret[14] = lhs[2] * rhs[12] + lhs[6] * rhs[13] + lhs[10] * rhs[14] +
            lhs[14] * rhs[15];
  ret[15] = lhs[3] * rhs[12] + lhs[7] * rhs[13] + lhs[11] * rhs[14] +
            lhs[15] * rhs[15];
  return ret;
}
