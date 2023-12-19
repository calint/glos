#pragma once

struct cell;
#define grid_ifc_overlaps 0
typedef struct grid_ifc {
  struct cell *main_cell;
  bits bits; // 1: overlaps
  bits collide_mask;
  bits collide_bits;
  unsigned tick;
  dynp checked_collisions_list;
} grid_ifc;

#define grid_ifc_def                                                           \
  { NULL, 0, 0, 0, 0, dynp_def }
