#pragma once
// reviewed: 2024-01-04

#include "fragment.hpp"

class bullet : public object {
public:
  inline bullet() {
    ++counter;
    name = "bullet_";
    name.append(std::to_string(counter));
    printf("%u: %lu: create %s\n", frame_context.frame_num, frame_context.ms,
           name.c_str());
    glob_ix = glob_ix_bullet;
    scale = {1, 1, 1};
    radius = globs.at(glob_ix).bounding_radius * scale.x;
    collision_bits = cb_hero_bullet;
    collision_mask = cb_asteroid;
    mass = 5;
  }

  inline ~bullet() override {
    printf("%u: %lu: free %s\n", frame_context.frame_num, frame_context.ms,
           name.c_str());
  }

  inline auto update() -> bool override {
    if (object::update()) {
      return true;
    }

    if (is_outside_game_area(position)) {
      return true;
    }

    return false;
  }

  inline auto on_collision(object *o) -> bool override {
    printf("%u: %lu: %s collision with %s\n", frame_context.frame_num,
           frame_context.ms, name.c_str(), o->name.c_str());

    fragment *frg = new (objects.alloc()) fragment{};
    frg->position = position;
    frg->angular_velocity = vec3(radians(rnd1(bullet_fragment_agl_vel_rnd)));
    frg->death_time_ms = frame_context.ms + 500;

    return true;
  }
};
