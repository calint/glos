#pragma once
// reviewed: 2024-01-04

#include "../configuration.hpp"
//
#include "asteroid_medium.hpp"
//
#include "../utils.hpp"

class asteroid_large : public object {
public:
  inline asteroid_large() {
    ++counter;
    name = "asteroid_large_";
    name.append(std::to_string(counter));
    printf("%u: %lu: create %s\n", frame_context.frame_num, frame_context.ms,
           name.c_str());
    scale = vec3{asteroid_large_scale};
    glob_ix = glob_ix_asteroid_large;
    radius = globs.at(glob_ix).bounding_radius * scale.x;
    collision_bits = cb_asteroid;
    collision_mask = cb_hero_bullet | cb_hero;
    mass = 1500;
    ++asteroids_alive;
  }

  inline ~asteroid_large() override {
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

    score += 10;

    for (unsigned i = 0; i < asteroid_large_split; ++i) {
      asteroid_medium *ast = new (objects.alloc()) asteroid_medium{};
      const float rd2 = radius / 2;
      const vec3 rp = vec3(rnd1(rd2), 0, rnd1(rd2));
      ast->position = position + rp;
      ast->velocity =
          velocity + rnd2(asteroid_large_split_speed) * normalize(rp);
      ast->angular_velocity =
          vec3(radians(rnd1(asteroid_large_split_agl_vel_deg)));
    }

    return true;
  }
};
