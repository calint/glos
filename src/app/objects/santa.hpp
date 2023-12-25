#pragma once
class santa : public object {
  unsigned health = 0;

public:
  inline santa() {
    node.glo = glos.find_by_name("icosphere");
    volume.scale = {1, 1, 1};
    volume.radius = node.glo->bounding_radius * 1; // r * scale
    grid_ifc.collision_bits = cb_hero;
    grid_ifc.collision_mask = cb_hero;
    health = unsigned(rand()) % 50001;
  }

  // inline ~santa() { printf("%s destructor\n", name.c_str()); }

  inline auto update(const frame_ctx &fc) -> bool override {
    if (grid_ifc.bits & 2) {
      printf("update: object already dead\n");
      exit(1);
    }
    if (object::update(fc)) {
      return true;
    }
    if (physics.position.y < volume.radius) {
      physics_nxt.position.y = volume.radius;
      physics_nxt.velocity = {0, 0, 0};
    }
    if (!state) {
      return false;
    }
    // update using signals state
    glm::vec3 &v = physics_nxt.velocity;
    v = {0, 0, 0};
    unsigned keys = state->keys;
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

    // std::cout << name << " pos: " << glm::to_string(physics.position) <<
    // "\n";
    return false;
  }

  inline auto on_collision(object *o, const frame_ctx &fc) -> bool override {
    if (grid_ifc.bits & 2) {
      printf("on collision: object already dead\n");
      abort();
    }
    // printf("%s collision with %s\n", name.c_str(), o->name.c_str());
    health--;
    return health == 0;
  }

  // inline auto on_collision(object *o, const frame_ctx &fc) -> bool override {
  //   return false;
  // }
};
