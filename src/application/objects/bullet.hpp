#pragma once

#include "fragment.hpp"

class bullet : public object {
public:
  inline bullet() {
    name = "bullet";
    glob_ix = glob_ix_bullet;
    scale = {1, 1, 1};
    glob const &g = globs.at(glob_ix);
    radius = g.bounding_radius * scale.x;
    mass = 1;
    collision_bits = cb_hero_bullet;
    collision_mask = cb_asteroid;
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
    printf("%u: %s collision with %s\n", frame_context.frame_num, name.c_str(),
           o->name.c_str());

    fragment *frg = new (objects.alloc()) fragment{};
    frg->position = position;
    frg->angular_velocity = vec3(radians(float(rand() % 360 - 180)));
    frg->death_time_ms = frame_context.ms + 500;
    frg->scale = {0.5f, 0.5f, 0.5f};
    frg->radius = frg->radius * frg->scale.x;

    return true;
  }
};
