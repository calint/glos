#pragma once
// reviewed: 2023-12-22

namespace glos {
class physics final {
public:
  glm::vec3 position{};
  glm::vec3 velocity{};
  glm::vec3 acceleration{};
  glm::vec3 angle{};
  glm::vec3 angular_velocity{};
  float mass = 0;

  inline void step(const frame_ctx &fc) {
    const float dt = fc.dt;
    velocity += acceleration * dt;
    position += velocity * dt;
    angle += angular_velocity * dt;
  }

  inline void apply_impulse(const glm::vec3 &force, const frame_ctx &fc) {
    glm::vec3 acc = force / mass;
    velocity += acc * fc.dt;
    std::cout << glm::to_string(velocity) << std::endl;
  }
};
} // namespace glos