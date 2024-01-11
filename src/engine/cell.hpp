#pragma once
// reviewed: 2023-12-22
// reviewed: 2024-01-04
// reviewed: 2024-01-06
// reviewed: 2024-01-10

namespace glos {

// object entry in a cell with copied members used in the hot code path for
// better cache utilization
struct cell_entry {
  glm::vec3 position{};
  float radius = 0;
  uint32_t collision_bits = 0;
  uint32_t collision_mask = 0;
  object *object = nullptr;
};

class cell final {
public:
  // called from grid (possibly by multiple threads)
  inline void update() const {
    for (cell_entry const &ce : entry_list) {
      if (threaded_grid) {
        // multithreaded mode
        if (ce.object->overlaps_cells) {
          // object is in several cells and may be called from multiple threads
          ce.object->acquire_lock();
          if (ce.object->updated_at_tick == frame_context.frame_num) {
            // object already updated in a different cell
            ce.object->release_lock();
            continue;
          }
          ce.object->updated_at_tick = uint32_t(frame_context.frame_num);
          ce.object->release_lock();
        }
      } else {
        // single threaded mode
        if (ce.object->overlaps_cells) {
          // object is in several cells and may be called from multiple cells
          if (ce.object->updated_at_tick == frame_context.frame_num) {
            continue;
          }
          ce.object->updated_at_tick = uint32_t(frame_context.frame_num);
        }
      }

      // only one thread at a time is here for 'ce.object'

      if (not ce.object->update()) {
        ce.object->is_dead = true;
        objects.free(ce.object);
        continue;
      }

      // note. opportunity to clear the list prior to 'resolve_collisions'
      ce.object->clear_handled_collisions();
    }
  }

  // called from grid (possibly by multiple threads)
  inline void resolve_collisions() const {
    // thread safe because 'entry_list' does not change during
    // 'resolve_collisions'
    size_t const len = entry_list.size();
    if (len < 2) {
      return;
    }
    for (unsigned i = 0; i < len - 1; ++i) {
      for (unsigned j = i + 1; j < len; ++j) {

        // thread safe because cell entries do not change during
        // 'resolve_collisions'
        cell_entry const &Oi = entry_list[i];
        cell_entry const &Oj = entry_list[j];

        // check if Oi and Oj have interest in collision with each other
        bool const Oi_interest_of_Oj = Oi.collision_mask & Oj.collision_bits;
        bool const Oj_interest_of_Oi = Oj.collision_mask & Oi.collision_bits;

        if (not Oi_interest_of_Oj and not Oj_interest_of_Oi) {
          continue;
        }

        // if (not are_bounding_spheres_in_collision(Oi, Oj)) {
        //   continue;
        // }

        // bounding spheres are in collision

        if (Oi.object->is_sphere and Oj.object->is_sphere) {
          bool const Oi_already_handled_Oj =
              Oi_interest_of_Oj ? dispatch_collision(Oi.object, Oj.object)
                                : false;
          bool const Oj_already_handled_Oi =
              Oj_interest_of_Oi ? dispatch_collision(Oj.object, Oi.object)
                                : false;

          // if collision has already been handled, possibly on a different
          // thread or grid cell continue
          if (Oi_already_handled_Oj or Oj_already_handled_Oi) {
            continue;
          }

          // collision has not been handled during this frame by any cell
          handle_sphere_collision(Oi.object, Oj.object);
          continue;
        }

        // check if sphere vs planes

        if (Oi.object->is_sphere) {
          // Oj is not a sphere
          Oj.object->update_planes_world_coordinates();
          if (Oj.object->planes.is_in_collision_with_sphere(Oi.position,
                                                            Oi.radius)) {
            if (Oi_interest_of_Oj) {
              dispatch_collision(Oi.object, Oj.object);
            }
            if (Oj_interest_of_Oi) {
              dispatch_collision(Oj.object, Oi.object);
            }
          }
          continue;
        }

        if (Oj.object->is_sphere) {
          // Oi is not a sphere
          Oi.object->update_planes_world_coordinates();
          if (Oi.object->planes.is_in_collision_with_sphere(Oj.position,
                                                            Oj.radius)) {
            if (Oi_interest_of_Oj) {
              dispatch_collision(Oi.object, Oj.object);
            }
            if (Oj_interest_of_Oi) {
              dispatch_collision(Oj.object, Oi.object);
            }
          }
          continue;
        }

        // both objects are convex volumes bounded by planes

        if (not are_bounding_planes_in_collision(Oi.object, Oj.object)) {
          continue;
        }

        // bounding planes in collision

        if (Oi_interest_of_Oj) {
          dispatch_collision(Oi.object, Oj.object);
        }
        if (Oj_interest_of_Oi) {
          dispatch_collision(Oj.object, Oi.object);
        }
      }
    }
  }

