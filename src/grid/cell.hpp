#pragma once
// reviewed: 2023-12-22

#include "../obj/object.hpp"
#include <algorithm>

class cell {
public:
  static constexpr unsigned bit_overlaps = 0;
  static constexpr unsigned bit_is_dead = 1;

  std::vector<object *> ols{};

  inline void update(const frame_ctx &fc) {
    for (object *o : ols) {
      //? if object overlaps grid in multithreaded use atomic int
      if (o->update_tick == fc.tick) {
        continue;
      }
      o->update_tick = fc.tick;
      if (o->update(fc)) {
        set_bit(o->grid_ifc.bits, bit_is_dead);
        objects.free(o);
      }
      metrics.objects_updated++;
    }
  }

  inline void render(const frame_ctx &fc) {
    for (object *o : ols) {
      if (o->render_tick == fc.tick) {
        continue;
      }
      o->render_tick = fc.tick;
      o->render(fc);
      metrics.objects_rendered++;
    }
  }

  inline void clear() { ols.clear(); }

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

  inline void resolve_collisions(const frame_ctx &fc) {
    const unsigned len = ols.size();
    if (len == 0) {
      return;
    }
    for (unsigned i = 0; i < len - 1; i++) {
      for (unsigned j = i + 1; j < len; j++) {
        metrics.collision_detections_possible++;

        object *Oi = ols.at(i);
        object *Oj = ols.at(j);

        // check if Oi and Oj have interest in collision with each other
        if ((Oi->grid_ifc.collision_mask & Oj->grid_ifc.collision_bits) == 0 and
            (Oj->grid_ifc.collision_mask & Oi->grid_ifc.collision_bits) == 0) {
          continue;
        }

        // check if both objects are dead
        if (is_bit_set(Oi->grid_ifc.bits, bit_is_dead) and
            is_bit_set(Oj->grid_ifc.bits, bit_is_dead)) {
          continue;
        }

        // check if both objects overlap grids in which case the collision
        // detection might have been done by another cell
        if (is_bit_set(Oi->grid_ifc.bits, bit_overlaps) and
            is_bit_set(Oj->grid_ifc.bits, bit_overlaps)) {
          // both objects overlap grids
          if (is_collision_checked(Oi, Oj, fc)) {
            // collision already checked by another cell
            continue;
          }
          // add Oj to Oi checked collisions list
          Oi->grid_ifc.checked_collisions.push_back(Oj);
          // note. only entry in one of the objects necessary
        }

        metrics.collision_detections_considered++;

        if (not detect_and_resolve_collision_for_spheres(Oi, Oj, fc)) {
          continue;
        }

        metrics.collision_detections++;

        if (not is_bit_set(Oi->grid_ifc.bits, bit_is_dead) and
            Oi->grid_ifc.collision_mask & Oj->grid_ifc.collision_bits) {
          if (Oi->on_collision(Oj, fc)) {
            set_bit(Oi->grid_ifc.bits, bit_is_dead);
            objects.free(Oi);
          }
        }

        if (not is_bit_set(Oj->grid_ifc.bits, bit_is_dead) and
            Oj->grid_ifc.collision_mask & Oi->grid_ifc.collision_bits) {
          if (Oj->on_collision(Oi, fc)) {
            set_bit(Oj->grid_ifc.bits, bit_is_dead);
            objects.free(Oj);
          }
        }
      }
    }
  }

private:
  inline static bool is_collision_checked(object *o1, object *o2,
                                          const frame_ctx &fc) {
    metrics.collision_grid_overlap_check++;

    if (o1->grid_ifc.tick != fc.tick) { // list out of date
      o1->grid_ifc.checked_collisions.clear();
      o1->grid_ifc.tick = fc.tick;
    }
    if (o2->grid_ifc.tick != fc.tick) { // list out of date
      o2->grid_ifc.checked_collisions.clear();
      o2->grid_ifc.tick = fc.tick;
    }

    return is_in_checked_collision_list(o1, o2) or
           is_in_checked_collision_list(o2, o1);
  }

  inline static bool is_in_checked_collision_list(object *src, object *trg) {
    auto it = std::find(src->grid_ifc.checked_collisions.begin(),
                        src->grid_ifc.checked_collisions.end(), trg);
    return it != src->grid_ifc.checked_collisions.end();
  }

