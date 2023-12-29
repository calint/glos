#pragma once
// reviewed: 2023-12-22

class volume final {
  class planes final {
    // transform matrixes
    glm::mat4 Mmw{};
    glm::mat3 Nmw{};

    // cached world coordinate system positions and normals
    std::vector<glm::vec3> world_positions{};
    std::vector<glm::vec3> world_normals{};
    glm::vec3 Mmw_pos{};
    glm::vec3 Mmw_agl{};
    glm::vec3 Mmw_scl{};
    glm::vec3 Nmw_agl{};
    bool first_update = true;

  public:
    inline void update_model_to_world(const std::vector<glm::vec3> points,
                                      const std::vector<glm::vec3> normals,
                                      const glm::vec3 &position,
                                      const glm::vec3 &angle,
                                      const glm::vec3 &scale) {
      if (position != Mmw_pos or angle != Mmw_agl or scale != Mmw_scl) {
        // make a new matrix
        Mmw_pos = position;
        Mmw_agl = angle;
        Mmw_scl = scale;
        glm::mat4 Ms = glm::scale(glm::mat4(1), Mmw_scl);
        glm::mat4 Mr = glm::eulerAngleXYZ(Mmw_agl.x, Mmw_agl.y, Mmw_agl.z);
        glm::mat4 Mt = glm::translate(glm::mat4(1), Mmw_pos);
        Mmw = Mt * Mr * Ms;

        // update world_positions
        world_positions.clear();
        for (const glm::vec3 &Pm : points) {
          glm::vec4 Pw = Mmw * glm::vec4(Pm, 1.0f);
          world_positions.emplace_back(glm::vec3(Pw));
        }
      }
      // normals
      if (first_update or angle != Nmw_agl) {
        first_update = false;
        // update world_normals
        Nmw_agl = angle;
        glm::mat4 Nr = glm::eulerAngleXYZ(Nmw_agl.x, Nmw_agl.y, Nmw_agl.z);
        Nmw = glm::mat3(Nr);

        // update world normals
        world_normals.clear();
        for (const glm::vec3 &Nm : normals) {
          glm::vec3 Nw = Nmw * Nm;
          world_normals.emplace_back(Nw);
        }
      }
    }

    inline auto is_any_point_behind_all_planes(const planes &planes) const
        -> bool {
      // for each position check if behind all planes in 'vol'
      // assumes both this and 'vol' has updated world positions and normals
      for (const glm::vec3 &p : world_positions) {
        if (planes.is_point_behind_all_planes(p)) {
          return true;
        }
      }
      return false;
    }

    inline auto is_point_behind_all_planes(const glm::vec3 &p) const -> bool {
      const size_t n = world_positions.size();
      for (unsigned i = 0; i < n; ++i) {
        const glm::vec3 v = p - world_positions[i];
        const glm::vec3 n = world_normals[i];
        const float d = glm::dot(n, v);
        if (d >= 0) {
          return false;
        }
      }
      return true;
    }

    inline void debug_render_normals(const std::vector<glm::vec3> points,
                                     const std::vector<glm::vec3> normals,
                                     const glm::vec3 &position,
                                     const glm::vec3 &angle,
                                     const glm::vec3 &scale) {
      update_model_to_world(points, normals, position, angle, scale);
      const size_t n = world_positions.size();
      for (size_t i = 0; i < n; ++i) {
        const glm::vec3 &pnt = world_positions[i];
        const glm::vec3 &nml = world_normals[i];
        render_wcs_line(pnt, pnt + nml, {1, 0, 0});
      }
    }
  };

public:
  float radius = 0;
  glm::vec3 scale{};
  planes planes{};
};
