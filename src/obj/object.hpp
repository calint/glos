#pragma once
// reviewed: 2023-12-22

#include "node.hpp"
#include "o1store.hpp"
#include "physics.hpp"
#include "volume.hpp"
#include <algorithm>

class object {
public:
  object **alloc_ptr;             // initiated at allocate by 'o1store'
  std::string name{};             // instance name
  node node{};                    // render info
  volume volume{};                // bounding volume
  physics physics_prv{};          // physics state from previous frame
  physics physics_nxt{};          // physics state for next frame
  physics physics{};              // physics state of current frame
  unsigned collision_bits = 0;    // mask & bits for collision subscription
  unsigned collision_mask = 0;    // ...
  net_state *net_state = nullptr; // pointer to signals used by this object
  unsigned rendered_at_tick = 0;  // avoids rendering same object twice
  unsigned updated_at_tick = 0;   // avoids updating same object twice
  unsigned grid_flags = 0;        // used by grid (overlaps, is_dead)
  std::vector<const object *> handled_collisions{};
  std::atomic_flag spinlock = ATOMIC_FLAG_INIT;

private:
  glm::vec3 Mmw_pos{}; // position of current Mmw matrix
  glm::vec3 Mmw_agl{}; // angle of current Mmw matrix
  glm::vec3 Mmw_scl{}; // scale of current Mmw matrix

public:
  inline virtual ~object() {}
  // note. 'delete obj' may not be used because memory is managed by 'o1store'
  //       destructor is invoked in the 'objects.apply_free'

  inline virtual void render() {
    if (!node.glo) {
      return;
    }
    node.glo->render(get_updated_Mmw());
  }

  // returns true if object has died
  inline virtual auto update(const frame_ctx &fc) -> bool {
    physics_prv = physics;
    physics = physics_nxt;

    physics_nxt.step(fc);
    return false;
  }

  // returns true if object has died
  inline virtual auto on_collision(object *obj, const frame_ctx &fc) -> bool {
    return false;
  }

  inline auto is_Mmw_valid() const -> bool {
    return physics.position == Mmw_pos and physics.angle == Mmw_agl and
           volume.scale == Mmw_scl;
  }

  inline const glm::mat4 &get_updated_Mmw() {
    if (is_Mmw_valid()) {
      return node.Mmw;
    }
    // save the state of the matrix
    Mmw_pos = physics.position;
    Mmw_agl = physics.angle;
    Mmw_scl = volume.scale;
    // make a new matrix
    glm::mat4 Ms = glm::scale(glm::mat4(1), Mmw_scl);
    glm::mat4 Mr = glm::eulerAngleXYZ(Mmw_agl.x, Mmw_agl.y, Mmw_agl.z);
    glm::mat4 Mt = glm::translate(glm::mat4(1), Mmw_pos);
    node.Mmw = Mt * Mr * Ms;
    return node.Mmw;
  }

  inline auto is_collision_handled_and_if_not_add(const object *obj) -> bool {
    const bool is_handled =
        std::find(handled_collisions.begin(), handled_collisions.end(), obj) !=
        handled_collisions.end();
    if (not is_handled) {
      handled_collisions.push_back(obj);
    }
    return is_handled;
  }

  inline void clear_handled_collisions() { handled_collisions.clear(); }

  inline auto is_dead() const -> bool { return grid_flags & 2; }

  inline void set_is_dead() { grid_flags |= 2; }

  inline auto is_overlaps_cells() const -> bool { return grid_flags & 1; }

  inline void set_overlaps_cells() { grid_flags |= 1; }

  inline void clear_flags() { grid_flags = 0; }

  inline void acquire_lock() {
    while (spinlock.test_and_set(std::memory_order_acquire)) {
    }
  }

  inline void release_lock() { spinlock.clear(std::memory_order_release); }
};

class objects final {
  o1store<object, objects_count, 0, objects_instance_size_B> store_{};
  object **allocated_list_end_ = nullptr;
  int allocated_list_len_ = 0;

public:
  inline void init() {}

  inline void free() {
    object **const end = store_.allocated_list_end();
    // note. important to get the 'end' outside the loop because objects may
    //       allocate new objects in the loop and that would change the 'end'
    for (object **it = store_.allocated_list(); it < end; it++) {
      object *obj = *it;
      obj->~object(); // note. the freeing of the memory is done by 'o1store'
    }
    //? what if destructors created new objects
  }

  inline auto alloc() -> object * { return store_.allocate_instance(); }

  inline void free(object *o) { store_.free_instance(o); }

  inline auto allocated_list_len() const -> int { return allocated_list_len_; }

  inline auto allocated_list_end() const { return allocated_list_end_; }

  inline auto allocated_list() const -> object ** {
    return store_.allocated_list();
  }

  inline void apply_allocated_instances() {
    // retrieve the end of list because during 'update' new objects might be
    // created which would change the end of list pointer
    allocated_list_len_ = store_.allocated_list_len();
    allocated_list_end_ = store_.allocated_list_end();
  }

  inline void apply_freed_instances() { store_.apply_free(); }
};

static objects objects{};
