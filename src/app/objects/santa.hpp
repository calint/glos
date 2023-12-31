#pragma once
class santa : public glos::object {
  unsigned health = 0;

public:
  inline santa() {
    //? glos.find_by_index for O(1)
    node.glo_ix = glo_santa_ix;
    const glos::glo &g = glos::glos.at(node.glo_ix);
    volume.scale = {1, 1, 1};
    volume.radius = g.bounding_radius * 1; // r * scale
    collision_bits = cb_hero;
    collision_mask = cb_hero;
    health = unsigned(rand()) % 50001;
  }

  // inline ~santa() { printf("%s destructor\n", name.c_str()); }

  inline auto update(const frame_ctx &fc) -> bool override {
    assert(not is_dead());

    if (object::update(fc)) {
      return true;
    }

    if (physics.position.y < volume.radius) {
      physics_nxt.position.y = volume.radius;
      physics_nxt.velocity = {0, 0, 0};
    }

    if (!net_state) {
      return false;
    }

    // update using signals state
    glm::vec3 &v = physics_nxt.velocity;
    v = {0, 0, 0};
    unsigned keys = net_state->keys;
    if (keys != 0) {
      // wasd keys
      if (keys & 1) {
        v.z = -1;
      }
      if (keys & 2) {
        v.x = -1;
      }
      if (keys & 4) {
        v.z = +1;
      }
      if (keys & 8) {
        v.x = +1;
      }
      v *= 10.0f;
    }

    return false;
  }

  inline auto on_collision(object *o, const frame_ctx &fc) -> bool override {
    assert(not is_dead());
    // printf("%s collision with %s\n", name.c_str(), o->name.c_str());
    health--;
    return health == 0;
  }
};