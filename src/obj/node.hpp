#pragma once

class node final {
public:
  glm::mat4 Mmw{};
  glm::mat3 Mnmw{};
  bool Mmw_is_valid = false;
  glo *glo{};
};
