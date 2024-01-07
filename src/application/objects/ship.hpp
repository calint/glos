#pragma once
// reviewed: 2024-01-04

#include "bullet.hpp"

class ship : public object {
public:
  inline ship() {
    name = "hero_";
    if (debug_multiplayer) {
      ++counter;
      name.append(std::to_string(counter));
      printf("%lu: %lu: create %s\n", frame_context.frame_num, frame_context.ms,
             name.c_str());
    }
    glob_ix = glob_ix_ship;
    scale = {1, 1, 1};
    radius = globs.at(glob_ix).bounding_radius * scale.x;
    collision_bits = cb_hero;
    collision_mask = cb_asteroid | cb_power_up;
    mass = 150;
  }

  inline ~ship() override {
    if (debug_multiplayer) {
      printf("%lu: %lu: free %s\n", frame_context.frame_num, frame_context.ms,
             name.c_str());
    }
  }

  inline auto update() -> bool override {
    if (not object::update()) {
      return false;
    }

    game_area_roll(position);

    angular_velocity = {0, 0, 0};
    glob_ix = glob_ix_ship;

    uint64_t const keys = net_state->keys;
    if (not keys) {
      return true;
    }

    // handle ship controls
    if (keys & key_w) {
      velocity +=
          ship_speed * vec3{-sin(angle.y), 0, -cos(angle.y)} * frame_context.dt;
      glob_ix = glob_ix_ship_engine_on;
    }
    if (keys & key_a) {
      angular_velocity.y = ship_turn_rate;
    }
    if (keys & key_d) {
      angular_velocity.y = -ship_turn_rate;
    }
    if (keys & key_j) {
      fire();
    }

    return true;
  }

  inline auto on_collision(object *o) -> bool override {
    if (debug_multiplayer) {
      printf("%lu: %lu: %s collision with %s\n", frame_context.frame_num,
             frame_context.ms, name.c_str(), o->name.c_str());
    }
    if (typeid(*o) == typeid(power_up)) {
      if (bullet_level == 0) {
        bullet_fire_rate_ms /= 2;
        if (bullet_fire_rate_ms < 100) {
          bullet_fire_rate_ms = ship_bullet_fire_interval_ms;
          ++bullet_level;
        }
      } else if (bullet_level == 1) {
        bullet_fire_rate_ms /= 2;
        if (bullet_fire_rate_ms < 100) {
          bullet_fire_rate_ms = 100;
        }
      }
    } else if (o->collision_bits & cb_asteroid) {
      angle.y += radians(rnd1(45));

      fragment *frg = new (objects.alloc()) fragment{};
      frg->position = o->position;
      frg->angular_velocity = vec3(radians(rnd1(bullet_fragment_agl_vel_rnd)));
      frg->death_time_ms = frame_context.ms + 500;
    }

    return true;
  }

private:
  inline void fire() {
    if (ready_to_fire_at_ms > frame_context.ms) {
      return;
    }

    mat4 const &M = get_updated_Mmw();
    vec3 const forward_vec = -normalize(vec3{M[2]});
    // note. M[2] is third column: z-axis
    // note. forward for object model space is negative z

    switch (bullet_level) {
    case 0: {
      bullet *o = new (objects.alloc()) bullet{};
      o->position = position + forward_vec;
      o->angle = angle;
      // note. forward is in negative z-axis direction
      o->velocity = ship_bullet_speed * forward_vec;
      ready_to_fire_at_ms = frame_context.ms + bullet_fire_rate_ms;
      break;
    }
    case 1: {
      for (unsigned i = 0; i < ship_bullet_level_1_fire_count; ++i) {
        bullet *o = new (objects.alloc()) bullet{};
        o->position = position + forward_vec;
        o->angle = angle;
        o->velocity = ship_bullet_speed * forward_vec;
        constexpr float sp = ship_bullet_spread;
        o->velocity.x += float(rnd1(sp));
        o->velocity.z += float(rnd1(sp));
      }
      ready_to_fire_at_ms = frame_context.ms + bullet_fire_rate_ms;
      break;
    }
    }
  }

  uint64_t ready_to_fire_at_ms = 0;
  uint64_t bullet_fire_rate_ms = ship_bullet_fire_interval_ms;
  uint8_t bullet_level = 0;
};
