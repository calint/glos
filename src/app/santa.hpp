#pragma once
class santa : public object {
public:
  inline santa() { volume = {.radius = 1.4f, .scale = {1, 1, 1}}; }

  inline auto update(const frame_ctx &fc) -> bool override {
    if (object::update(fc)) {
      return true;
    }
    if (!state) {
      return false;
    }
    // update using signals state
    glm::vec3 &v = physics_nxt.velocity;
    v = {0, 0, 0};
    int keys = state->keys;
    if (keys != 0) {
      // wasd keys
      if (keys & 1) {
        v.z = -1;
      }
      if (keys & 2) {
        v.x = -1;
      }
      if (keys & 4) {
        v.z = +1;
      }
      if (keys & 8) {
        v.x = +1;
      }
      v *= 10.0f;
    }

    // std::cout << name << " pos: " << glm::to_string(physics.position) <<
    // "\n";
    return false;
  }

  inline void on_collision(object *o, const frame_ctx &fc) override {
    printf("%s collision with %s\n", name.c_str(), o->name.c_str());
  }
};
