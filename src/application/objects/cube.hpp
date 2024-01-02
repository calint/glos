#pragma once

class cube : public object {
public:
  inline cube() {
    glob_ix = glob_cube_ix;
    scale = {2, 1, 1};
    glob const &g = globs.at(glob_ix);
    radius = g.bounding_radius * 2; // r * scale
    is_sphere = false;
    collision_bits = cb_hero;
    collision_mask = cb_hero;
  }

  inline auto update(frame_context const &fc) -> bool override {
    assert(not is_dead());

    if (object::update(fc)) {
      return true;
    }

    if (!net_state) {
      return false;
    }

    // update using signals state
    glm::vec3 &v = velocity;
    glm::vec3 &a = angle;
    glm::vec3 &av = angular_velocity;
    v = {0, 0, 0};
    av = {0, 0, 0};
    uint64_t keys = net_state->keys;
    if (keys != 0) {
      // wasd keys
      if (keys & 1) {
        v = -glm::vec3{glm::sin(a.y), 0, glm::cos(a.y)};
      }
      if (keys & 2) {
        av.y = 1;
      }
      if (keys & 4) {
        v = {glm::sin(a.y), 0, glm::cos(a.y)};
      }
      if (keys & 8) {
        av.y = -1;
      }
      v *= 5.0f;
    }

    return false;
  }

  inline auto on_collision(object *o, frame_context const &fc)
      -> bool override {
    assert(not is_dead());
    printf("%u: %s collision with %s\n", fc.tick, name.c_str(),
           o->name.c_str());
    return false;
  }
};
