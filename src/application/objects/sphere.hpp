#pragma once

class sphere : public object {
public:
  inline sphere() {
    glob_ix = glob_sphere_ix;
    scale = {1, 1, 1};
    glob const &g = globs.at(glob_ix);
    radius = g.bounding_radius * 1; // r * scale
    mass = 1;
    collision_bits = cb_hero;
    collision_mask = cb_hero;
  }

  inline auto update(frame_context const &fc) -> bool override {
    assert(not is_dead());

    if (object::update(fc)) {
      return true;
    }

    if (position.y < radius) {
      position.y = radius;
      velocity.y = 0;
    }

    if (position.x < -80) {
      position.x = -80;
      velocity.x = -velocity.x;
    }

    if (position.x > 80) {
      position.x = 80;
      velocity.x = -velocity.x;
    }

    if (position.z < -80) {
      position.z = -80;
      velocity.z = -velocity.z;
    }

    if (position.z > 80) {
      position.z = 80;
      velocity.z = -velocity.z;
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
    unsigned keys = net_state->keys;
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
      v *= 2.0f;
    }
    return false;
  }

  inline auto on_collision(object *o, frame_context const &fc)
      -> bool override {
    assert(not is_dead());
    // printf("%u: %s collision with %s\n", fc.tick, name.c_str(),
    //        o->name.c_str());
    return false;
  }
};
