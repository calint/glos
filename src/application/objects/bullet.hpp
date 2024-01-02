#pragma once

class bullet : public object {
public:
  inline bullet() {
    name = "bullet";
    glob_ix = glob_bullet_ix;
    scale = {1, 1, 1};
    glob const &g = globs.at(glob_ix);
    radius = g.bounding_radius * 1; // r * scale
    mass = 1;
    collision_bits = cb_hero_bullet;
    collision_mask = cb_asteroid;
  }

  inline auto update(frame_context const &fc) -> bool override {
    assert(not is_dead());

    if (object::update(fc)) {
      return true;
    }

    if (position.y < -80 or position.x > 80 or position.y < -80 or
        position.y > 80 or position.z < -80 or position.z > 80) {
      return true;
    }

    return false;
  }

  inline auto on_collision(object *o, frame_context const &fc)
      -> bool override {
    assert(not is_dead());
    printf("%u: %s collision with %s\n", fc.tick, name.c_str(),
           o->name.c_str());
    return true;
  }
};
