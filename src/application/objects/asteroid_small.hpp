#pragma once
// reviewed: 2024-01-04

#include "../configuration.hpp"
//
#include "../utils.hpp"

class asteroid_small : public object {
public:
  inline asteroid_small() {
    name = "asteroid_small";
    glob_ix = glob_ix_asteroid_small;
    scale = vec3{asteroid_small_scale};
    radius = globs.at(glob_ix).bounding_radius * scale.x;
    collision_bits = cb_asteroid;
    collision_mask = cb_hero_bullet | cb_hero;
    mass = 500;
    ++asteroids_alive;
  }

  inline ~asteroid_small() { asteroids_alive--; }

  inline auto update() -> bool override {
    if (object::update()) {
      return true;
    }

    game_area_roll(position);

    return false;
  }

  inline auto on_collision(object *o) -> bool override {
    printf("%u: %s collision with %s\n", frame_context.frame_num, name.c_str(),
           o->name.c_str());

    score += 100;

    power_up_by_chance(position);

    return true;
  }
};
