#pragma once
// reviewed: 2023-12-22

#include <algorithm>
#include <atomic>

class object;

// decoupling circular reference between 'grid' and 'object'
class grid_ifc final {
  unsigned flags = 0; // 1: overlaps cells  2: is dead

public:
  std::atomic_flag spinlock = ATOMIC_FLAG_INIT;

  inline void acquire_lock() {
    while (spinlock.test_and_set(std::memory_order_acquire)) {
    }
  }

  inline void release_lock() { spinlock.clear(std::memory_order_release); }

  inline auto is_dead() const -> bool { return flags & 2; }

  inline void set_is_dead() { flags |= 2; }

  inline auto is_overlaps_cells() const -> bool { return flags & 1; }

  inline void set_overlaps_cells() { flags |= 1; }

  inline void clear_flags() { flags = 0; }
};
