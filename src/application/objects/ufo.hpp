#pragma once
// reviewed: 2024-01-04
// reviewed: 2024-01-08
// reviewed: 2024-07-08

class ufo final : public object {
public:
  inline ufo() {
    if (debug_multiplayer) {
      ++counter;
      name.append("ufo_").append(std::to_string(counter));
      printf("%lu: %lu: create %s\n", frame_context.frame_num, frame_context.ms,
             name.c_str());
    }
    ++ufos_alive;
    glob_ix(glob_ix_ufo);
    scale = {1.0f, 1.0f, 1.0f};
    bounding_radius = globs.at(glob_ix()).bounding_radius * scale.x;
    is_sphere = true;
    mass = 20;
    collision_bits = cb_ufo;
    collision_mask = cb_hero_bullet;
  }

  inline ~ufo() override {
    if (debug_multiplayer) {
      printf("%lu: %lu: free %s\n", frame_context.frame_num, frame_context.ms,
             name.c_str());
    }
    --ufos_alive;
  }

  inline auto update() -> bool override {
    if (!object::update()) {
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

    score += 400;

    for (uint32_t i = 0; i < ufo_power_ups_at_death; ++i) {
      power_up *pu = new (objects.alloc()) power_up{};
      pu->position = position;
      pu->velocity = {rnd1(ufo_power_up_velocity), 0,
                      rnd1(ufo_power_up_velocity)};
      pu->angular_velocity.y = radians(90.0f);
    }

    return false;
  }
};
