#pragma once
class cube : public object {
public:
  inline cube() {
    node.glo = glos.get_by_index(glo_index_cube);
    volume.scale = {2, 1, 1};
    volume.radius = node.glo->bounding_radius * 2; // r * scale
    collision_bits = cb_hero;
    collision_mask = cb_hero;
  }

  inline auto update(const frame_ctx &fc) -> bool override {
    assert(not is_dead());

    if (object::update(fc)) {
      return true;
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
