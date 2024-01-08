#pragma once
// reviewed: 2023-12-22
// reviewed: 2024-01-04
// reviewed: 2024-01-06

namespace glos {
class cell final {
public:
  // called from grid (possibly by multiple threads)
  inline void update() const {
    for (object *obj : ols) {
      if (threaded_grid) {
        if (obj->overlaps_cells) {
          // object is in several cells and may be called from multiple threads
          obj->acquire_lock();
          if (obj->updated_at_tick == frame_context.frame_num) {
            obj->release_lock();
            continue;
          }
          obj->updated_at_tick = frame_context.frame_num;
          obj->release_lock();
        }
      } else {
        if (obj->overlaps_cells) {
          // object is in several cells and may be called from multiple cells
          if (obj->updated_at_tick == frame_context.frame_num) {
            continue;
          }
          obj->updated_at_tick = frame_context.frame_num;
        }
      }

      // only one thread at a time is here for 'obj'

      if (not obj->update()) {
        obj->is_dead = true;
        objects.free(obj);
        continue;
      }

      // note. opportunity to clear the list prior to 'resolve_collisions'
      obj->clear_handled_collisions();
    }
  }

  // called from grid (possibly by multiple threads)
  inline void resolve_collisions() const {
    // thread safe because 'ols' does not change during 'resolve_collisions'
    size_t const len = ols.size();
    if (len < 2) {
      return;
    }
    for (unsigned i = 0; i < len - 1; ++i) {
      for (unsigned j = i + 1; j < len; ++j) {

        object *Oi = ols[i];
        object *Oj = ols[j];

        // thread safe because 'collision_mask' and 'collision_bits' do not
        // change during 'resolve_collisions'
        //! what if object 'on_collision' changes the mask or bitss

        bool const Oi_interest_of_Oj = Oi->collision_mask & Oj->collision_bits;
        bool const Oj_interest_of_Oi = Oj->collision_mask & Oi->collision_bits;
        // check if Oi and Oj have interest in collision with each other
        if (not Oi_interest_of_Oj and not Oj_interest_of_Oi) {
          continue;
        }

        // note. instead of checking if collision detection has been tried
        // between these 2 objects just try it because it is more expensive to
        // do the locking and lookup than just trying and only handling a
        // collision between 2 objects once

        if (not are_bounding_spheres_in_collision(Oi, Oj)) {
          continue;
        }

        // bounding spheres are in collision

        if (Oi->is_sphere and Oj->is_sphere) {
          bool Oi_handled_Oj = false;
          bool Oj_handled_Oi = false;
          if (Oi_interest_of_Oj) {
            Oi_handled_Oj = dispatch_collision(Oi, Oj);
          }
          if (Oj_interest_of_Oi) {
            Oj_handled_Oi = dispatch_collision(Oj, Oi);
          }
          if ((Oi_interest_of_Oj and not Oi_handled_Oj) or
              (Oj_interest_of_Oi and not Oj_handled_Oi)) {
            handle_sphere_collision(Oi, Oj);
          }
          continue;
        }

        // sphere vs planes
        if (Oi->is_sphere) {
          // Oj is not a sphere
          Oj->update_planes_world_coordinates();
          if (Oj->planes.is_in_collision_with_sphere(Oi->position,
                                                     Oi->radius)) {
            if (Oi_interest_of_Oj) {
              dispatch_collision(Oi, Oj);
            }
            if (Oj_interest_of_Oi) {
              dispatch_collision(Oj, Oi);
            }
          }
          continue;
        }
        
        if (Oj->is_sphere) {
          // Oi is not a sphere
          Oi->update_planes_world_coordinates();
          if (Oi->planes.is_in_collision_with_sphere(Oj->position,
                                                     Oj->radius)) {
            if (Oi_interest_of_Oj) {
              dispatch_collision(Oi, Oj);
            }
            if (Oj_interest_of_Oi) {
              dispatch_collision(Oj, Oi);
            }
          }
          continue;
        }

        // both objects are convex volumes bounded by planes

        if (not are_bounding_planes_in_collision(Oi, Oj)) {
          continue;
        }

        if (Oi_interest_of_Oj) {
          dispatch_collision(Oi, Oj);
        }
        if (Oj_interest_of_Oi) {
          dispatch_collision(Oj, Oi);
        }
      }
    }
  }

  // called from grid (from only one thread)
  inline void render() const {
    for (object *o : ols) {
      // check if object has been rendered by another cell
      if (o->rendered_at_tick == frame_context.frame_num) {
        continue;
      }
      o->rendered_at_tick = frame_context.frame_num;
      o->render();
      ++metrics.rendered_objects;
    }
  }

  // called from grid (from only one thread)
  inline void clear() { ols.clear(); }

  // called from grid (from only one thread)
  inline void add(object *o) { ols.push_back(o); }

  inline void print() const {
    unsigned i = 0;
    for (object const *o : ols) {
      if (i++) {
        printf(", ");
      }
      printf("%s", o->name.c_str());
    }
    printf("\n");
  }

  inline auto objects_count() const -> size_t { return ols.size(); }

private:
  std::vector<object *> ols{};

  static inline void handle_sphere_collision(object *Oi, object *Oj) {
    //! racing

    glm::vec3 const collision_normal =
        glm::normalize(Oj->position - Oi->position);

    float const relative_velocity_along_collision_normal =
        glm::dot(Oj->velocity - Oi->velocity, collision_normal);

    if (relative_velocity_along_collision_normal >= 0 or
        std::isnan(relative_velocity_along_collision_normal)) {
      return;
    }

    // resolve collision between the spheres

    constexpr float restitution = 1;
    float const impulse = (1.0f + restitution) *
                          relative_velocity_along_collision_normal /
                          (Oi->mass + Oj->mass);

    Oi->velocity += impulse * Oj->mass * collision_normal;
    Oj->velocity -= impulse * Oi->mass * collision_normal;
  }

  // returns true if collision has already been handled by Osrc
  static inline auto dispatch_collision(object *receiver, object *obj) -> bool {
    // if object overlaps cells then this code might be called by several
    // threads at the same time
    const bool synchronize = threaded_grid and receiver->overlaps_cells;

    if (synchronize) {
      receiver->acquire_lock();
    }

    if (receiver->is_collision_handled_and_if_not_add(obj)) {
      if (synchronize) {
        receiver->release_lock();
      }
      return true;
    }

    // only one thread at a time can be here

    if (not receiver->is_dead and not receiver->on_collision(obj)) {
      receiver->is_dead = true;
      objects.free(receiver);
    }

    if (synchronize) {
      receiver->release_lock();
    }

    return false;
  }

  static inline auto are_bounding_spheres_in_collision(object const *o1,
                                                       object const *o2)
      -> bool {

    glm::vec3 const v = o2->position - o1->position;
    float const d = o1->radius + o2->radius;
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
    // bounding volume object state does not change (spheres do)

    return o1->planes.is_any_point_in_volume(o2->planes) or
           o2->planes.is_any_point_in_volume(o1->planes);
  }
};
} // namespace glos