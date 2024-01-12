#pragma once
// reviewed: 2024-01-04
// reviewed: 2024-01-08

class sphere final : public object {
public:
  inline sphere() {
    name = "sphere";
    if (debug_multiplayer) {
      ++counter;
      name.append(1, '_').append(std::to_string(counter));
      printf("%lu: %lu: create %s\n", frame_context.frame_num, frame_context.ms,
             name.c_str());
    }
    glob_ix = glob_ix_sphere;
    scale = {1.0f, 1.0f, 1.0f};
    radius = globs.at(glob_ix).bounding_radius * scale.x;
    is_sphere = true;
    mass = 1;
    collision_bits = cb_power_up;
    collision_mask = cb_hero;
  }

  inline ~sphere() override {
    if (debug_multiplayer) {
      printf("%lu: %lu: free %s\n", frame_context.frame_num, frame_context.ms,
             name.c_str());
    }
  }

  inline auto update() -> bool override {
    if (not object::update()) {
      return false;
    }

    return true;
  }

  inline auto on_collision(object *o) -> bool override {
    if (debug_multiplayer) {
      printf("%lu: %lu: %s collision with %s\n", frame_context.frame_num,
             frame_context.ms, name.c_str(), o->name.c_str());
    }

    score += 10;

    return true;
  }
};