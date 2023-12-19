#pragma once
#include "../lib.h"
typedef struct node {
  mat4 Mmw;
  mat3 Mnmw;
  unsigned Mmw_is_valid;
  glo *glo;
} node;
#define node_def                                                               \
  { mat4_identity_, mat3_identity_, 0, 0 }
