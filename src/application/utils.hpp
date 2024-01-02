#pragma once

#include "configuration.hpp"
#include <atomic>
#include <glm/glm.hpp>

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