#pragma once
// reviewed: 2023-12-22

#include <algorithm>
#include <atomic>

class object;

// decoupling circular reference between 'grid' and 'object'
class grid_ifc final {
  std::vector<const object *> handled_collisions{};
  unsigned flags = 0; // 1: overlaps cells  2: is dead

public:
  unsigned collision_bits = 0;
  unsigned collision_mask = 0;
  unsigned updated_at_tick = 0;
  unsigned rendered_at_tick = 0;
  std::atomic_flag spinlock = ATOMIC_FLAG_INIT;

  inline void acquire_lock() {
    while (spinlock.test_and_set(std::memory_order_acquire)) {
    }
  }

  inline void release_lock() { spinlock.clear(std::memory_order_release); }

  inline auto is_collision_handled_and_add_if_not(const object *obj) -> bool {
    const bool is_handled =
        std::find(handled_collisions.begin(), handled_collisions.end(), obj) !=
        handled_collisions.end();
    if (not is_handled) {
      handled_collisions.push_back(obj);
    }
    return is_handled;
  }

  inline auto is_dead() const -> bool { return flags & 2; }

  inline void set_is_dead() { flags |= 2; }

  inline auto is_overlaps_cells() const -> bool { return flags & 1; }

  inline void set_overlaps_cells() { flags |= 1; }

  inline void clear_flags() { flags = 0; }

  inline void clear_handled_collisions() { handled_collisions.clear(); }
};
