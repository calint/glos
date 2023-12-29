#pragma once
class cube : public object {
public:
  inline cube() {
    node.glo = glos.get_by_index(glo_index_cube);
    volume.scale = {2, 1, 1};
    volume.radius = node.glo->bounding_radius * 2; // r * scale
    volume.is_sphere = false;
    collision_bits = cb_hero;
    collision_mask = cb_hero;
  }

  inline auto update(const frame_ctx &fc) -> bool override {
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
      v *= 10.0f;
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
