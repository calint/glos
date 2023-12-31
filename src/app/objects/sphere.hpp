#pragma once
class sphere : public glos::object {
public:
  inline sphere() {
    node.glo_ix = glo_sphere_ix;
    const glos::glo &g = glos::glos.get_by_index(node.glo_ix);
    volume.scale = {1, 1, 1};
    volume.radius = g.bounding_radius * 1; // r * scale
    collision_bits = cb_hero;
    collision_mask = cb_hero;
  }

  inline auto update(const frame_ctx &fc) -> bool override {
    assert(not is_dead());

    if (object::update(fc)) {
      return true;
    }

    if (physics.position.y < volume.radius) {
      physics_nxt.position.y = volume.radius;
      physics_nxt.velocity.y = 0;
    }

    if (physics.position.x < -80) {
      physics_nxt.position.x = -80;
      physics_nxt.velocity.x = -physics_nxt.velocity.x;
    }

    if (physics.position.x > 80) {
      physics_nxt.position.x = 80;
      physics_nxt.velocity.x = -physics_nxt.velocity.x;
    }

    if (physics.position.z < -80) {
      physics_nxt.position.z = -80;
      physics_nxt.velocity.z = -physics_nxt.velocity.z;
    }

    if (physics.position.z > 80) {
      physics_nxt.position.z = 80;
      physics_nxt.velocity.z = -physics_nxt.velocity.z;
    }

    if (!net_state) {
      return false;
    }

    // update using signals state
    glm::vec3 &v = physics_nxt.velocity;
    glm::vec3 &a = physics_nxt.angle;
    glm::vec3 &av = physics_nxt.angular_velocity;
    v = {0, 0, 0};
    av = {0, 0, 0};
    unsigned keys = net_state->keys;
    if (keys != 0) {
      // wasd keys
      if (keys & 1) {
        v = -glm::vec3{sin(a.y), 0, cos(a.y)};
      }
      if (keys & 2) {
        av.y = 1;
      }
      if (keys & 4) {
        v = {sin(a.y), 0, cos(a.y)};
      }
      if (keys & 8) {
        av.y = -1;
      }
      v *= 2.0f;
    }
    return false;
  }

  inline auto on_collision(object *o, const frame_ctx &fc) -> bool override {
    assert(not is_dead());
    printf("%u: %s collision with %s\n", fc.tick, name.c_str(),
           o->name.c_str());
    return false;
  }
};
