#pragma once
// reviewed: 2023-12-22

#include "../grid/grid_ifc.hpp"
#include "node.hpp"
#include "o1store.hpp"
#include "physics.hpp"
#include "volume.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <mutex>
#include <string>

class object {
public:
  std::string name{};
  node node{};                // render info
  volume volume{};            // bounding volume
  physics physics_prv{};      // physics state from previous frame
  physics physics_nxt{};      // physics state for next frame
  physics physics{};          // physics state of current frame
  grid_ifc grid_ifc{};        // interface to 'grid'
  net_state *state = nullptr; // pointer to signals used by this object
  object **alloc_ptr;         // initiated at allocate by 'o1store'
  std::atomic_flag spinlock = ATOMIC_FLAG_INIT;
  // std::atomic<int> spinlock{0};
  // std::mutex mutex{}; // slower than atomic

private:
  glm::vec3 Mmw_pos{}; // position of current Mmw matrix
  glm::vec3 Mmw_agl{}; // angle of current Mmw matrix
  glm::vec3 Mmw_scl{}; // scale of current Mmw matrix

public:
  inline object() {}

  inline virtual ~object() {}
  // note. 'delete obj' may not be used
  //       destructor invoked in the 'objects.apply_free'
  //       override only to free resources

  inline virtual void render(const frame_ctx &fc) {
    if (!node.glo) {
      return;
    }
    node.glo->render(get_updated_Mmw());
  }

  // returns true if object has died
  inline virtual auto update(const frame_ctx &fc) -> bool {
    physics_prv = physics;
    physics = physics_nxt;

    const float dt = fc.dt;
    physics_nxt.position += physics_nxt.velocity * dt;
    physics_nxt.angle += physics_nxt.angular_velocity * dt;
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

  inline void acquire_lock() {
    while (spinlock.test_and_set(std::memory_order_acquire)) {
    }
  }

  inline void release_lock() { spinlock.clear(std::memory_order_release); }

  // inline void acquire_lock() {
  //   while (spinlock.exchange(1, std::memory_order_acquire) == 1) {
  //   }
  // }

  // inline void release_lock() { spinlock.store(0, std::memory_order_release);
  // }

  // inline void acquire_lock() { mutex.lock(); }
  // inline void release_lock() { mutex.unlock(); }
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
