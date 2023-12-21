#pragma once
//--------------------------------------------------------------------- object
#include "../grid/grid-ifc.h"
#include "../lib.h"
#include "bounding_volume.hpp"
#include "node.hpp"
#include "physics.hpp"
#include "vtbl.hpp"
#include <string>
//------------------------------------------------------------------------ def
#define object_part_cap 5
#define parts_def                                                              \
  { 0, 0, 0, 0, 0 }
class part;
class object {
public:
  unsigned update_tick = 0;
  unsigned draw_tick = 0;
  node node{};
  bounding_volume bounding_volume{};
  physics physics_prv{}; // physics state from previous frame
  physics physics_nxt{}; // physics state for next frame
  physics physics{};
  grid_ifc grid_ifc{};
  vtbl vtbl{};
  type type{};
  bits *alloc_bits_ptr{};
  void *part[object_part_cap]{};
  std::string name = "";

  inline void update(framectx *fc) {
    // if matrix is flagged valid and position or angle changed invalidate
    // matrix
    if (node.Mmw_is_valid and
        (!vec3_equals(&physics_prv.position, &physics.position) or
         !vec3_equals(&physics_prv.angle, &physics.angle))) {
      node.Mmw_is_valid = 0;
    }

    physics_prv = physics;
    physics = physics_nxt;

    const float dt = fc->dt;
    vec3_inc_with_vec3_over_dt(&physics.position, &physics.velocity, dt);
    vec3_inc_with_vec3_over_dt(&physics.angle, &physics.angular_velocity, dt);
    physics_nxt = physics;
  }

  inline const float *get_updated_Mmw() {
    if (node.Mmw_is_valid) {
      return node.Mmw;
    }
    mat4_set_translation(node.Mmw, &physics.position);
    mat4_append_rotation_about_z_axis(node.Mmw, physics.angle.z);
    mat3_scale(node.Mmw, &bounding_volume.scale);
    node.Mmw_is_valid = 1;
    return node.Mmw;
  }
};
//---------------------------------------------------------------------- -----
static object object_def = {
    .update_tick = 0,
    .draw_tick = 0,
    .node{},
    .bounding_volume{},
    .physics_prv{},
    .physics_nxt{},
    .physics{},
    .grid_ifc = grid_ifc_def,
    .vtbl{},
    .type = type_def,
    .alloc_bits_ptr = 0,
    .part = parts_def,
    .name = "",
};
//----------------------------------------------------------------------------
inline static void _object_render_glo_(object *o, framectx *fc) {
  if (!o->node.glo) {
    return;
  }
  const float *mtx = o->get_updated_Mmw();
  glo_render(o->node.glo, mtx);
}
//----------------------------------------------------------------------------
