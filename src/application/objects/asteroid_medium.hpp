#pragma once
// reviewed: 2024-01-04
// reviewed: 2024-01-08

// include order relevant
#include "../configuration.hpp"
//
#include "asteroid_small.hpp"
//
#include "../utils.hpp"

class asteroid_medium final : public object {
public:
  inline asteroid_medium() {
    name = "asteroid_medium";
    if (debug_multiplayer) {
      ++counter;
      name.append(1, '_').append(std::to_string(counter));
      printf("%lu: %lu: create %s\n", frame_context.frame_num, frame_context.ms,
             name.c_str());
    }
    glob_ix(glob_ix_asteroid_medium);
    scale = vec3{asteroid_medium_scale};
    bounding_radius = globs.at(glob_ix()).bounding_radius * scale.x;
    mass = 1000;
    collision_bits = cb_asteroid;
    collision_mask = cb_hero_bullet | cb_hero;
    ++asteroids_alive;
  }

  inline ~asteroid_medium() override {
    if (debug_multiplayer) {
      printf("%lu: %lu: free %s\n", frame_context.frame_num, frame_context.ms,
             name.c_str());
    }

    --asteroids_alive;
  }

  inline auto update(object_context &ctx) -> bool override {
    if (!object::update(ctx)) {
      return false;
    }

    game_area_roll(position);

    return true;
  }

  inline auto on_collision(object_context &ctx, object *o) -> bool override {
    if (debug_multiplayer) {
      printf("%lu: %lu: %s collision with %s\n", frame_context.frame_num,
             frame_context.ms, name.c_str(), o->name.c_str());
    }

    score += 50;

    for (uint32_t i = 0; i < asteroid_medium_split; ++i) {
      asteroid_small *ast = new (objects.alloc()) asteroid_small{};
      float const rd2 = bounding_radius / 2;
      vec3 const rp = {rnd1(ctx.rand, rd2), 0, rnd1(ctx.rand, rd2)};
      ast->position = position + rp;
      ast->velocity = velocity + rnd2(ctx.rand, asteroid_medium_split_speed) *
                                     normalize(rp);
      ast->angular_velocity =
          vec3{rnd1(ctx.rand, asteroid_medium_split_agl_vel_rnd)};
    }

    return false;
  }
};
