#pragma once

#include "bullet.hpp"

class ship : public object {
  unsigned ready_to_fire_at_ms = 0;

public:
  inline ship() {
    name = "hero";
    mass = 150;
    glob_ix = glob_ship_ix;
    scale = {1, 1, 1};
    glob const &g = globs.at(glob_ix);
    radius = g.bounding_radius * 1; // r * scale
    collision_bits = cb_hero;
    collision_mask = cb_asteroid;
  }

  inline auto update(frame_context const &fc) -> bool override {
    assert(not is_dead());

    if (object::update(fc)) {
      return true;
    }

    angular_velocity = {0, 0, 0};
    const uint64_t keys = net_state->keys;
    if (keys != 0) {
      // wasd keys
      if (keys & 1) {
        velocity += 6.0f * vec3{-sin(angle.y), 0, -cos(angle.y)} * fc.dt;
      }
      if (keys & 2) {
        angular_velocity.y = radians(90.0f);
      }
      if (keys & 8) {
        angular_velocity.y = radians(-90.0f);
      }
      if (keys & 64) { // j
        if (ready_to_fire_at_ms < fc.ms) {
          bullet *o = new (objects.alloc()) bullet{};
          o->angle = angle;
          mat4 M = get_updated_Mmw();
          o->position = position;
          o->velocity = -17.0f * vec3(M[2]);
          ready_to_fire_at_ms = fc.ms + 1000;
        }
      }
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
