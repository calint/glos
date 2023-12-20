#pragma once
//--------------------------------------------------------------------- object
#include "../grid/grid-ifc.h"
#include "../lib.h"
#include "bounding_volume.h"
#include "node.h"
#include "physics.h"
#include "vtbl.h"
//------------------------------------------------------------------------ def
#define object_part_cap 5
#define parts_def                                                              \
  { 0, 0, 0, 0, 0 }
typedef struct part part;
typedef struct object {
  unsigned update_tick;
  unsigned draw_tick;
  node node;
  bounding_volume bounding_volume;
  physics physics_prv; // physics state from previous frame
  physics physics_nxt; // physics state for next frame
  physics physics;
  grid_ifc grid_ifc;
  vtbl vtbl;
  type type;
  bits *alloc_bits_ptr;
  void *part[object_part_cap];
  str name;
} object;
//---------------------------------------------------------------------- -----
static object object_def = {
    .update_tick = 0,
    .draw_tick = 0,
    .node = node_def,
    .bounding_volume = bounding_volume_def,
    .physics_prv = physics_def,
    .physics_nxt = physics_def,
    .physics = physics_def,
    .grid_ifc = grid_ifc_def,
    .vtbl = vtbl_def,
    .type = type_def,
    .alloc_bits_ptr = 0,
    .part = parts_def,
    .name = str_def,
};
//----------------------------------------------------------------------------
inline static void object_update(object *o, framectx *fc) {
  if (o->node.Mmw_is_valid &&
      ( // if matrix is flagged valid and
          !vec3_equals(&o->physics_prv.position,
                       &o->physics.position) || // if position
          !vec3_equals(&o->physics_prv.angle,
                       &o->physics.angle))) { // or angle changed
    o->node.Mmw_is_valid = 0;                 // invalidate matrix
  }

  o->physics_prv = o->physics;
  o->physics = o->physics_nxt;

  physics *p = &o->physics;
  const dt dt = fc->dt;
  vec3_inc_with_vec3_over_dt(&p->position, &p->velocity, dt);
  vec3_inc_with_vec3_over_dt(&p->angle, &p->angular_velocity, dt);
  o->physics_nxt = o->physics;
}
inline static const float *object_get_updated_Mmw(object *o) {
  if (o->node.Mmw_is_valid)
    return o->node.Mmw;

  mat4_set_translation(o->node.Mmw, &o->physics.position);

  mat4_append_rotation_about_z_axis(o->node.Mmw, o->physics.angle.z);

  mat3_scale(o->node.Mmw, &o->bounding_volume.scale);

  o->node.Mmw_is_valid = 1;

  // printf("\n-- %s\n",o->name.data);
  // printf("%f %f %f %f\n",o->n.Mmw[0],o->n.Mmw[1],o->n.Mmw[2],o->n.Mmw[3]);
  // printf("%f %f %f %f\n",o->n.Mmw[4],o->n.Mmw[5],o->n.Mmw[6],o->n.Mmw[7]);
  // printf("%f %f %f %f\n",o->n.Mmw[8],o->n.Mmw[9],o->n.Mmw[10],o->n.Mmw[11]);
  // printf("\npos: %f, %f, %f\n",o->p.p.x,o->p.p.y,o->p.p.z);
  return o->node.Mmw;
}
inline static void _object_render_glo_(object *o, framectx *fc) {
  if (!o->node.glo)
    return;

  const float *f = object_get_updated_Mmw(o);
  glo_render(o->node.glo, f);
}
//----------------------------------------------------------------------------
