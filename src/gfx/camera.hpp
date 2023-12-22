#pragma once
// reviewed: 2023-12-22

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class camera final {
public:
  glm::vec3 position{0, 100, 100};
  glm::vec3 look_at{0, 0, 0};
  float near_plane = 0.1;
  float far_plane = 1000;
  // field of view
  float fov = 60.0f;
  // window dimensions
  float width = 1024;
  float height = 1024;
  // view-projection matrix
  glm::mat4 Mvp{};

  inline void init() {}

  inline void free() {}

  inline void update_matrix_wvp() {
    const float aspect_ratio = height == 0 ? 1 : (width / height);
    glm::mat4 Mv = glm::lookAt(position, look_at, {0, 1, 0});
    glm::mat4 Mp = glm::perspective(glm::radians(fov), aspect_ratio,
                                       near_plane, far_plane);
    Mvp = Mp * Mv;
  }
};

static camera camera{};
