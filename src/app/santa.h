#pragma once
class santa : public object {
public:
  inline santa() { volume = {.radius = 1.4f, .scale = {1, 1, 1, 0}}; }

  inline virtual auto update(const framectx &fc) -> bool override {
    if (object::update(fc)) {
      return true;
    }
    if (!keys_ptr) {
      return false;
    }
    int n = *keys_ptr;
    velocity *v = &physics_nxt.velocity;
    *v = vec4_def;
    if (n != 0) {
      // wasd keys
      if (n & 1)
        v->z = +1;
      if (n & 2)
        v->x = -1;
      if (n & 4)
        v->z = -1;
      if (n & 8)
        v->x = +1;
      vec3_scale(v, 10);
    }
    return false;
  }
};
