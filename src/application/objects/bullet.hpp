#pragma once
// reviewed: 2024-01-04
// reviewed: 2024-01-08

#include "fragment.hpp"

class bullet final : public object {
public:
  inline bullet() {
    name = "bullet";
    if (debug_multiplayer) {
      ++counter;
      name.append(1, '_').append(std::to_string(counter));
      printf("%lu: %lu: create %s\n", frame_context.frame_num, frame_context.ms,
             name.c_str());
    }
    glob_ix(glob_ix_bullet);
    scale = {1, 1, 1};
    bounding_radius = globs.at(glob_ix()).bounding_radius * scale.x;
    mass = 5;
    collision_bits = cb_hero_bullet;
    collision_mask = cb_asteroid;
  }

  inline ~bullet() override {
    if (debug_multiplayer) {
      printf("%lu: %lu: free %s\n", frame_context.frame_num, frame_context.ms,
             name.c_str());
    }
  }

  inline auto update() -> bool override {
    if (!object::update()) {
      return false;
    }

    if (is_outside_game_area(position)) {
      return false;
    }

    return true;
  }

  inline auto on_collision(object *o) -> bool override {
    if (debug_multiplayer) {
      printf("%lu: %lu: %s collision with %s\n", frame_context.frame_num,
             frame_context.ms, name.c_str(), o->name.c_str());
    }

    fragment *frg = new (objects.alloc()) fragment{};
    frg->position = position;
    frg->angular_velocity = vec3{rnd1(bullet_fragment_agl_vel_rnd)};
    frg->death_time_ms = frame_context.ms + 500;

    return false;
  }
};
