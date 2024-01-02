#pragma once

#include "../configuration.hpp"
//
#include "asteroid_medium.hpp"
//
#include "../utils.hpp"

class asteroid_large : public object {
public:
  inline asteroid_large() {
    name = "asteroid_large";
    scale = vec3{asteroid_large_scale};
    glob_ix = glob_ix_asteroid_large;
    radius = globs.at(glob_ix).bounding_radius * scale.x;
    collision_bits = cb_asteroid;
    collision_mask = cb_hero_bullet | cb_hero;
    mass = 1500;
    asteroids_alive++;
  }

  inline ~asteroid_large() { asteroids_alive--; }

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

    for (int i = 0; i < asteroid_large_split; i++) {
      asteroid_medium *ast = new (objects.alloc()) asteroid_medium{};
      const vec3 rp = vec3(rnd1(radius / 2), 0, rnd1(radius / 2));
      ast->position = position + rp;
      ast->velocity = velocity + rnd2(asteroid_large_split_speed) * normalize(rp);
      ast->angular_velocity =
          vec3(radians(rnd1(asteroid_large_split_agl_vel_deg)));
    }

    return true;
  }
};
