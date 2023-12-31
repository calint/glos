#pragma once
// reviewed: 2023-12-22

namespace glos {
class volume final {

public:
  float radius = 0;
  glm::vec3 scale{};
  planes planes{};
  bool is_sphere = true;
};
} // namespace glos