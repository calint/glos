#pragma once
// reviewed: 2023-12-22
namespace glos {
class node final {
public:
  glm::mat4 Mmw{}; // model -> world matrix
  int glo_ix = 0;
};
} // namespace glos
