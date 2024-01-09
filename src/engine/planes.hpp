#pragma once
// reviewed: 2024-01-04
// reviewed: 2024-01-06

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
      glm::mat4 const Nr = glm::eulerAngleXYZ(Nmw_agl.x, Nmw_agl.y, Nmw_agl.z);
      Nmw = glm::mat3{Nr};

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
    size_t const n = world_points.size();
    for (unsigned i = 0; i < n; ++i) {
      glm::vec3 const &pnt = world_points[i];
      glm::vec3 const &nml = world_normals[i];
      printf("normal %u: %s  ->  %s\n", i, glm::to_string(pnt).c_str(),
             glm::to_string(nml).c_str());
    }
  }

  inline void debug_render_normals() {
    acquire_lock();
    size_t const n = world_normals.size();
    for (unsigned i = 0; i < n; ++i) {
      glm::vec3 const &pnt = world_points.at(i);
      glm::vec3 const &nml = world_normals.at(i);
      debug_render_wcs_line(pnt, pnt + nml, {1, 0, 0});
    }
    size_t const m = world_points.size();
    for (size_t i = n; i < m; ++i) {
      glm::vec3 const &pt = world_points.at(i);
      debug_render_wcs_line(pt, pt + glm::vec3{0, 0.2f, -0.2f}, {1, 1, 1});
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

  inline auto is_in_collision_with_sphere(glm::vec3 const &pos,
                                          float const radius) const -> bool {
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

  inline void acquire_lock() {
    while (spinlock.test_and_set(std::memory_order_acquire)) {
    }
  }

  inline void release_lock() { spinlock.clear(std::memory_order_release); }

private:
  // normals rotation matrix from model to world coordinate system
  glm::mat3 Nmw{};

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
  std::atomic_flag spinlock = ATOMIC_FLAG_INIT;

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