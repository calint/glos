#pragma once
// reviewed: 2023-12-22
namespace glos {
class cell final {
public:
  std::vector<object *> ols{};

  // called from grid (possibly by multiple threads)
  inline void update(frame_context const &fc) {
    for (object *o : ols) {
      if (grid_threaded and o->is_overlaps_cells()) {
        // object is in several cells and may be called from multiple threads

        o->acquire_lock();
        if (o->updated_at_tick == fc.tick) {
          o->release_lock();
          continue;
        }
        o->updated_at_tick = fc.tick;
        o->release_lock();
      } else {
        // object is in only one cell or can only be called from one thread
        if (o->updated_at_tick == fc.tick) {
          continue;
        }
        o->updated_at_tick = fc.tick;
      }

      // only one thread at a time gets to this code

      if (o->update(fc)) {
        o->set_is_dead();
        objects.free(o);
        continue;
      }

      // note. opportunity to clear the list prior to 'resolve_collisions'
      o->clear_handled_collisions();
    }
  }

  // called from grid (possibly by multiple threads)
  inline void resolve_collisions(frame_context const &fc) {
    // thread safe because 'ols' does not change during 'resolve_collisions'
    const size_t len = ols.size();
    if (len == 0) {
      return;
    }
    for (unsigned i = 0; i < len - 1; i++) {
      for (unsigned j = i + 1; j < len; j++) {

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

        if (not are_bounding_spheres_in_collision(Oi, Oj, fc)) {
          continue;
        }

        if (Oi->is_sphere and Oj->is_sphere) {
          handle_sphere_collision(Oi, Oj, fc);
          handle_sphere_collision(Oj, Oi, fc);
          continue;
        }

        // sphere vs planes
        if (Oi->is_sphere) {
          // Oj is not a sphere
          update_planes_world_coordinates(Oj);
          if (Oj->planes.is_in_collision_with_sphere(Oi->position,
                                                     Oi->radius)) {
            dispatch_collision(Oi, Oj, fc);
            dispatch_collision(Oj, Oi, fc);
          }
          continue;
        } else if (Oj->is_sphere) {
          // Oi is not a sphere
          update_planes_world_coordinates(Oi);
          if (Oi->planes.is_in_collision_with_sphere(Oj->position,
                                                     Oj->radius)) {
            dispatch_collision(Oj, Oi, fc);
            dispatch_collision(Oi, Oj, fc);
          }
          continue;
        }

        if (not are_bounding_planes_in_collision(Oi, Oj, fc)) {
          continue;
        }

        dispatch_collision(Oi, Oj, fc);
        dispatch_collision(Oj, Oi, fc);
      }
    }
  }

  // called from grid (from only one thread)
  inline void render(unsigned const render_frame_num) {
    for (object *o : ols) {
      if (o->rendered_at_tick == render_frame_num) {
        continue;
      }
      o->rendered_at_tick = render_frame_num;
      o->render();
      metrics.rendered_objects++;
    }
  }

  // called from grid (from only one thread)
  inline void clear() { ols.clear(); }

  // called from grid (from only one thread)
  inline void add(object *o) { ols.push_back(o); }

  inline void print() {
    int i = 0;
    for (object *o : ols) {
      if (i++) {
        printf(", ");
      }
      printf("%s", o->name.c_str());
    }
    printf("\n");
  }

private:
  inline static void handle_sphere_collision(object *Oi, object *Oj,
                                             frame_context const &fc) {

    // check if Oi is subscribed to collision with Oj
    if ((Oi->collision_mask & Oj->collision_bits) == 0) {
      return;
    }

    // if object overlaps cells this code might be called by several threads at
    // the same time
    const bool synchronization_necessary =
        grid_threaded and Oi->is_overlaps_cells();

    if (synchronization_necessary) {
      Oi->acquire_lock();
    }

    if (Oi->is_collision_handled_and_if_not_add(Oj)) {
      if (synchronization_necessary) {
        Oi->release_lock();
      }
      return;
    }

    // only one thread at a time can be here

    const glm::vec3 collision_normal =
        glm::normalize(Oj->position - Oi->position);

    const float relative_velocity_along_collision_normal =
        glm::dot(Oj->velocity - Oi->velocity, collision_normal);

    if (relative_velocity_along_collision_normal >= 0) {
      // objects are not moving towards each other
      if (synchronization_necessary) {
        Oi->release_lock();
      }
      return;
    }

    if (not Oi->is_dead() and Oi->on_collision(Oj, fc)) {
      Oi->set_is_dead();
      objects.free(Oi);
      if (synchronization_necessary) {
        Oi->release_lock();
      }
      return;
    }

    // resolve collision between the spheres

    constexpr float restitution = 1;
    const float impulse = (1.0f + restitution) *
                          relative_velocity_along_collision_normal /
                          (Oi->mass + Oj->mass);

    Oi->velocity += impulse * Oj->mass * collision_normal;

    // std::cout << Oi->name << ": new velocity: " << glm::to_string(Oi->velocity)
    //           << "\n";

    if (synchronization_necessary) {
      Oi->release_lock();
    }
  }

  inline static void dispatch_collision(object *Osrc, object *Otrg,
                                        frame_context const &fc) {

    // check if Oi is subscribed to collision with Oj
    if ((Osrc->collision_mask & Otrg->collision_bits) == 0) {
      return;
    }

    // if object overlaps cells this code might be called by several threads at
    // the same time
    const bool synchronization_necessary =
        grid_threaded and Osrc->is_overlaps_cells();

    if (synchronization_necessary) {
      Osrc->acquire_lock();
    }

    if (Osrc->is_collision_handled_and_if_not_add(Otrg)) {
      if (synchronization_necessary) {
        Osrc->release_lock();
      }
      return;
    }

    // only one thread at a time can be here

    if (not Osrc->is_dead() and Osrc->on_collision(Otrg, fc)) {
      Osrc->set_is_dead();
      objects.free(Osrc);
    }

    if (synchronization_necessary) {
      Osrc->release_lock();
    }
  }

  inline static auto are_bounding_spheres_in_collision(object *o1, object *o2,
                                                       frame_context const &fc)
      -> bool {

    const glm::vec3 v = o2->position - o1->position;
    const float d = o1->radius + o2->radius;
    const float dsq = d * d;
    const float vsq = glm::dot(v, v);
    const float diff = vsq - dsq;
    return diff < 0;
  }

  inline static void update_planes_world_coordinates(object *o) {
    const bool needs_synchronization = grid_threaded and o->is_overlaps_cells();

    if (needs_synchronization) {
      o->planes.acquire_lock();
    }

    const glob &g = globs.at(o->glob_ix);

    o->planes.update_model_to_world(g.planes_points, g.planes_normals,
                                    o->get_updated_Mmw(), o->position, o->angle,
                                    o->scale);

    if (needs_synchronization) {
      o->planes.release_lock();
    }
  }

  inline static auto are_bounding_planes_in_collision(object *o1, object *o2,
                                                      frame_context const &fc)
      -> bool {

    update_planes_world_coordinates(o1);
    update_planes_world_coordinates(o2);

    if (o1->planes.is_in_collision_with_planes(o2->planes)) {
      return true;
    }

    if (o2->planes.is_in_collision_with_planes(o1->planes)) {
      return true;
    }

    return false;
  }
};
} // namespace glos