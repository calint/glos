#pragma once
class sphere : public object {
public:
  inline sphere() {
    node.glo = glos.get_by_index(glo_index_sphere);
    volume.scale = {1, 1, 1};
    volume.radius = node.glo->bounding_radius * 1; // r * scale
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

    return false;
  }

  inline auto on_collision(object *o, const frame_ctx &fc) -> bool override {
    assert(not is_dead());
    // printf("%u: %s collision with %s\n", fc.tick, name.c_str(),
    //        o->name.c_str());
    return false;
  }
};
