#pragma once
// reviewed: 2024-01-04
// reviewed: 2024-01-06
// reviewed: 2024-01-10

namespace glos {

class planes final {
public:
  bool invalidated = true;

  // points and normals are in model coordinates
  // Mmw matrix was constructed using pos, agl, scl
  inline void update_model_to_world(std::vector<glm::vec4> const &points,
                                    std::vector<glm::vec3> const &normals,
                                    glm::mat4 const &Mmw, glm::vec3 const &pos,
                                    glm::vec3 const &agl,
                                    glm::vec3 const &scl) {

    bool const angle_scale_changed = Mmw_agl != agl or Mmw_scl != scl;

    if (invalidated or pos != Mmw_pos or angle_scale_changed) {
      // world points and normals are not in sync with object Mmw
      world_points.clear();
      for (glm::vec4 const &point : points) {
        glm::vec4 const Pw = Mmw * point;
        world_points.emplace_back(Pw);
      }

      if (angle_scale_changed) {
        // the generalized solution is:
        // glm::mat3 const N = glm::transpose(glm::inverse(glm::mat3(Mmw)));
        // but since it is known how Mmw is composed a less expensive operations
        // is done

        bool const is_uniform_scale = scl.x == scl.y and scl.y == scl.z;

        glm::mat3 const N =
            is_uniform_scale
                ? glm::eulerAngleXYZ(agl.x, agl.y, agl.z)
                : glm::scale(
                      glm::eulerAngleXYZ(agl.x, agl.y, agl.z),
                      glm::vec3{1.0f / scl.x, 1.0f / scl.y, 1.0f / scl.z});

        world_normals.clear();
        world_planes.clear();
        size_t const n = normals.size();
        for (size_t i = 0; i < n; ++i) {
          glm::vec3 const normal = N * normals[i];
          world_normals.emplace_back(normal);
          world_planes.emplace_back(glm::vec4{normal, 0});
        }
        // save the state of the cache
        Mmw_agl = agl;
        Mmw_scl = scl;
      }

      // update planes
      size_t const n = world_normals.size();
      for (size_t i = 0; i < n; ++i) {
        glm::vec3 const &normal = world_normals[i];
        glm::vec3 const &point = world_points[i];
        // d in a*x+b*y+z*d+d=0
        world_planes[i].w = -glm::dot(normal, point);
      }
      // save the state of the cache
      Mmw_pos = pos;
      invalidated = false;
    }
  }

  inline void debug_render_normals() {
    acquire_lock();
    size_t const n = world_planes.size();
    for (unsigned i = 0; i < n; ++i) {
      glm::vec4 const &point = world_points.at(i);
      glm::vec4 const &plane = world_planes.at(i);
      debug_render_wcs_line(point, point + plane, {1, 0, 0, 0.5f});
    }
    size_t const m = world_points.size();
    for (size_t i = n; i < m; ++i) {
      glm::vec4 const &point = world_points.at(i);
      debug_render_wcs_line(point, point + glm::vec4{0, 0.2f, -0.2f, 1.0f},
                            {1, 1, 1, 0.5f});
    }
    release_lock();
  }

  // assumes both this and 'pns' have updated world points and normals
  // returns true if any point in this is behind all planes in 'pns'
  inline auto is_any_point_in_volume(planes const &pns) const -> bool {
    return std::ranges::any_of(world_points, [&](glm::vec4 const &point) {
      return pns.is_point_in_volume(point);
    });
  }

  // assumes updated world points and normals
  inline auto is_point_in_volume(glm::vec4 const &point) const -> bool {
    return std::ranges::all_of(world_planes, [&](glm::vec4 const &plane) {
      return glm::dot(plane, point) <= 0;
    });
  }

  // works in cases where the sphere is much smaller than the convex volume
  // e.g. bullets vs walls. gives false positives when spheres are larger than
  // the volume and the volume has "pointy" edges because there are positions
  // where the sphere i "within" the collision planes "outside" the volume
  // workaround: add some more planes to the volume at the "pointy" edges
  inline auto is_in_collision_with_sphere(glm::vec3 const &position,
                                          float const radius) const -> bool {

    // return is_in_collision_with_sphere_sat(position, radius);

    glm::vec4 const point{position, 1.0f};
    return std::ranges::all_of(world_planes, [&](glm::vec4 const &plane) {
      return glm::dot(point, plane) / glm::length(glm::vec3{plane}) <= radius;
    });

    // bool const definitely_no =
    //     std::ranges::any_of(world_planes, [&](glm::vec4 const &plane) {
    //       return glm::dot(point, plane) / glm::length(glm::vec3{plane}) >
    //              radius;
    //     });

    // if (definitely_no) {
    //   printf("definitely no\n");
    //   return false;
    // }

    // for (glm::vec4 const &wp : world_points) {
    //   glm::vec3 const nml = glm::normalize(wp - point);
    //   glm::vec3 const pt_on_plane = nml * radius;
    //   glm::vec3 const v = glm::vec3{wp} - pt_on_plane;
    //   float const d = glm::dot(v, nml);
    //   if (d < 0) {
    //     printf("found point behind plane\n");
    //     return true;
    //   }
    // }

    // return false;
  }

  // there are cases that give false positive
  inline auto is_in_collision_with_sphere_sat(glm::vec3 const &pos,
                                              float const radius) const
      -> bool {

    // check for separation along each normal
    for (glm::vec4 const &plane : world_planes) {
      glm::vec3 nml{plane};

      float min_projection = glm::dot(glm::vec3{world_points[0]}, nml);
      float max_projection = min_projection;

      // project both the sphere and the convex volume onto the normal
      size_t const n = world_planes.size();
      for (size_t i = 1; i < n; ++i) {
        glm::vec3 const &point = world_points[i];
        float const projection = glm::dot(point, nml);
        if (projection < min_projection) {
          min_projection = projection;
        } else if (projection > max_projection) {
          max_projection = projection;
        }
      }

      float const sphere_projection = glm::dot(pos, nml);

      // check for separation
      if (sphere_projection + radius < min_projection or
          sphere_projection - radius > max_projection) {
        return false; // separating axis found, no collision
      }

      debug_render_wcs_line(nml * (sphere_projection + radius),
                            nml * (sphere_projection - radius),
                            {1.0f, 0.0f, 0.0f, 0.5f});

      debug_render_wcs_line(nml * min_projection, nml * max_projection,
                            {0.0f, 0.0f, 1.0f, 0.5f});
    }

    return true; // no separating axis found, collision detected
  }

  inline void acquire_lock() {
    while (lock.test_and_set(std::memory_order_acquire)) {
    }
  }

  inline void release_lock() { lock.clear(std::memory_order_release); }

  inline static auto are_in_collision(planes const &pns1, planes const &pns2)
      -> bool {
    return pns1.is_any_point_in_volume(pns2) or
           pns2.is_any_point_in_volume(pns1);
  }

private:
  // cached world coordinate system points and planes
  // note. there is a point for each plane followed by additional points
  //       without planes for use in collision detection
  std::vector<glm::vec4> world_points{};
  std::vector<glm::vec3> world_normals{};
  std::vector<glm::vec4> world_planes{}; // A*X + B*Y + C*Z + D = 0
  // the components used in the cached points and normals
  glm::vec3 Mmw_pos{};
  glm::vec3 Mmw_agl{};
  glm::vec3 Mmw_scl{};
  //
  std::atomic_flag lock = ATOMIC_FLAG_INIT;
};
} // namespace glos