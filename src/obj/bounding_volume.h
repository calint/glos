#pragma once
#include "../lib.h"

//------------------------------------------------------------- bounding volume
typedef struct bounding_volume {
  radius radius;
  scale scale;
} bounding_volume;
#define bounding_volume_def                                                    \
  {                                                                            \
    1, { 1, 1, 1, 0 }                                                          \
  }

inline static void
bounding_volume_update_radius_using_scale(bounding_volume *o) {
  o->radius = (radius)sqrtf(o->scale.x * o->scale.x + o->scale.y * o->scale.y);
}

//
// typedef vec3 vector;
//
// typedef struct line{
//	point p0;
//	point p1;
//}line;
//
// typedef float radius;
//
// typedef struct sphere{
//	point p;
//	float r;
//}sphere;
//
// typedef dynp planes;
//
// typedef struct plane{
//	point p;
//	vector np;
//}plane;
//
// typedef dynp planes;
//
// typedef vector box;
//
////typedef struct bvol{
////	sphere sr;
////	box bx;
////	planes ps;
////}bvol;
//
// inline static line  make_line_from_2_points(point p0,point p1);
// inline static plane make_plane_from_3_points(point p0,point p1,point p2);
// inline static float solve_3d_spheres_intersect(sphere s1,sphere s2);
// inline static float solve_3d_distance_point_to_sphere(line l,plane p);
// inline static float solve_3d_distance_point_to_plane(line l,plane p);
// inline static float solve_3d_distance_sphere_to_plane(line l,plane p);
// inline static bool  solve_3d_is_dot_behind_all_planes(point p,planes ps);
// inline static float solve_3d_where_line_intersects_plane(line l,plane p);
