#pragma once

#include "../configuration.hpp"
//
#include "../utils.hpp"

class asteroid_small : public object {
public:
  inline asteroid_small() {
    name = "asteroid_small";
    glob_ix = glob_ix_asteroid_small;
    scale = {0.5f, 0.5f, 0.5f};
    glob const &g = globs.at(glob_ix);
    radius = g.bounding_radius * scale.x;
    mass = 1;
    collision_bits = cb_asteroid;
    collision_mask = cb_hero_bullet | cb_hero;
    asteroids_alive++;
  }

  inline ~asteroid_small() { asteroids_alive--; }

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

    power_up_by_chance(position);

    return true;
  }
};
