#pragma once
// reviewed: 2024-01-04

#include "../configuration.hpp"
//
#include "asteroid_small.hpp"
//
#include "../utils.hpp"

class asteroid_medium : public object {
public:
  inline asteroid_medium() {
    name = "asteroid_medium_";
    if (debug_multiplayer) {
      ++counter;
      name.append(std::to_string(counter));
      printf("%lu: %lu: create %s\n", frame_context.frame_num, frame_context.ms,
             name.c_str());
    }
    glob_ix = glob_ix_asteroid_medium;
    scale = vec3{asteroid_medium_scale};
    radius = globs.at(glob_ix).bounding_radius * scale.x;
    collision_bits = cb_asteroid;
    collision_mask = cb_hero_bullet | cb_hero;
    mass = 1000;
    ++asteroids_alive;
  }

  inline ~asteroid_medium() override {
    if (debug_multiplayer) {
      printf("%lu: %lu: free %s\n", frame_context.frame_num, frame_context.ms,
             name.c_str());
    }

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
    if (debug_multiplayer) {
      printf("%lu: %lu: %s collision with %s\n", frame_context.frame_num,
             frame_context.ms, name.c_str(), o->name.c_str());
    }

    score += 50;

    for (unsigned i = 0; i < asteroid_medium_split; ++i) {
      asteroid_small *ast = new (objects.alloc()) asteroid_small{};
      const float rd2 = radius / 2;
      const vec3 rp = vec3(rnd1(rd2), 0, rnd1(rd2));
      ast->position = position + rp;
      ast->velocity =
          velocity + rnd2(asteroid_medium_split_speed) * normalize(rp);
      ast->angular_velocity =
          vec3(radians(rnd1(asteroid_medium_split_agl_vel_deg)));
    }

    return true;
  }
};
