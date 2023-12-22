#pragma once
// reviewed: 2023-12-22

class node final {
public:
  glm::mat4 Mmw{}; // model -> world matrix
  glm::mat3 Nmw{}; // model -> world normals rotation matrix
  glo *glo{};
};
