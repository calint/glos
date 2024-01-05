#pragma once
// reviewed: 2024-01-04

#include "../configuration.hpp"
//
#include "../utils.hpp"

class asteroid_small : public object {
public:
  inline asteroid_small() {
    ++counter;
    name = "asteroid_small_";
    name.append(std::to_string(counter));
    printf("%u: %lu: create %s\n", frame_context.frame_num, frame_context.ms,
           name.c_str());
    glob_ix = glob_ix_asteroid_small;
    scale = vec3{asteroid_small_scale};
    radius = globs.at(glob_ix).bounding_radius * scale.x;
    collision_bits = cb_asteroid;
    collision_mask = cb_hero_bullet | cb_hero;
    mass = 500;
    ++asteroids_alive;
  }

  inline ~asteroid_small() override {
    printf("%u: %lu: free %s\n", frame_context.frame_num, frame_context.ms,
           name.c_str());
    --asteroids_alive;
  }

  inline auto update() -> bool override {
    if (object::update()) {
      return true;
    }

    game_area_roll(position);

    return false;
  }

  inline auto on_collision(object *o) -> bool override {
    printf("%u: %lu: %s collision with %s\n", frame_context.frame_num,
           frame_context.ms, name.c_str(), o->name.c_str());

    score += 100;

    power_up_by_chance(position);

    return true;
  }
};
