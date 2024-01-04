#pragma once
// reviewed: 2024-01-04

#include <atomic>
#include <glm/glm.hpp>
//
#include "configuration.hpp"
//
#include "objects/power_up.hpp"
//
inline void game_area_roll(glm::vec3 &position) {
  if (position.x < game_area_min_x or position.x > game_area_max_x) {
    position.x = -position.x;
  }
  if (position.y < game_area_min_y or position.y > game_area_max_y) {
    position.y = -position.y;
  }
  if (position.z < game_area_min_z or position.z > game_area_max_z) {
    position.z = -position.z;
  }
}

inline auto is_outside_game_area(glm::vec3 const &position) -> bool {
  return position.x < game_area_min_x or position.x > game_area_max_x or
         position.y < game_area_min_y or position.y > game_area_max_y or
         position.z < game_area_min_z or position.z > game_area_max_z;
}

inline void power_up_by_chance(glm::vec3 const &position) {
  if (rand() % power_up_chance_rem) {
    return;
  }

  power_up *obj = new (objects.alloc()) power_up{};
  obj->position = position;
  obj->angular_velocity.y = radians(90.0f);
}

inline auto rnd1(float const plus_minus_range) -> float {
  return float(rand()) / float(RAND_MAX) * plus_minus_range -
         plus_minus_range / 2;
}

inline auto rnd2(float const zero_to_range) -> float {
  return float(rand()) / float(RAND_MAX) * zero_to_range;
}