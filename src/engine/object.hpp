#pragma once
// reviewed: 2023-12-22
// reviewed: 2024-01-04
// reviewed: 2024-01-06
// reviewed: 2024-01-10

#include "o1store.hpp"
#include "planes.hpp"

namespace glos {

class object {
  friend class grid;
  friend class cell;

public:
  // first members in same order as in cell_entry
  glm::vec3 position{};        // in meters
  float bounding_radius = 0;   // in meters
  uint32_t collision_bits = 0; // mask & bits for collision subscription
  uint32_t collision_mask = 0; // ...
  // rest of object public state
  glm::vec3 velocity{};           // in meters/second
  glm::vec3 acceleration{};       // in meters/second^2
  glm::vec3 angle{};              // in radians
  glm::vec3 angular_velocity{};   // in radians/second
  uint32_t glob_ix = 0;           // index in globs store
  glm::vec3 scale{};              // in meters
  planes planes{};                // bounding planes (if any)
  float mass = 0;                 // in kg
  net_state *net_state = nullptr; // pointer to signals used by this object
  object **alloc_ptr;             // initiated at allocate by 'o1store'
  std::string name{};             // instance name
  bool is_sphere = false;         // true if object can be considered a sphere
private:
  glm::mat4 Mmw{};     // model -> world matrix
  glm::vec3 Mmw_pos{}; // position of current Mmw matrix
  glm::vec3 Mmw_agl{}; // angle of current Mmw matrix
  glm::vec3 Mmw_scl{}; // scale of current Mmw matrix

  // note. 32 bit resolution vs 64 bit comparison source ok since only checking
  // for equality
  uint32_t rendered_at_tick = 0; // used by cell to avoid rendering twice
  uint32_t updated_at_tick = 0;  // used by cell to avoid updating twice

  std::vector<const object *> handled_collisions{};
  std::atomic_flag lock = ATOMIC_FLAG_INIT;
  std::atomic_flag lock_get_updated_Mmw = ATOMIC_FLAG_INIT;
  bool overlaps_cells = false; // used by grid to flag cell overlap
  bool is_dead = false;        // used by cell to avoid events on dead objects

public:
  inline virtual ~object() = default;
  // note. 'delete obj;' may not be used because memory is managed by 'o1store'
  //       destructor is invoked in the 'objects.apply_free()'

  inline virtual void render() {
    globs.at(glob_ix).render(get_updated_Mmw());

    if (debug_object_planes_normals) {
      planes.debug_render_normals();
    }

    if (debug_object_bounding_sphere) {
      debug_render_bounding_sphere(debug_get_Mmw_for_bounding_sphere());
    }
  }

  // returns false if object has died, true otherwise
  inline virtual auto update() -> bool {
    float const dt = frame_context.dt;
    velocity += acceleration * dt;
    position += velocity * dt;
    angle += angular_velocity * dt;

    if (debug_object_planes_normals) {
      glm::mat4 const &M = get_updated_Mmw();
      glob const &g = globs.at(glob_ix);
      planes.update_model_to_world(g.planes_points, g.planes_normals, M,
                                   position, angle, scale);
    }

    return true;
  }

  // returns false if object has died, true otherwise
  inline virtual auto on_collision(object *obj) -> bool { return true; }

  inline auto get_updated_Mmw() -> glm::mat4 const & {
    // * synchronize if render and update run on different threads; both racing
    //   for this
    // * in threaded grid when objects in different cells running on different
    //   threads might race for this
    bool const synchronize = threaded_update or threaded_grid;

    if (synchronize) {
      while (lock_get_updated_Mmw.test_and_set(std::memory_order_acquire)) {
      }
    }

    if (is_Mmw_valid()) {
      if (synchronize) {
        lock_get_updated_Mmw.clear(std::memory_order_release);
      }
      return Mmw;
    }

    // save the state of the matrix
    Mmw_pos = position;
    Mmw_agl = angle;
    Mmw_scl = scale;
    // make a new matrix
    glm::mat4 const Ms = glm::scale(glm::mat4(1), Mmw_scl);
    glm::mat4 const Mr = glm::eulerAngleXYZ(Mmw_agl.x, Mmw_agl.y, Mmw_agl.z);
    glm::mat4 const Mt = glm::translate(glm::mat4(1), Mmw_pos);
    Mmw = Mt * Mr * Ms;

    if (synchronize) {
      lock_get_updated_Mmw.clear(std::memory_order_release);
    }

    return Mmw;
  }

private:
  inline auto is_Mmw_valid() const -> bool {
    return position == Mmw_pos and angle == Mmw_agl and scale == Mmw_scl;
  }

  inline void clear_handled_collisions() { handled_collisions.clear(); }

  inline auto is_collision_handled_and_if_not_add(object const *obj) -> bool {
    bool const found =
        std::ranges::find(handled_collisions, obj) != handled_collisions.cend();

    if (not found) {
      handled_collisions.push_back(obj);
    }

    return found;
  }

  inline void update_planes_world_coordinates() {
    bool const synchronize = threaded_grid and overlaps_cells;

    if (synchronize) {
      planes.acquire_lock();
    }

    glob const &g = globs.at(glob_ix);
    glm::mat4 const &M = get_updated_Mmw();
    planes.update_model_to_world(g.planes_points, g.planes_normals, M, Mmw_pos,
                                 Mmw_agl, Mmw_scl);

    if (synchronize) {
      planes.release_lock();
    }
  }

  inline auto debug_get_Mmw_for_bounding_sphere() const -> glm::mat4 {
    return glm::scale(glm::translate(glm::mat4(1), Mmw_pos),
                      glm::vec3(bounding_radius));
  }

  inline void acquire_lock() {
    while (lock.test_and_set(std::memory_order_acquire)) {
    }
  }

  inline void release_lock() { lock.clear(std::memory_order_release); }
};

class objects final {
public:
  inline void init() {
    // initiate list size and end pointer
    apply_allocated_instances();
  }

  inline void free() {
    object **const end = store_.allocated_list_end();
    // note. important to get the 'end' outside the loop because objects may
    //       allocate new objects in the loop and that would change 'end'
    for (object **it = store_.allocated_list(); it < end; ++it) {
      object *obj = *it;
      obj->~object(); // note. the freeing of memory is done by 'o1store'
    }
    //? what if destructor created objects
  }

  inline auto alloc() -> object * { return store_.allocate_instance(); }

  inline void free(object *o) { store_.free_instance(o); }

  inline auto allocated_list_len() const -> size_t {
    return allocated_list_len_;
  }

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

private:
  o1store<object, objects_count, 0, objects_instance_size_B> store_{};
  object **allocated_list_end_ = nullptr;
  size_t allocated_list_len_ = 0;
};

inline objects objects{};
} // namespace glos