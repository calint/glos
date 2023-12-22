#pragma once
//--------------------------------------------------------------------- object
#include "../grid/grid-ifc.hpp"
#include "node.hpp"
#include "physics.hpp"
#include "volume.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <string>
//------------------------------------------------------------------------ def
class object {
public:
  std::string name{};
  unsigned update_tick = 0;
  unsigned render_tick = 0;
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
    if (node.Mmw_is_valid and (physics_prv.position != physics.position or
                               physics_prv.angle != physics.angle)) {
      node.Mmw_is_valid = false;
    }

    physics_prv = physics;
    physics = physics_nxt;

    const float dt = fc.dt;
    physics_nxt.position += physics_nxt.velocity * dt;
    physics_nxt.angle += physics_nxt.angular_velocity * dt;
    return false;
  }

  inline virtual void render(const frame_ctx &fc) {
    if (!node.glo) {
      return;
    }
    node.glo->render(get_updated_Mmw());
  }

  inline virtual void on_collision(object *obj, const frame_ctx &fc) {}

  inline const glm::mat4 &get_updated_Mmw() {
    if (node.Mmw_is_valid) {
      return node.Mmw;
    }
    glm::mat4 Ms = glm::scale(glm::mat4(1), volume.scale);
    glm::mat4 Mr =
        glm::eulerAngleXYZ(physics.angle.x, physics.angle.y, physics.angle.z);
    glm::mat4 Mt = glm::translate(glm::mat4(1), physics.position);
    node.Mmw = Mt * Mr * Ms;
    node.Mmw_is_valid = true;
    return node.Mmw;
  }
};

class objects final {
public:
  std::vector<object *> store{};

  inline void init() {}
  inline void free() {
    for (object *o : store) {
      delete o;
    }
  }
};

static objects objects{};
