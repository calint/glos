#pragma once
//--------------------------------------------------------------------- object
#include "../grid/grid-ifc.hpp"
#include "../lib.h"
#include "volume.hpp"
#include "node.hpp"
#include "physics.hpp"
#include <string>
//------------------------------------------------------------------------ def
class object {
public:
  // char const *name = "";//?? using char const* degrades performance by 40%
  // how? std::string name{}; //?? using std::string degrades performance by 20%
  // how? str name = str_def; char pad; //?? uncommenting degrades performance
  // by 30% how?
  unsigned update_tick = 0;
  unsigned draw_tick = 0;
  node node{};
  volume volume{};
  physics physics_prv{}; // physics state from previous frame
  physics physics_nxt{}; // physics state for next frame
  physics physics{};
  grid_ifc grid_ifc{};
  unsigned *keys_ptr{};

  inline object() {}

  inline virtual ~object() {}

  inline virtual auto update(const frame_ctx &fc) -> bool {
    // if matrix is flagged valid and position or angle changed invalidate
    // matrix
    if (node.Mmw_is_valid and
        (!vec3_equals(&physics_prv.position, &physics.position) or
         !vec3_equals(&physics_prv.angle, &physics.angle))) {
      node.Mmw_is_valid = false;
    }

    physics_prv = physics;
    physics = physics_nxt;

    const float dt = fc.dt;
    vec3_inc_with_vec3_over_dt(&physics_nxt.position, &physics_nxt.velocity,
                               dt);
    vec3_inc_with_vec3_over_dt(&physics_nxt.angle,
                               &physics_nxt.angular_velocity, dt);
    return false;
  }

  inline virtual void render(const frame_ctx &fc) {
    if (!node.glo) {
      return;
    }
    const float *mtx = get_updated_Mmw();
    glo_render(node.glo, mtx);
  }

  inline virtual void on_collision(object *obj, const frame_ctx &fc) {}

  inline const float *get_updated_Mmw() {
    if (node.Mmw_is_valid) {
      return node.Mmw;
    }
    mat4_set_translation(node.Mmw, &physics.position);
    mat4_append_rotation_about_z_axis(node.Mmw, physics.angle.z);
    mat3_scale(node.Mmw, &volume.scale);
    node.Mmw_is_valid = true;
    return node.Mmw;
  }
};