  // called from grid (from only one thread)
  inline void render() const {
    for (cell_entry const &ce : entry_list) {
      if (ce.object->overlaps_cells) {
        // check if object has been rendered by another cell
        if (ce.object->rendered_at_tick == frame_context.frame_num) {
          continue;
        }
        ce.object->rendered_at_tick = uint32_t(frame_context.frame_num);
      }
      ce.object->render();
      ++metrics.rendered_objects;
    }
  }

  // called from grid (from only one thread)
  inline void clear() { entry_list.clear(); }

  // called from grid (from only one thread)
  inline void add(object *o) {
    entry_list.emplace_back(o->position, o->radius, o->collision_bits,
                            o->collision_mask, o);
  }

  inline void print() const {
    unsigned i = 0;
    for (cell_entry const &ce : entry_list) {
      if (i++) {
        printf(", ");
      }
      printf("%s", ce.object->name.c_str());
    }
    printf("\n");
  }

  inline auto objects_count() const -> size_t { return entry_list.size(); }

private:
  std::vector<cell_entry> entry_list{};

  static inline void handle_sphere_collision(object *Oi, object *Oj) {
    // synchronize objects that overlap cells
    
    if (threaded_grid and Oi->overlaps_cells) {
      Oi->acquire_lock();
    }
    if (threaded_grid and Oj->overlaps_cells) {
      Oj->acquire_lock();
    }

    glm::vec3 const collision_normal =
        glm::normalize(Oj->position - Oi->position);

    float const relative_velocity_along_collision_normal =
        glm::dot(Oj->velocity - Oi->velocity, collision_normal);

    if (relative_velocity_along_collision_normal >= 0 or
        std::isnan(relative_velocity_along_collision_normal)) {
      // sphere are not moving towards each other
      if (threaded_grid and Oj->overlaps_cells) {
        Oj->release_lock();
      }
      if (threaded_grid and Oi->overlaps_cells) {
        Oi->release_lock();
      }
      return;
    }

    // resolve collision between the spheres

    constexpr float restitution = 1;
    float const impulse = (1.0f + restitution) *
                          relative_velocity_along_collision_normal /
                          (Oi->mass + Oj->mass);

    Oi->velocity += impulse * Oj->mass * collision_normal;
    Oj->velocity -= impulse * Oi->mass * collision_normal;

    if (threaded_grid and Oj->overlaps_cells) {
      Oj->release_lock();
    }
    if (threaded_grid and Oi->overlaps_cells) {
      Oi->release_lock();
    }
  }

  // returns true if collision with 'obj' has already been handled by 'receiver'
  static inline auto dispatch_collision(object *receiver, object *obj) -> bool {
    // if object overlaps cells then this code might be called by several
    // threads at the same time from different cells
    bool const synchronize = threaded_grid and receiver->overlaps_cells;

    if (synchronize) {
      receiver->acquire_lock();
    }

    // if both objects overlap cells then the same collision is detected and
    // dispatched in multiple cells
    if (receiver->overlaps_cells and obj->overlaps_cells and
        receiver->is_collision_handled_and_if_not_add(obj)) {
      // collision already dispatched for this 'receiver' and 'obj'
      if (synchronize) {
        receiver->release_lock();
      }
      return true;
    }

    // only one thread at a time can be here for 'receiver'

    if (not receiver->is_dead and not receiver->on_collision(obj)) {
      receiver->is_dead = true;
      objects.free(receiver);
    }

    if (synchronize) {
      receiver->release_lock();
    }

    return false;
  }

  static inline auto are_bounding_spheres_in_collision(cell_entry const &ce1,
                                                       cell_entry const &ce2)
      -> bool {

    glm::vec3 const v = ce2.position - ce1.position;
    float const d = ce1.radius + ce2.radius;
    float const dsq = d * d;
    float const vsq = glm::dot(v, v); // distance squared
    float const diff = vsq - dsq;
    return diff < 0;
  }

  static inline auto are_bounding_planes_in_collision(object *o1, object *o2)
      -> bool {

    o1->update_planes_world_coordinates();
    o2->update_planes_world_coordinates();

    // planes can be update only once per 'resolve_collisions' pass because
    // bounding volume objects state do not change because there is no handle
    // collision implementation (spheres do)

    return planes::are_in_collision(o1->planes, o2->planes);
  }
};
} // namespace glos