  //? works only in 1D with equal masses
  inline static bool
  detect_and_resolve_collision_for_spheres(object *o1, object *o2,
                                           const frame_ctx &fc) {

    glm::vec3 v = o2->physics.position - o1->physics.position;
    const float d = o1->volume.radius + o2->volume.radius;
    const float dsq = d * d;
    const float vsq = glm::dot(v, v);
    const float diff = vsq - dsq;
    if (diff >= 0) {
      return false;
    }
    return true;

    // partial dt to collision
    const float x1 = o1->physics.position.x;
    const float u1 = o1->physics.velocity.x;
    const float r1 = o1->volume.radius;
    const float x2 = o2->physics.position.x;
    const float u2 = o2->physics.velocity.x;
    const float r2 = o1->volume.radius;
    // if x1<x2
    //   x1+u1*t+r1=x2+u2*t-r2
    // else if x1>x2
    //   x1+u1*t-r1=x2+u2*t+r2
    // else
    //   ?
    // for x1<x2
    //   x1+u1*t+r1=x2+u2*t-r2
    //   x1-x2+r1+r2=u2*t-u1*t
    //   (x1-x2+r1+r2)/(u2-u1)=t
    // for x1>x2
    //   x1+u1*t-r1=x2+u2*t+r2
    //   x1-x2-r1-r2=u2*t-u1*t
    //   (x1-x2-r1-r2)/(u2-u1)=t
    const float div = u2 - u1;
    if (div == 0) {
      //? number precision
      // printf(" *** du=0   diff=%f    dx=%f    %s x=%f    vs   %s x=%f\n",
      // diff,
      //        o2->physics.position.x - o1->physics.position.x,
      //        o1->name.c_str(), o1->physics.position.x, o2->name.c_str(),
      //        o2->physics.position.x);
      return 0;
    }
    float t;
    if (x1 < x2) {
      t = (x1 - x2 + r1 + r2) / div;
    } else if (x1 > x2) {
      t = (x1 - x2 - r1 - r2) / div;
    } else {
      fprintf(stderr, "\n%s:%u: x1==x2?\n", __FILE__, __LINE__);
      fprintf(stderr, "\n\n");
      exit(-1);
    }

    // move objects to moment off collision
    o1->physics_nxt.position += o1->physics_nxt.velocity * t;
    o2->physics_nxt.position += o2->physics_nxt.velocity * t;

    //	{
    //		// validate
    //		const float dist=o2->p_nxt.p.x-o1->p_nxt.p.x;
    //		vec4 vv;vec3_minus(&vv,&o2->p_nxt.p,&o1->p_nxt.p);
    //		const float d=o1->b.r+o2->b.r;
    //		const float dsq=d*d;
    //		const float epsilon=0;//.001f;
    //		const float vsq=vec3_dot(&vv,&vv)+epsilon;
    //		if(vsq<dsq){
    //			fprintf(stderr,"\n%s:%u: collision not fully
    // resolved\n",__FILE__,__LINE__); stacktrace_print(stderr);
    //			fprintf(stderr,"\n\n");
    //			exit(-1);
    //		}
    //	}

    // swap velocities
    if (o1->grid_ifc.collision_mask == 0) { // o1 is a bouncer
      o2->physics_nxt.velocity = -o2->physics_nxt.velocity;
    } else if (o2->grid_ifc.collision_mask == 0) { // o2 is a bouncer
      o1->physics_nxt.velocity = -o1->physics_nxt.velocity;
    } else { // swap velocities
      glm::vec3 swap_from_o1 = o1->physics_nxt.velocity;
      glm::vec3 swap_from_o2 = o2->physics_nxt.velocity;
      o1->physics_nxt.velocity = swap_from_o2;
      o2->physics_nxt.velocity = swap_from_o1;
    }
    // rest of t
    o1->physics_nxt.position += o1->physics_nxt.velocity * -t;
    o2->physics_nxt.position += o2->physics_nxt.velocity * -t;

    return true;
  }

public:
  inline static bool is_bit_set(const unsigned &bits,
                                int bit_number_starting_at_zero) {
    return bits & (1 << bit_number_starting_at_zero);
  }

  inline static void set_bit(unsigned &bits, int bit_number_starting_at_zero) {
    bits |= (1 << bit_number_starting_at_zero);
  }

  inline static void clear_bit(unsigned &bits,
                               int bit_number_starting_at_zero) {
    bits &= ~(1 << bit_number_starting_at_zero);
  }
};
