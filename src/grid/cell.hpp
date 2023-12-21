#pragma once
#include "../lib.h"
#include "../obj/object.hpp"
#include <algorithm>

class cell {
public:
  std::vector<object *> objects{};

  inline void update(const frame_ctx &fc) {
    for (object *oi : objects) {
      if (oi->update_tick == fc.tick) {
        continue;
      }
      oi->update_tick = fc.tick;
      oi->update(fc);
      metrics.objects_updated_prv_frame++;
    }
  }

  inline void render(const frame_ctx &fc) {
    for (object *oi : objects) {
      if (oi->draw_tick == fc.tick) {
        continue;
      }
      oi->draw_tick = fc.tick;
      oi->render(fc);
      metrics.objects_rendered_prv_frame++;
    }
  }

  inline void print() { printf("cell{%zu}", objects.size()); }

  inline void clear() { objects.clear(); }

  inline void add(object *o) { objects.push_back(o); }

  inline void resolve_collisions(const frame_ctx &fc) {
    //	printf("[ cell %p ] detect collisions\n",(void*)o);
    const unsigned len = objects.size();
    for (unsigned i = 0; i < len - 1; i++) {
      for (unsigned j = i + 1; j < len; j++) {
        metrics.collision_detections_possible_prv_frame++;

        object *Oi = objects.at(i);
        object *Oj = objects.at(j);

        if (!((Oi->grid_ifc.collision_mask & Oj->grid_ifc.collision_bits) ||
              (Oj->grid_ifc.collision_mask & Oi->grid_ifc.collision_bits))) {
          continue;
        }

        if (is_bit_set(&Oi->grid_ifc.bits, grid_ifc_overlaps) &&
            is_bit_set(&Oj->grid_ifc.bits, grid_ifc_overlaps)) {
          // both overlap grids, this detection might have been tried
          if (is_collision_checked(Oi, Oj, fc)) {
            continue;
          }
        }

        Oi->grid_ifc.checked_collisions_list.push_back(Oj);
        metrics.collision_detections_considered_prv_frame++;

        if (!detect_and_resolve_collision_for_spheres(Oi, Oj, fc)) {
          continue;
        }

        metrics.collision_detections_prv_frame++;

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
      o1->grid_ifc.checked_collisions_list.clear();
      o1->grid_ifc.tick = fc.tick;
    }
    if (o2->grid_ifc.tick != fc.tick) { // list out of date
      o2->grid_ifc.checked_collisions_list.clear();
      o2->grid_ifc.tick = fc.tick;
    }

    return is_in_checked_collision_list(o1, o2) or
           is_in_checked_collision_list(o2, o1);
  }

  inline static bool is_in_checked_collision_list(object *src, object *trg) {
    auto it = std::find(src->grid_ifc.checked_collisions_list.begin(),
                        src->grid_ifc.checked_collisions_list.end(), trg);
    return it != src->grid_ifc.checked_collisions_list.end();
  }

  inline static int
  detect_and_resolve_collision_for_spheres(object *o1, object *o2,
                                           const frame_ctx &fc) {

    vec4 v;
    vec3_minus(&v, &o2->physics.position, &o1->physics.position);
    const float d = o1->volume.radius +
                    o2->volume.radius; // minimum distance
    const float dsq = d * d;                    //  squared
    const float vsq = vec3_dot(&v, &v);         // distance of vector squared
    const float diff = vsq - dsq;               //
    if (diff >= 0) {                            //
      return 0;
    }
    //
    //	if(fabs(diff)<.01f){
    //		printf("   ... diff within error margin  %f\n",diff);
    //		return false;
    //	}

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
      stacktrace_print(stderr);
      fprintf(stderr, "\n\n");
      exit(-1);
    }

    // move objects to moment off collision
    vec3_inc_with_vec3_over_dt(&o1->physics_nxt.position,
                               &o1->physics_nxt.velocity, t);
    vec3_inc_with_vec3_over_dt(&o2->physics_nxt.position,
                               &o2->physics_nxt.velocity, t);

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
      vec3_negate(&o2->physics_nxt.velocity);
    } else if (o2->grid_ifc.collision_mask == 0) { // o2 is a bouncer
      vec3_negate(&o1->physics_nxt.velocity);
    } else { // swap velocities
      vec4 swap_from_o1 = o1->physics_nxt.velocity;
      vec4 swap_from_o2 = o2->physics_nxt.velocity;
      o1->physics_nxt.velocity = swap_from_o2;
      o2->physics_nxt.velocity = swap_from_o1;
    }
    // rest of t
    vec3_inc_with_vec3_over_dt(&o1->physics_nxt.position,
                               &o1->physics_nxt.velocity, -t);
    vec3_inc_with_vec3_over_dt(&o2->physics_nxt.position,
                               &o2->physics_nxt.velocity, -t);

    return 1;
  }
};
