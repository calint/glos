#pragma once

#include "bullet.hpp"

class ship : public object {
  uint32_t ready_to_fire_at_ms = 0;
  uint32_t bullet_fire_rate_ms = ship_bullet_fire_intervall_ms;
  uint8_t bullet_level = 0;

public:
  inline ship() {
    name = "hero";
    mass = 150;
    glob_ix = glob_ix_ship;
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
    glob_ix = glob_ix_ship;
    if (keys != 0) {
      // wasd keys
      if (keys & glos::key_w) {
        velocity += ship_speed * vec3{-sin(angle.y), 0, -cos(angle.y)} *
                    frame_context.dt;
        glob_ix = glob_ix_ship_engine_on;
      }
      if (keys & glos::key_a) {
        angular_velocity.y = radians(ship_turn_rate_deg);
      }
      if (keys & glos::key_d) {
        angular_velocity.y = radians(-ship_turn_rate_deg);
      }
      if (keys & glos::key_j) {
        if (ready_to_fire_at_ms < frame_context.ms) {
          switch (bullet_level) {
          case 0: {
            bullet *o = new (objects.alloc()) bullet{};
            o->angle = angle;
            mat4 M = get_updated_Mmw();
            o->position = position;
            o->velocity = -ship_bullet_speed * vec3(M[2]);
            ready_to_fire_at_ms = frame_context.ms + bullet_fire_rate_ms;
            break;
          }
          case 1: {
            for (int i = 0; i < ship_bullet_level_1_count; i++) {
              bullet *o = new (objects.alloc()) bullet{};
              o->angle = angle;
              mat4 M = get_updated_Mmw();
              o->position = position;
              o->velocity = -ship_bullet_speed * vec3(M[2]);
              constexpr int ship_bullet_spread = 4;
              constexpr int sp = ship_bullet_spread;
              o->velocity.x += float(rand() % sp - sp / 2);
              o->velocity.z += float(rand() % sp - sp / 2);
            }
            ready_to_fire_at_ms = frame_context.ms + bullet_fire_rate_ms;
            break;
          }
          }
        }
      }
    }
    return false;
  }

  inline auto on_collision(object *o) -> bool override {
    printf("%u: %s collision with %s\n", frame_context.frame_num, name.c_str(),
           o->name.c_str());

    if (typeid(*o) == typeid(power_up)) {
      if (bullet_level == 0) {
        bullet_fire_rate_ms /= 2;
        if (bullet_fire_rate_ms < 100) {
          bullet_fire_rate_ms = ship_bullet_fire_intervall_ms;
          ++bullet_level;
        }
      } else if (bullet_level == 1) {
        bullet_fire_rate_ms /= 2;
        if (bullet_fire_rate_ms < 100) {
          bullet_fire_rate_ms = 100;
        }
      }
    } else if (o->collision_bits & cb_asteroid) {
      angle.y += radians(float(rand() % 45 - 22));
    }

    return false;
  }
};
