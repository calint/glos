#pragma once
// reviewed: 2024-01-04
// reviewed: 2024-01-08

// include order relevant
#include "../configuration.hpp"
//
#include "../utils.hpp"

class asteroid_small final : public object {
public:
  inline asteroid_small() {
    name = "asteroid_small";
    if (debug_multiplayer) {
      ++counter;
      name.append(1, '_').append(std::to_string(counter));
      printf("%lu: %lu: create %s\n", frame_context.frame_num, frame_context.ms,
             name.c_str());
    }
    glob_ix(glob_ix_asteroid_small);
    scale = vec3{asteroid_small_scale};
    bounding_radius = globs.at(glob_ix()).bounding_radius * scale.x;
    mass = 500;
    collision_bits = cb_asteroid;
    collision_mask = cb_hero_bullet | cb_hero;
    ++asteroids_alive;
  }

  inline ~asteroid_small() override {
    if (debug_multiplayer) {
      printf("%lu: %lu: free %s\n", frame_context.frame_num, frame_context.ms,
             name.c_str());
    }

    --asteroids_alive;
  }

  inline auto update() -> bool override {
    if (!object::update()) {
      return false;
    }

    game_area_roll(position);

    return true;
  }

  inline auto on_collision(object *o) -> bool override {
    if (debug_multiplayer) {
      printf("%lu: %lu: %s collision with %s\n", frame_context.frame_num,
             frame_context.ms, name.c_str(), o->name.c_str());
    }

    score += 100;

    power_up_by_chance(position);

    return false;
  }
};
