#pragma once
// reviewed: 2024-01-04
// reviewed: 2024-01-08

// include order relevant
#include "../configuration.hpp"
//
#include "asteroid_medium.hpp"
//
#include "../utils.hpp"

class asteroid_large final : public object {
public:
  inline asteroid_large() {
    name = "asteroid_large";
    if (debug_multiplayer) {
      ++counter;
      name.append(1, '_').append(std::to_string(counter));
      printf("%lu: %lu: create %s\n", frame_context.frame_num, frame_context.ms,
             name.c_str());
    }
    glob_ix(glob_ix_asteroid_large);
    scale = vec3{asteroid_large_scale};
    bounding_radius = globs.at(glob_ix()).bounding_radius * scale.x;
    mass = 1500;
    collision_bits = cb_asteroid;
    collision_mask = cb_hero_bullet | cb_hero;
    angular_velocity.y = rnd1(asteroid_large_agl_vel_rnd);
    ++asteroids_alive;
  }

  inline ~asteroid_large() override {
    if (debug_multiplayer) {
      printf("%lu: %lu: free %s\n", frame_context.frame_num, frame_context.ms,
             name.c_str());
    }

    --asteroids_alive;
  }

  inline auto update() -> bool override {
    if (not object::update()) {
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

    score += 10;

    for (unsigned i = 0; i < asteroid_large_split; ++i) {
      asteroid_medium *ast = new (objects.alloc()) asteroid_medium{};
      float const rd2 = bounding_radius / 2;
      vec3 const rp = {rnd1(rd2), 0, rnd1(rd2)};
      ast->position = position + rp;
      ast->velocity =
          velocity + rnd2(asteroid_large_split_speed) * normalize(rp);
      ast->angular_velocity = vec3{rnd1(asteroid_large_split_agl_vel_rnd)};
    }

    return false;
  }
};
