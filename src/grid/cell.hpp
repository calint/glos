#pragma once
// reviewed: 2023-12-22

#include "../obj/object.hpp"
#include <algorithm>

class cell {
public:
  static constexpr unsigned bit_overlaps = 0;

  std::vector<object *> objects{};

  inline void update(const frame_ctx &fc) {
    for (object *oi : objects) {
      //? if object overlaps grid in multithreaded use atomic int
      if (oi->update_tick == fc.tick) {
        continue;
      }
      oi->update_tick = fc.tick;
      oi->update(fc);
      metrics.objects_updated++;
    }
  }

  inline void render(const frame_ctx &fc) {
    for (object *oi : objects) {
      if (oi->render_tick == fc.tick) {
        continue;
      }
      oi->render_tick = fc.tick;
      oi->render(fc);
      metrics.objects_rendered++;
    }
  }

  inline void clear() { objects.clear(); }

  inline void add(object *o) { objects.push_back(o); }

  inline void print() {
    int i = 0;
    for (object *o : objects) {
      if (i++) {
        printf(", ");
      }
      printf("%s", o->name.c_str());
    }
    printf("\n");
  }

  inline void resolve_collisions(const frame_ctx &fc) {
    const unsigned len = objects.size();
    if (len == 0) {
      return;
    }
    for (unsigned i = 0; i < len - 1; i++) {
      for (unsigned j = i + 1; j < len; j++) {
        metrics.collision_detections_possible++;

        object *Oi = objects.at(i);
        object *Oj = objects.at(j);

        if (!((Oi->grid_ifc.collision_mask & Oj->grid_ifc.collision_bits) or
              (Oj->grid_ifc.collision_mask & Oi->grid_ifc.collision_bits))) {
          continue;
        }

        if (is_bit_set(Oi->grid_ifc.bits, bit_overlaps) and
            is_bit_set(Oj->grid_ifc.bits, bit_overlaps)) {
          // both objects overlap grids
          if (is_collision_checked(Oi, Oj, fc)) {
            continue;
          }
          Oi->grid_ifc.checked_collisions.push_back(Oj);
          // note. only entry in one of the objects necessary
        }

        metrics.collision_detections_considered++;

        if (not detect_and_resolve_collision_for_spheres(Oi, Oj, fc)) {
          continue;
        }

        metrics.collision_detections++;

        if (Oi->grid_ifc.collision_mask & Oj->grid_ifc.collision_bits) {
          Oi->on_collision(Oj, fc);
        }

        if (Oj->grid_ifc.collision_mask & Oi->grid_ifc.collision_bits) {
          Oj->on_collision(Oi, fc);
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
