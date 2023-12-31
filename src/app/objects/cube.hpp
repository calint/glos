#pragma once
class cube : public glos::object {
public:
  inline cube() {
    glo_ix = glo_cube_ix;
    scale = {2, 1, 1};
    glos::glo const &g = glos::glos.at(glo_ix);
    radius = g.bounding_radius * 2; // r * scale
    is_sphere = false;
    collision_bits = cb_hero;
    collision_mask = cb_hero;
  }

  inline auto update(const frame_context &fc) -> bool override {
    assert(not is_dead());

    if (object::update(fc)) {
      return true;
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
        v = -glm::vec3{glm::sin(a.y), 0, glm::cos(a.y)};
      }
      if (keys & 2) {
        av.y = 1;
      }
      if (keys & 4) {
        v = {glm::sin(a.y), 0, glm::cos(a.y)};
      }
      if (keys & 8) {
        av.y = -1;
      }
      v *= 5.0f;
    }

    return false;
  }

  inline auto on_collision(object *o, const frame_context &fc)
      -> bool override {
    assert(not is_dead());
    printf("%u: %s collision with %s\n", fc.tick, name.c_str(),
           o->name.c_str());
    return false;
  }
};
