#pragma once
// reviewed: 2023-12-22
// reviewed: 2024-01-04
// reviewed: 2024-01-06
// reviewed: 2024-01-10
// reviewed: 2024-01-16

namespace glos {

class cell final {
  // object entry in a cell. members used in the hot code path copied for better
  // cache utilization
  struct entry {
    glm::vec3 position{};
    float radius = 0;
    uint32_t collision_bits = 0;
    uint32_t collision_mask = 0;
    object *object = nullptr;
  };

  // entry in list of objects whose bounding spheres are in collision. further
  // processed to check for collision between objects with bounding planes
  struct collision {
    object *o1 = nullptr;
    object *o2 = nullptr;
    bool Oi_subscribed_to_collision_with_Oj = false;
    bool Oj_subscribed_to_collision_with_Oi = false;
    bool is_collision = false;
  };

  std::vector<entry> entry_list{};
  std::vector<collision> check_collisions_list{};
  random_numbers random_numbers{};

public:
  // called from grid
  inline auto update() -> void {
    object_context ctx{frame_context.dt, random_numbers};
    for (entry const &ce : entry_list) {
      if (threaded_grid) {
        // multithreaded mode
        if (ce.object->overlaps_cells) [[unlikely]] {
          // object is in several cells and may be called from multiple threads
          ce.object->acquire_lock();
          if (ce.object->updated_at_tick == frame_context.frame_num) {
            // object already updated in a different cell by a different thread
            ce.object->release_lock();
            continue;
          }
          ce.object->updated_at_tick = uint32_t(frame_context.frame_num);
          ce.object->release_lock();
        }
      } else {
        // single threaded mode
        if (ce.object->overlaps_cells) [[unlikely]] {
          // object is in several cells and may be called from multiple cells
          if (ce.object->updated_at_tick == frame_context.frame_num) {
            // already called from a different cell
            continue;
          }
          ce.object->updated_at_tick = uint32_t(frame_context.frame_num);
        }
      }

      // only one thread at a time is here for 'ce.object'

      if (!ce.object->update(ctx)) {
        ce.object->is_dead = true;
        objects.free(ce.object);
        continue;
      }

      // note: opportunity to clear the list prior to 'resolve_collisions'
      ce.object->clear_handled_collisions();
    }
  }

  inline auto resolve_collisions() -> void {
    make_check_collisions_list();
    process_check_collisions_list();
    handle_check_collisions_list();
  }

