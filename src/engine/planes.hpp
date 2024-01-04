#pragma once
// reviewed: 2024-01-04

#include <glm/gtx/string_cast.hpp>

namespace glos {
class planes final {
  // normals rotation matrix from model to world coordinate system
  glm::mat3 Nmw{};

  // cached world coordinate system points and normals
  std::vector<glm::vec3> world_points{};
  std::vector<glm::vec3> world_normals{};
  // the components used in the cached points and normals
  glm::vec3 Mmw_pos{};
  glm::vec3 Mmw_agl{};
  glm::vec3 Mmw_scl{};
  glm::vec3 Nmw_agl{};
  // necessary because Nmw_agl is zero and so might also be the object angle
  // before any changes
  bool first_update = true;
  //
  std::atomic_flag spinlock = ATOMIC_FLAG_INIT;

public:
  inline void update_model_to_world(std::vector<glm::vec3> const &points,
                                    std::vector<glm::vec3> const &normals,
                                    glm::mat4 const &Mmw, glm::vec3 const &pos,
                                    glm::vec3 const &agl,
                                    glm::vec3 const &scl) {
    if (points.size() != 0 and
        (pos != Mmw_pos or agl != Mmw_agl or scl != Mmw_scl)) {
      // matrix state is not in sync with current state
      Mmw_pos = pos;
      Mmw_agl = agl;
      Mmw_scl = scl;
      // update world_positions
      world_points.clear();
      for (glm::vec3 const &Pm : points) {
        glm::vec4 Pw = Mmw * glm::vec4{Pm, 1.0f};
        world_points.emplace_back(glm::vec3{Pw});
      }
    }
    // normals
    if (normals.size() != 0 and (first_update or agl != Nmw_agl)) {
      first_update = false;
      // update world_normals
      Nmw_agl = agl;
      glm::mat4 Nr = glm::eulerAngleXYZ(Nmw_agl.x, Nmw_agl.y, Nmw_agl.z);
      Nmw = glm::mat3{Nr};

      // update world normals
      world_normals.clear();
      for (glm::vec3 const &Nm : normals) {
        glm::vec3 Nw = Nmw * Nm;
        world_normals.emplace_back(Nw);
      }
    }
  }

  inline void print() {
    const size_t n = world_points.size();
    for (size_t i = 0; i < n; ++i) {
      glm::vec3 const &pnt = world_points[i];
      glm::vec3 const &nml = world_normals[i];
      printf("normal %zu: %s  ->  %s\n", i, glm::to_string(pnt).c_str(),
             glm::to_string(nml).c_str());
    }
  }

  inline void debug_render_normals() {
    size_t const n = world_points.size();
    for (size_t i = 0; i < n; ++i) {
      glm::vec3 const &pnt = world_points.at(i);
      glm::vec3 const &nml = world_normals.at(i);
      debug_render_wcs_line(pnt, pnt + nml, {1, 0, 0});
    }
  }

  // assumes both this and 'pns' have updated world points and normals
  // returns true if any point in this is behind all planes in 'pns'
  inline auto is_in_collision_with_planes(planes const &pns) const -> bool {
    for (glm::vec3 const &p : world_points) {
      if (pns.is_point_behind_all_planes(p)) {
        return true;
      }
    }
    return false;
  }

  inline auto is_in_collision_with_sphere(glm::vec3 const &pos,
                                          float const radius) const -> bool {
    size_t const n = world_normals.size();
    for (unsigned i = 0; i < n; ++i) {
      glm::vec3 const v = pos - world_points[i];
      glm::vec3 const nml = world_normals[i];
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
  inline auto is_point_behind_all_planes(glm::vec3 const &p) const -> bool {
    size_t const n = world_normals.size();
    for (unsigned i = 0; i < n; ++i) {
      // render_wcs_line(world_positions[i], p, {0, 1, 0});
      glm::vec3 const v = p - world_points[i];
      glm::vec3 const nml = world_normals[i];
      float const d = glm::dot(v, nml);
      if (d > 0) {
        return false;
      }
    }
    return true;
  }
};
} // namespace glos