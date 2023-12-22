#pragma once
// reviewed: 2023-12-22

class volume final {
public:
  float radius = 0;
  glm::vec3 scale{};

  inline void update_radius_using_scale() {
    radius = sqrtf(scale.x * scale.x + scale.y * scale.y);
  }
};

// inline static line  make_line_from_2_points(point p0,point p1);
// inline static plane make_plane_from_3_points(point p0,point p1,point p2);
// inline static float solve_3d_spheres_intersect(sphere s1,sphere s2);
// inline static float solve_3d_distance_point_to_sphere(line l,plane p);
// inline static float solve_3d_distance_point_to_plane(line l,plane p);
// inline static float solve_3d_distance_sphere_to_plane(line l,plane p);
// inline static bool  solve_3d_is_dot_behind_all_planes(point p,planes ps);
// inline static float solve_3d_where_line_intersects_plane(line l,plane p);
