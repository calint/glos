#pragma once

class santa : public object {
  unsigned health = 0;

public:
  inline santa() {
    glob_ix = glob_santa_ix;
    scale = {1, 1, 1};
    glob const &g = globs.at(glob_ix);
    radius = g.bounding_radius * 1; // r * scale
    collision_bits = cb_hero;
    collision_mask = cb_hero;
    health = unsigned(rand()) % 50001;
  }

  // inline ~santa() { printf("%s destructor\n", name.c_str()); }

  inline auto update(frame_context const &fc) -> bool override {
    assert(not is_dead());

    if (object::update(fc)) {
      return true;
    }

    if (position.y < radius) {
      position.y = radius;
      velocity.y = 0;
    }

    if (!net_state) {
      return false;
    }

    // update using signals state
    glm::vec3 &v = velocity;
    v = {0, 0, 0};
    uint64_t keys = net_state->keys;
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

    return false;
  }

  inline auto on_collision(object *o, frame_context const &fc)
      -> bool override {
    assert(not is_dead());
    // printf("%s collision with %s\n", name.c_str(), o->name.c_str());
    health--;
    return health == 0;
  }
};