  // called from grid (from only one thread)
  inline auto render() const -> void {
    for (entry const &ce : entry_list) {
      if (ce.object->overlaps_cells) [[unlikely]] {
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
  inline auto clear() -> void { entry_list.clear(); }

  // called from grid (from only one thread)
  inline auto add(object *o) -> void {
    entry_list.emplace_back(o->position, o->bounding_radius, o->collision_bits,
                            o->collision_mask, o);
  }

  inline auto print() const -> void {
    uint32_t i = 0;
    for (entry const &ce : entry_list) {
      if (i++) {
        printf(", ");
      }
      printf("%s", ce.object->name.c_str());
    }
    printf("\n");
  }

  inline auto objects_count() const -> size_t { return entry_list.size(); }

private:
  // called from grid (possibly by multiple threads)
  inline auto make_check_collisions_list() -> void {
    check_collisions_list.clear();

    // thread safe because 'entry_list' does not change during
    // 'resolve_collisions'
    size_t const len = entry_list.size();
    if (len < 2) {
      return;
    }

    for (uint32_t i = 0; i < len - 1; ++i) {
      for (uint32_t j = i + 1; j < len; ++j) {

        entry const &Oi = entry_list[i];
        entry const &Oj = entry_list[j];

        bool const Oi_subscribed_to_collisions_with_Oj =
            Oi.collision_mask & Oj.collision_bits;

        bool const Oj_subscribed_to_collisions_with_Oi =
            Oj.collision_mask & Oi.collision_bits;

        if ((Oi_subscribed_to_collisions_with_Oj ||
             Oj_subscribed_to_collisions_with_Oi) &&
            bounding_spheres_are_in_collision(Oi, Oj)) {

          check_collisions_list.emplace_back(
              Oi.object, Oj.object, Oi_subscribed_to_collisions_with_Oj,
              Oj_subscribed_to_collisions_with_Oi);
        }
      }
    }
  }

  inline auto process_check_collisions_list() -> void {
    for (collision &cc : check_collisions_list) {
      // bounding spheres are in collision
      object *Oi = cc.o1;
      object *Oj = cc.o2;

      if (Oi->is_sphere && Oj->is_sphere) {
        cc.is_collision = true;
        continue;
      }

      // check if sphere vs planes

      if (Oi->is_sphere) {
        // Oj is not a sphere
        Oj->update_planes_world_coordinates();
        if (Oj->planes.are_in_collision_with_sphere(Oi->position,
                                                    Oi->bounding_radius)) {
          cc.is_collision = true;
        }
        continue;
      }

      if (Oj->is_sphere) {
        // Oi is not a sphere
        Oi->update_planes_world_coordinates();
        if (Oi->planes.are_in_collision_with_sphere(Oj->position,
                                                    Oj->bounding_radius)) {
          cc.is_collision = true;
        }
        continue;
      }

      // both objects are convex volumes bounded by planes

      if (bounding_planes_are_in_collision(Oi, Oj)) {
        cc.is_collision = true;
      }
    }
  }

  inline auto handle_check_collisions_list() -> void {
    object_context ctx{frame_context.dt, random_numbers};
    for (collision const &cc : check_collisions_list) {
      if (!cc.is_collision) {
        continue;
      }

      // objects are in collision
      object *Oi = cc.o1;
      object *Oj = cc.o2;

      if (Oi->is_sphere && Oj->is_sphere) {
        bool const Oi_already_handled_Oj = cc.Oi_subscribed_to_collision_with_Oj
                                               ? dispatch_collision(ctx, Oi, Oj)
                                               : false;
        bool const Oj_already_handled_Oi = cc.Oj_subscribed_to_collision_with_Oi
                                               ? dispatch_collision(ctx, Oj, Oi)
                                               : false;

        // check if collision has already been handled, possibly on a different
        // thread in a different cell
        if (!Oi_already_handled_Oj && !Oj_already_handled_Oi) [[likely]] {
          // collision has not been handled during this frame by any cell
          handle_sphere_collision(Oi, Oj);
        }
        continue;
      }

      if (cc.Oi_subscribed_to_collision_with_Oj) {
        dispatch_collision(ctx, Oi, Oj);
      }

      if (cc.Oj_subscribed_to_collision_with_Oi) {
        dispatch_collision(ctx, Oj, Oi);
      }
    }
  }

  static inline auto handle_sphere_collision(object *Oi, object *Oj) -> void {
    // synchronize objects that overlap cells

    if (threaded_grid && Oi->overlaps_cells) {
      Oi->acquire_lock();
    }
    if (threaded_grid && Oj->overlaps_cells) {
      Oj->acquire_lock();
    }

    glm::vec3 const collision_normal =
        glm::normalize(Oj->position - Oi->position);

    float const relative_velocity_along_collision_normal =
        glm::dot(Oj->velocity - Oi->velocity, collision_normal);

    if (relative_velocity_along_collision_normal >= 0 ||
        std::isnan(relative_velocity_along_collision_normal)) {
      // sphere are not moving towards each other
      if (threaded_grid && Oj->overlaps_cells) {
        Oj->release_lock();
      }
      if (threaded_grid && Oi->overlaps_cells) {
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

    if (threaded_grid && Oj->overlaps_cells) {
      Oj->release_lock();
    }
    if (threaded_grid && Oi->overlaps_cells) {
      Oi->release_lock();
    }
  }

  // returns true if collision with 'obj' has already been handled by 'receiver'
  static inline auto dispatch_collision(object_context &ctx, object *receiver,
                                        object *obj) -> bool {
    // if object overlaps cells then this code might be called by several
    // threads at the same time from different cells
    bool const synchronize = threaded_grid && receiver->overlaps_cells;

    if (synchronize) {
      receiver->acquire_lock();
    }

    // if both objects overlap cells then the same collision is detected &&
    // dispatched in multiple cells
    if (receiver->overlaps_cells && obj->overlaps_cells &&
        receiver->is_collision_handled_and_if_not_add(obj)) {
      // collision already dispatched for this 'receiver' and 'obj'
      if (synchronize) {
        receiver->release_lock();
      }
      return true;
    }

    // only one thread at a time can be here for 'receiver'

    if (!receiver->is_dead && !receiver->on_collision(ctx, obj)) {
      receiver->is_dead = true;
      objects.free(receiver);
    }

    if (synchronize) {
      receiver->release_lock();
    }

    return false;
  }

  static inline auto
  bounding_spheres_are_in_collision(entry const &ce1,
                                    entry const &ce2) -> bool {

    glm::vec3 const v = ce2.position - ce1.position;
    float const d = ce1.radius + ce2.radius;
    float const dsq = d * d;
    float const vsq = glm::dot(v, v); // distance squared
    float const diff = vsq - dsq;
    return diff < 0;
  }

  static inline auto bounding_planes_are_in_collision(object *o1,
                                                      object *o2) -> bool {

    o1->update_planes_world_coordinates();
    o2->update_planes_world_coordinates();

    // planes can be update only once per 'resolve_collisions' pass because
    // bounding plane objects state do not change because there is no handle
    // collision implementation such as spheres do

    return planes::are_in_collision(o1->planes, o2->planes);
  }
};
} // namespace glos