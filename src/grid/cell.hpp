#pragma once
// reviewed: 2023-12-22

class cell final {
public:
  std::vector<object *> ols{};

  // called from grid (possibly by multiple threads)
  inline void update(const frame_ctx &fc) {
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
  inline void resolve_collisions(const frame_ctx &fc) {
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

        if (not detect_collision_for_spheres(Oi, Oj, fc)) {
          continue;
        }

        handle_collision(Oi, Oj, fc);
        handle_collision(Oj, Oi, fc);
      }
    }
  }

  // called from grid (from only one thread)
  inline void render(const unsigned render_frame_num) {
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
  inline static void handle_collision(object *Oi, object *Oj,
                                      const frame_ctx &fc) {

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
        glm::normalize(Oj->physics.position - Oi->physics.position);

    const float relative_velocity =
        glm::dot(Oj->physics.velocity - Oi->physics.velocity, collision_normal);

    constexpr float restitution = 1;
    if (relative_velocity < 0) {
      // objects are moving towards each other

      if (not Oi->is_dead() and Oi->on_collision(Oj, fc)) {
        Oi->set_is_dead();
        objects.free(Oi);
      }

      const float impulse = -(1.0f + restitution) * relative_velocity /
                            (1.0f / Oi->physics.mass + 1.0f / Oj->physics.mass);

      Oi->physics_nxt.velocity -= impulse / Oi->physics.mass * collision_normal;
    }

    if (synchronization_necessary) {
      Oi->release_lock();
    }
  }

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
