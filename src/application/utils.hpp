#pragma once

#include <atomic>
#include <glm/glm.hpp>
//
#include "configuration.hpp"
//
#include "objects/power_up.hpp"
//
std::atomic<int> asteroids_alive{0};

inline void game_area_roll(glm::vec3 &position) {
  if (position.x < game_area_min_x) {
    position.x = -position.x;
  } else if (position.x > game_area_max_x) {
    position.x = -position.x;
  } else if (position.z < game_area_min_z) {
    position.z = -position.z;
  } else if (position.z > game_area_max_z) {
    position.z = -position.z;
  }
}

inline void power_up_by_chance(const glm::vec3 &position) {
  if (rand() % power_up_chance_rem) {
    return;
  }

  power_up *obj = new (objects.alloc()) power_up{};
  obj->position = position;
  obj->angular_velocity.y = radians(90.0f);
}