#pragma once

#include "../configuration.hpp"
//
#include "asteroid_small.hpp"
//
#include "../utils.hpp"

class asteroid_medium : public object {
public:
  inline asteroid_medium() {
    name = "asteroid_medium";
    glob_ix = glob_ix_asteroid_medium;
    scale = {1, 1, 1};
    glob const &g = globs.at(glob_ix);
    radius = g.bounding_radius * 1; // r * scale
    mass = 1;
    collision_bits = cb_asteroid;
    collision_mask = cb_hero_bullet | cb_hero;
    asteroids_alive++;
  }

  inline ~asteroid_medium() { asteroids_alive--; }

  inline auto update() -> bool override {
    assert(not is_dead());

    if (object::update()) {
      return true;
    }

    game_area_roll(position);

    return false;
  }

  inline auto on_collision(object *o) -> bool override {
    assert(not is_dead());
    printf("%u: %s collision with %s\n", frame_context.tick, name.c_str(),
           o->name.c_str());

    for (int i = 0; i < asteroid_medium_split; i++) {
      asteroid_small *ast = new (objects.alloc()) asteroid_small{};
      ast->position = position;

      constexpr int v = asteroid_medium_split_speed;
      ast->velocity = velocity + vec3(float(rand() % v - v / 2), 0,
                                      float(rand() % v - v / 2));
      constexpr int r = asteroid_medium_split_angular_vel_deg;
      ast->angular_velocity = vec3(radians(float(rand() % r - r / 2)));
    }
    return true;
  }
};
