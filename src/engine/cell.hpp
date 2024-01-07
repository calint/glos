#pragma once
// reviewed: 2023-12-22
// reviewed: 2024-01-04
// reviewed: 2024-01-06

namespace glos {
class cell final {
public:
  // called from grid (possibly by multiple threads)
  inline void update() const {
    for (object *o : ols) {
      // check if object has already been updated by a different cell
      if (threaded_grid and o->is_overlaps_cells()) {
        // object is in several cells and may be called from multiple threads

        o->acquire_lock();
        if (o->updated_at_tick == frame_context.frame_num) {
          o->release_lock();
          continue;
        }
        o->updated_at_tick = frame_context.frame_num;
        o->release_lock();

      } else {
        // object is in only one cell or can only be called from one thread
        if (o->updated_at_tick == frame_context.frame_num) {
          continue;
        }
        o->updated_at_tick = frame_context.frame_num;
      }

      // only one thread at a time is here

      if (o->update()) {
        // object died during update
        o->set_is_dead();
        objects.free(o);
        continue;
      }

      // note. opportunity to clear the list prior to 'resolve_collisions'
      o->clear_handled_collisions();
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

        // check if Oi and Oj have interest in collision with each other
        if ((Oi->collision_mask & Oj->collision_bits) == 0 and
            (Oj->collision_mask & Oi->collision_bits) == 0) {
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
          handle_sphere_collision(Oi, Oj);
          handle_sphere_collision(Oj, Oi);
          continue;
        }

        // sphere vs planes
        if (Oi->is_sphere) {
          // Oj is not a sphere
          update_planes_world_coordinates(Oj);
          if (Oj->planes.is_in_collision_with_sphere(Oi->position,
                                                     Oi->radius)) {
            dispatch_collision(Oi, Oj);
            dispatch_collision(Oj, Oi);
          }
          continue;

        } else if (Oj->is_sphere) {
          // Oi is not a sphere
          update_planes_world_coordinates(Oi);
          if (Oi->planes.is_in_collision_with_sphere(Oj->position,
                                                     Oj->radius)) {
            dispatch_collision(Oj, Oi);
            dispatch_collision(Oi, Oj);
          }
          continue;
        }

        // both objects are convex volumes

        if (not are_bounding_planes_in_collision(Oi, Oj)) {
          continue;
        }

        dispatch_collision(Oi, Oj);
        dispatch_collision(Oj, Oi);
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

    // check if Oi is subscribed to collision with Oj
    if ((Oi->collision_mask & Oj->collision_bits) == 0) {
      return;
    }

    // if object overlaps cells and threaded grid then this code might be called
    // by several threads at the same time
    const bool synchronize = threaded_grid and Oi->is_overlaps_cells();

    if (synchronize) {
      Oi->acquire_lock();
    }

    if (Oi->is_collision_handled_and_if_not_add(Oj)) {
      if (synchronize) {
        Oi->release_lock();
      }
      return;
    }

    // only one thread at a time can be here

    glm::vec3 const collision_normal =
        glm::normalize(Oj->position - Oi->position);

    float const relative_velocity_along_collision_normal =
        glm::dot(Oj->velocity - Oi->velocity, collision_normal);

    if (relative_velocity_along_collision_normal >= 0 or
        std::isnan(relative_velocity_along_collision_normal)) {

      // objects are not moving towards each other
      if (synchronize) {
        Oi->release_lock();
      }
      return;
    }

    if (not Oi->is_dead() and Oi->on_collision(Oj)) {
      // object has died
      Oi->set_is_dead();
      objects.free(Oi);
      if (synchronize) {
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

    if (synchronize) {
      Oi->release_lock();
    }
  }

  static inline void dispatch_collision(object *Osrc, object *Otrg) {

    // check if Osrc is subscribed to collision with Otrg
    if ((Osrc->collision_mask & Otrg->collision_bits) == 0) {
      return;
    }

    // if object overlaps cells this code might be called by several threads at
    // the same time
    const bool synchronize = threaded_grid and Osrc->is_overlaps_cells();

    if (synchronize) {
      Osrc->acquire_lock();
    }

    if (Osrc->is_collision_handled_and_if_not_add(Otrg)) {
      if (synchronize) {
        Osrc->release_lock();
      }
      return;
    }

    // only one thread at a time can be here

    if (not Osrc->is_dead() and Osrc->on_collision(Otrg)) {
      // object died
      Osrc->set_is_dead();
      objects.free(Osrc);
    }

    if (synchronize) {
      Osrc->release_lock();
    }
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

  static inline void update_planes_world_coordinates(object *o) {
    bool const synchronize = threaded_grid and o->is_overlaps_cells();

    if (synchronize) {
      o->planes.acquire_lock();
    }

    glob const &g = globs.at(o->glob_ix);

    o->planes.update_model_to_world(g.planes_points, g.planes_normals,
                                    o->get_updated_Mmw(), o->position, o->angle,
                                    o->scale);

    if (synchronize) {
      o->planes.release_lock();
    }
  }

  static inline auto are_bounding_planes_in_collision(object *o1, object *o2)
      -> bool {

    update_planes_world_coordinates(o1);
    update_planes_world_coordinates(o2);

    // planes can be update only once per 'resolve_collisions' pass because
    // bounding volume object state does not change (spheres do)

    if (o1->planes.is_in_collision_with_planes(o2->planes) or
        o2->planes.is_in_collision_with_planes(o1->planes)) {
      return true;
    }

    return false;
  }
};
} // namespace glos