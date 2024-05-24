#pragma once
// reviewed: 2023-12-22
// reviewed: 2024-01-04
// reviewed: 2024-01-06
// reviewed: 2024-01-10

namespace glos {

class camera final {
public:
  enum class type { LOOK_AT, ANGLE, ORTHOGONAL };
  glm::vec3 position{0, 50, 0};
  glm::vec3 look_at{0, 0, -0.0001f};
  // note: -0.0001f because of the math of 'look at'
  glm::vec3 angle{0, 0, 0};
  float near_plane = 0.1f;
  float far_plane = 300;
  float fov = 60.0f; // field of view
  // window dimensions
  float width = 1024;
  float height = 1024;
  // from world coordinate to view to projection matrix
  glm::mat4 Mwvp{};
  // default view
  type type = type::ORTHOGONAL;
  float ortho_min_x = -50;
  float ortho_max_x = 50;
  float ortho_min_y = -50;
  float ortho_max_y = 50;

  inline auto init() -> void {}

  inline auto free() -> void {}

  inline auto update_matrix_wvp() -> void {
    switch (type) {
    case type::LOOK_AT: {
      float const aspect_ratio = height == 0 ? 1 : (width / height);
      glm::mat4 const Mv = glm::lookAt(position, look_at, {0, 1, 0});
      glm::mat4 const Mp = glm::perspective(glm::radians(fov), aspect_ratio,
                                            near_plane, far_plane);
      Mwvp = Mp * Mv;
      break;
    }
    case type::ORTHOGONAL: {
      glm::mat4 const Mv = glm::lookAt(position, look_at, {0, 1, 0});
      glm::mat4 const Mp = glm::ortho(ortho_min_x, ortho_max_x, ortho_min_y,
                                      ortho_max_y, near_plane, far_plane);
      Mwvp = Mp * Mv;
      break;
    }
    case type::ANGLE: {
      float const aspect_ratio = height == 0 ? 1 : (width / height);
      glm::mat4 const Mv = glm::eulerAngleXYZ(angle.x, angle.y, angle.z);
      glm::mat4 const Mp = glm::perspective(glm::radians(fov), aspect_ratio,
                                            near_plane, far_plane);
      Mwvp = Mp * Mv;
      break;
    }
    }
  }
};

static camera camera{};
} // namespace glos