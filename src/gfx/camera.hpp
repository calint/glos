#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class camera final {
public:
  glm::vec3 pos{0, 100, 100};
  glm::mat4 mtx_vp{};
  glm::vec3 target{0, 0, 0};
  glm::vec3 up{0, 1, 0};
  float near_plane = 0.1;
  float far_plane = 1000;
  float fov = 60.0f;
  float wi = 1024;
  float hi = 1024;

  inline void init() {}

  inline void free() {}

  inline void update_matrix_wvp() {
    float aspect_ratio = wi / hi;
    glm::mat4 mtx_v = glm::lookAt(pos, target, up);
    glm::mat4 mtx_p = glm::perspective(glm::radians(fov), aspect_ratio,
                                       near_plane, far_plane);
    mtx_vp = mtx_p * mtx_v;
  }
};

static camera camera{};
