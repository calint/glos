#pragma once
// reviewed: 2023-12-22

namespace glos {

class camera final {
public:
  enum class type { LOOK_AT, ANGLE, ORTHO };
  glm::vec3 position{0, 100, 100};
  glm::vec3 look_at{0, 0, 0};
  glm::vec3 angle{0, 0, 0};
  float near_plane = 0.1f;
  float far_plane = 300;
  // field of view
  float fov = 60.0f;
  // window dimensions
  float width = 1024;
  float height = 1024;
  // view-projection matrix
  glm::mat4 Mvp{};
  type type = type::ORTHO;
  float ortho_min_x = -50;
  float ortho_max_x = 50;
  float ortho_min_y = -50;
  float ortho_max_y = 50;

  inline void init() {}

  inline void free() {}

  inline void update_matrix_wvp() {
    switch (type) {
    case type::LOOK_AT: {
      const float aspect_ratio = height == 0 ? 1 : (width / height);
      glm::mat4 Mv = glm::lookAt(position, look_at, {0, 1, 0});
      glm::mat4 Mp = glm::perspective(glm::radians(fov), aspect_ratio,
                                      near_plane, far_plane);
      Mvp = Mp * Mv;
      break;
    }
    case type::ORTHO: {
      glm::mat4 Mv = glm::lookAt(position, look_at, {0, 1, 0});
      // glm::mat4 Mv = glm::eulerAngleXYZ(angle.x, angle.y, angle.z);
      glm::mat4 Mp = glm::ortho(ortho_min_x, ortho_max_x, ortho_min_y,
                                ortho_max_y, near_plane, far_plane);
      Mvp = Mp * Mv;
      break;
    }
    case type::ANGLE: {
      const float aspect_ratio = height == 0 ? 1 : (width / height);
      glm::mat4 Mv = glm::eulerAngleXYZ(angle.x, angle.y, angle.z);
      glm::mat4 Mp = glm::perspective(glm::radians(fov), aspect_ratio,
                                      near_plane, far_plane);
      Mvp = Mp * Mv;
      break;
    }
    }
  }
};

static camera camera{};
} // namespace glos