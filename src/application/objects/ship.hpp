#pragma once

#include "bullet.hpp"

class ship : public object {
  unsigned ready_to_fire_at_ms = 0;
  uint32_t glob_engine_on_ix = 0;
  uint32_t glob_engine_off_ix = 0;
  unsigned bullet_fire_rate_ms = ship_bullet_fire_intervall_ms;

public:
  inline ship() {
    name = "hero";
    mass = 150;
    glob_engine_off_ix = glob_ix = glob_ix_ship;
    glob_engine_on_ix = glob_ix_ship_engine_on;
    scale = {1, 1, 1};
    glob const &g = globs.at(glob_ix);
    radius = g.bounding_radius * 1; // r * scale
    collision_bits = cb_hero;
    collision_mask = cb_asteroid | cb_power_up;
  }

  inline auto update() -> bool override {
    assert(not is_dead());

    if (object::update()) {
      return true;
    }

    game_area_roll(position);

    angular_velocity = {0, 0, 0};
    const uint64_t keys = net_state->keys;
    glob_ix = glob_engine_off_ix;
    if (keys != 0) {
      // wasd keys
      if (keys & 1) {
        velocity += ship_speed * vec3{-sin(angle.y), 0, -cos(angle.y)} *
                    frame_context.dt;
        glob_ix = glob_engine_on_ix;
      }
      if (keys & 2) {
        angular_velocity.y = radians(ship_turn_rate_deg);
      }
      if (keys & 8) {
        angular_velocity.y = radians(-ship_turn_rate_deg);
      }
      if (keys & 64) { // j
        if (ready_to_fire_at_ms < frame_context.ms) {
          bullet *o = new (objects.alloc()) bullet{};
          o->angle = angle;
          mat4 M = get_updated_Mmw();
          o->position = position;
          o->velocity = -ship_bullet_speed * vec3(M[2]);
          ready_to_fire_at_ms = frame_context.ms + bullet_fire_rate_ms;
        }
      }
    }

    return false;
  }

  inline auto on_collision(object *o) -> bool override {

    assert(not is_dead());

    printf("%u: %s collision with %s\n", frame_context.frame_num, name.c_str(),
           o->name.c_str());

    if (typeid(*o) == typeid(power_up)) {
      bullet_fire_rate_ms /= 2;
      if (bullet_fire_rate_ms < 200) {
        bullet_fire_rate_ms = 200;
      }
    }

    return false;
  }
};
