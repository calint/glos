#pragma once
// reviewed: 2023-12-22

class cell final {
public:
  std::vector<object *> ols{};

  // called from grid (possibly by multiple threads)
  inline void update(const frame_ctx &fc) {
    for (object *o : ols) {
      if (grid_threaded and o->grid_ifc.is_overlaps_cells()) {
        // object is in several cells and may be called from multiple threads

        o->grid_ifc.acquire_lock();
        if (o->grid_ifc.updated_at_tick == fc.tick) {
          o->grid_ifc.release_lock();
          continue;
        }
        o->grid_ifc.updated_at_tick = fc.tick;
        o->grid_ifc.release_lock();
      } else {
        // object is in only one cell or can only be called from one thread
        if (o->grid_ifc.updated_at_tick == fc.tick) {
          continue;
        }
        o->grid_ifc.updated_at_tick = fc.tick;
      }

      // only one thread at a time gets to this code
      if (o->update(fc)) {
        o->grid_ifc.set_is_dead();
        objects.free(o);
        continue;
      }

      // note. opportunity to clear the list prior to 'resolve_collisions'
      o->grid_ifc.clear_handled_collisions();
    }
  }

  // called from grid (possibly by multiple threads)
  inline void resolve_collisions(const frame_ctx &fc) {
    // thread safe because 'ols' does not change during 'resolve_collisions'
    // phase
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
        if ((Oi->grid_ifc.collision_mask & Oj->grid_ifc.collision_bits) == 0 and
            (Oj->grid_ifc.collision_mask & Oi->grid_ifc.collision_bits) == 0) {
          continue;
        }

        // note. instead of checking if collision detection has been tried
        // between these 2 objects just try it because it is more expensive to
        // do the locking than just trying and only handling a collision
        // between 2 objects once

        if (not detect_collision_for_spheres(Oi, Oj, fc)) {
          continue;
        }

        handle_collision(Oi, Oj, fc);
        handle_collision(Oj, Oi, fc);
      }
    }
  }

  // called from grid (from only one thread)
  inline void render(const frame_ctx &fc) {
    for (object *o : ols) {
      if (o->grid_ifc.rendered_at_tick == fc.tick) {
        continue;
      }
      o->grid_ifc.rendered_at_tick = fc.tick;
      o->render(fc);
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
  inline static void handle_collision(object *Oi, object *Oj,
                                      const frame_ctx &fc) {

    // check if Oi is subscribed to collision with Oj
    if ((Oi->grid_ifc.collision_mask & Oj->grid_ifc.collision_bits) == 0) {
      return;
    }

    // if object overlaps cells this code might be called by several threads at
    // the same time
    const bool synchronization_necessary =
        grid_threaded and Oi->grid_ifc.is_overlaps_cells();

    if (synchronization_necessary) {
      Oi->acquire_lock();
    }

    if (Oi->grid_ifc.is_collision_handled_and_add_if_not(Oj)) {
      if (synchronization_necessary) {
        Oi->release_lock();
      }
      return;
    }

    // only one thread at a time can be here

    if (not Oi->grid_ifc.is_dead() and Oi->on_collision(Oj, fc)) {
      Oi->grid_ifc.set_is_dead();
      objects.free(Oi);
    }

    if (synchronization_necessary) {
      Oi->release_lock();
    }
  }

  //? works only in 1D with equal masses
  inline static bool detect_collision_for_spheres(object *o1, object *o2,
                                                  const frame_ctx &fc) {

    const glm::vec3 v = o2->physics.position - o1->physics.position;
    const float d = o1->volume.radius + o2->volume.radius;
    const float dsq = d * d;
    const float vsq = glm::dot(v, v);
    const float diff = vsq - dsq;
    return diff < 0;
  }
};
