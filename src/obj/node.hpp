#pragma once
// reviewed: 2023-12-22
namespace glos {
class node final {
public:
  int glo_ix = 0;
  glm::mat4 Mmw{};     // model -> world matrix
  glm::vec3 Mmw_pos{}; // position of current Mmw matrix
  glm::vec3 Mmw_agl{}; // angle of current Mmw matrix
  glm::vec3 Mmw_scl{}; // scale of current Mmw matrix
};
} // namespace glos
