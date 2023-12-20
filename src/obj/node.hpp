#pragma once
#include "../lib.h"
class node final {
public:
  mat4 Mmw{};
  mat3 Mnmw{};
  bool Mmw_is_valid = false;
  glo *glo{};
};

// #define node_def                                                               \
//   { mat4_identity_, mat3_identity_, 0, 0 }
