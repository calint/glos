#pragma once
// reviewed: 2024-01-04
// reviewed: 2024-01-06
// reviewed: 2024-01-10

namespace glos {

class planes final {
public:
  bool points_invalidated = true;
  bool normals_invalidated = true;

  // points and normals are in model coordinates
  // Mmw matrix was constructed using pos, agl, scl are
  inline void update_model_to_world(std::vector<glm::vec3> const &points,
                                    std::vector<glm::vec3> const &normals,
                                    glm::mat4 const &Mmw, glm::vec3 const &pos,
                                    glm::vec3 const &agl,
                                    glm::vec3 const &scl) {
    // points
    if (points_invalidated or pos != Mmw_pos or agl != Mmw_agl or
        scl != Mmw_scl) {
      // matrix state is not in sync with current state
      Mmw_pos = pos;
      Mmw_agl = agl;
      Mmw_scl = scl;

      // update world_positions
      world_points.clear();
      for (glm::vec3 const &Pm : points) {
        glm::vec4 const Pw = Mmw * glm::vec4{Pm, 1.0f};
        world_points.emplace_back(Pw);
      }

      points_invalidated = false;
    }

    // normals
    if (normals_invalidated or agl != Nmw_agl) {
      // update matrix
      Nmw_agl = agl;
      glm::mat3 const Nmw =
          glm::mat3{glm::eulerAngleXYZ(Nmw_agl.x, Nmw_agl.y, Nmw_agl.z)};

      // update world normals
      world_normals.clear();
      for (glm::vec3 const &Nm : normals) {
        glm::vec3 const Nw = Nmw * Nm;
        world_normals.emplace_back(Nw);
      }

      normals_invalidated = false;
    }
  }

  inline void print() const {
    size_t const n = world_normals.size();
    for (unsigned i = 0; i < n; ++i) {
      glm::vec3 const &pnt = world_points.at(i);
      glm::vec3 const &nml = world_normals.at(i);
      printf("normal %u: %s  ->  %s\n", i, glm::to_string(pnt).c_str(),
             glm::to_string(nml).c_str());
    }
    size_t const m = world_points.size();
    for (size_t i = n; i < m; ++i) {
      glm::vec3 const &pnt = world_points.at(i);
      printf("point %s\n", glm::to_string(pnt).c_str());
    }
  }

  inline void debug_render_normals() {
    acquire_lock();
    size_t const n = world_normals.size();
    for (unsigned i = 0; i < n; ++i) {
      glm::vec3 const &pnt = world_points.at(i);
      glm::vec3 const &nml = world_normals.at(i);
      debug_render_wcs_line(pnt, pnt + nml, {1, 0, 0, 0.5f});
    }
    size_t const m = world_points.size();
    for (size_t i = n; i < m; ++i) {
      glm::vec3 const &pt = world_points.at(i);
      debug_render_wcs_line(pt, pt + glm::vec3{0, 0.2f, -0.2f},
                            {1, 1, 1, 0.5f});
    }
    release_lock();
  }

  // assumes both this and 'pns' have updated world points and normals
  // returns true if any point in this is behind all planes in 'pns'
  inline auto is_any_point_in_volume(planes const &pns) const -> bool {
    return std::ranges::any_of(world_points, [&](glm::vec3 const &p) {
      return pns.is_point_behind_all_planes(p);
    });
  }

  // works in cases where the sphere is much smaller than the convex volume
  // e.g. bullets vs walls. gives false positives when spheres are larger than
  // the volume and the volume has "pointy" edges
  // workaround: add some more planes to the volume at the "pointy" edges
  inline auto is_in_collision_with_sphere(glm::vec3 const &pos,
                                          float const radius) const -> bool {

    return is_in_collision_with_sphere_sat(pos, radius);

    size_t const n = world_normals.size();
    for (unsigned i = 0; i < n; ++i) {
      glm::vec3 const v = pos - world_points[i];
      glm::vec3 const &nml = world_normals[i];
      float const d = glm::dot(v, nml);
      if (d > radius) {
        return false;
      }
    }

    return true;
  }

  inline auto is_in_collision_with_sphere_sat(glm::vec3 const &pos,
                                              float const radius) const
      -> bool {

    // check for separation along each normal
    for (glm::vec3 const &normal : world_normals) {
      float min_projection = glm::dot(world_points[0], normal);
      float max_projection = min_projection;

      // project both the sphere and the convex volume onto the normal
      size_t const n = world_normals.size();
      for (size_t i = 1; i < n; ++i) {
        glm::vec3 const &point = world_points[i];
        float const projection = glm::dot(point, normal);
        if (projection < min_projection) {
          min_projection = projection;
        } else if (projection > max_projection) {
          max_projection = projection;
        }
      }

      float const sphere_projection = glm::dot(pos, normal);

      // check for separation
      if (sphere_projection + radius < min_projection or
          sphere_projection - radius > max_projection) {
        return false; // separating axis found, no collision
      }

      debug_render_wcs_line(normal * (sphere_projection + radius),
                            normal * (sphere_projection - radius),
                            {1.0f, 0.0f, 0.0f, 0.2f});

      debug_render_wcs_line(normal * min_projection, normal * max_projection,
                            {0.0f, 0.0f, 1.0f, 0.2f});
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
  // cached world coordinate system points and normals
  // note. there is a point for each normal followed by additional points
  //       without normals used in collision detection
  std::vector<glm::vec3> world_points{};
  std::vector<glm::vec3> world_normals{};
  // the components used in the cached points and normals
  glm::vec3 Mmw_pos{};
  glm::vec3 Mmw_agl{};
  glm::vec3 Mmw_scl{};
  glm::vec3 Nmw_agl{};
  //
  std::atomic_flag lock = ATOMIC_FLAG_INIT;

  inline auto is_point_behind_all_planes(glm::vec3 const &p) const -> bool {
    size_t const n = world_normals.size();
    for (unsigned i = 0; i < n; ++i) {
      glm::vec3 const v = p - world_points[i];
      glm::vec3 const &nml = world_normals[i];
      float const d = glm::dot(v, nml);
      if (d > 0) {
        return false;
      }
    }
    return true;
  }
};
} // namespace glos