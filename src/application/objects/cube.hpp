#pragma once
// reviewed: 2024-01-04
// reviewed: 2024-01-08

class cube final : public object {
public:
  inline cube() {
    name = "cube";
    if (debug_multiplayer) {
      ++counter;
      name.append(1, '_').append(std::to_string(counter));
      printf("%lu: %lu: create %s\n", frame_context.frame_num, frame_context.ms,
             name.c_str());
    }
    glob_ix = glob_ix_cube;
    scale = {1.0f, 1.0f, 1.0f};
    radius = globs.at(glob_ix).bounding_radius * scale.x;
    angular_velocity.y = radians(20.0f);
    mass = 10;
    collision_bits = cb_power_up;
    collision_mask = cb_power_up;
  }

  inline ~cube() override {
    if (debug_multiplayer) {
      printf("%lu: %lu: free %s\n", frame_context.frame_num, frame_context.ms,
             name.c_str());
    }
  }

  inline auto update() -> bool override {
    if (not object::update()) {
      return false;
    }

    game_area_roll(position);

    return true;
  }

  inline auto on_collision(object *o) -> bool override {
    if (debug_multiplayer) {
      printf("%lu: %lu: %s collision with %s\n", frame_context.frame_num,
             frame_context.ms, name.c_str(), o->name.c_str());
    }

    score += 1;

    return true;
  }
};
