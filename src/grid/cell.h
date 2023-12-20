#pragma once
#include "../lib.h"
#include "../obj/object.hpp"
#include "../obj/part.hpp"

typedef struct cell {
  dynp objrefs;
} cell;

#define cell_def                                                               \
  { dynp_def }

inline static void cell_update(cell *o, framectx *fc) {
  unsigned i = o->objrefs.count;

  if (i == 0)
    return;

  //	object*oi=*(object**)(o->objrefs.data);
  while (i--) {
    object *oi = (object *)dynp_get(&o->objrefs, i);
    if (oi->update_tick == fc->tick) {
      //			printf("[ grid ] skipped updated %s
      //%p\n",oi->n.glo_ptr->name.data,(void*)oi);
      // printf("[ grid ] %u %u\n",oi->updtk,fc->tick);
      // oi++;
      continue;
    }
    //		printf("[ grid ] updated %s
    //%p\n",oi->n.glo_ptr->name.data,(void*)oi); 		printf("[ grid ]
    //%u %u\n",oi->updtk,fc->tick);
    oi->update_tick = fc->tick;
    if (oi->vtbl.update) {
      oi->vtbl.update(oi, fc);
    } else {
      oi->update(fc);
    }
    metrics.objects_updated_prv_frame++;

    for (int i = 0; i < object_part_cap; i++) {
      if (!oi->part[i])
        continue;
      part *p = (part *)oi->part[i];
      if (p->update) {
        p->update(oi, p, fc);
        metrics.parts_updated_prv_frame++;
      }
    }
    //		oi++;
  }
}

inline static void cell_render(cell *o, framectx *fc) {
  unsigned i = o->objrefs.count;

  if (i == 0)
    return;

  //	object*oi=*(object**)(o->objrefs.data);
  while (i--) {
    object *oi = (object *)dynp_get(&o->objrefs, i);
    if (oi->draw_tick == fc->tick) {
      //			printf("[ grid ] skipped rendered
      //%s\n",oi->n.glo_ptr->name.data); 			printf("[ grid
      //] %u %u\n",oi->updtk,fc->tick);
      oi++;
      continue;
    }
    //		printf("[ grid ] rendered %s
    //%p\n",oi->n.glo_ptr->name.data,(void*)oi); 		printf("[ grid ]
    //%u %u\n",oi->updtk,fc->tick);
    oi->draw_tick = fc->tick;

    if (oi->node.glo) {
      const float *Mmw = oi->get_updated_Mmw();
      glo_render(oi->node.glo, Mmw);
    }

    if (oi->vtbl.render) {
      oi->vtbl.render(oi, fc);
    }

    metrics.objects_rendered_prv_frame++;

    for (int i = 0; i < object_part_cap; i++) {
      if (!oi->part[i]) {
        continue;
      }
      part *p = (part *)oi->part[i];
      if (p->render) {
        p->render(oi, p, fc);
        metrics.parts_rendered_prv_frame++;
      }
    }
    oi++;
  }
}
//
// newton cradle cases:
//
//
//   o : ball
//   > : velocity
//   . : no velocity
//   * : magnitude vs vector
//
//     >.
//     oo
//     .>
//     . >
//
//     ><
//     oo
//     <>
//    <  >
//
//     .<<
//     ooo
//     <.<
//    < <.
//   < < .
//
//     ><<
//     ooo
//     *.<
//    < <.
//   < < .
//
//     >.<
//     ooo
//     .*.
//     <.>
//    < . >
inline static int
_cell_detect_and_resolve_collision_for_spheres1(object *o1, object *o2,
                                                framectx *fc) {

  vec4 v;
  vec3_minus(&v, &o2->physics.position, &o1->physics.position);
  const float d = o1->bounding_volume.radius + o2->bounding_volume.radius;
  const float dsq = d * d;
  const float vsq = vec3_dot(&v, &v);
  if (!(vsq < dsq)) //?
    return 0;

  const float diff = fabs(dsq - vsq);
  if (diff < .00001f) {
    printf("   ... diff within error margin  %f\n", diff);
  }

  // in collision
  printf("frame: %u   %s vs %s\n", fc->tick, o1->name.c_str(),
         o2->name.c_str());
  if (o1->grid_ifc.collide_mask == 0) { // o1 is a bouncer
    vec3_negate(&o2->physics.velocity);
  } else if (o2->grid_ifc.collide_mask == 0) { // o1 is a bouncer
    vec3_negate(&o1->physics.velocity);
  } else { // swap velocities
    const vec4 swap = o1->physics.velocity;
    o1->physics.velocity = o2->physics.velocity;
    o2->physics.velocity = swap;
  }
  //	vec4 swap=o1->p.v;
  //	o1->p.v=o2->p.v;
  //	o2->p.v=swap;
  vec3_print(&o1->physics.position);
  vec3_print(&o2->physics.position);
  puts("");
  vec3_print(&o1->physics.velocity);
  vec3_print(&o2->physics.velocity);
  puts("");
  return 1;
}

inline static int _cell_detect_and_resolve_collision_for_spheres(object *o1,
                                                                 object *o2,
                                                                 framectx *fc) {

  vec4 v;
  vec3_minus(&v, &o2->physics.position, &o1->physics.position);
  const float d = o1->bounding_volume.radius +
                  o2->bounding_volume.radius; // minimum distance
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
  const float r1 = o1->bounding_volume.radius;
  const float x2 = o2->physics.position.x;
  const float u2 = o2->physics.velocity.x;
  const float r2 = o1->bounding_volume.radius;
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
    printf(" *** du=0   diff=%f    dx=%f    %s x=%f    vs   %s x=%f\n", diff,
           o2->physics.position.x - o1->physics.position.x, o1->name.c_str(),
           o1->physics.position.x, o2->name.c_str(), o2->physics.position.x);
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
  if (o1->grid_ifc.collide_mask == 0) { // o1 is a bouncer
    vec3_negate(&o2->physics_nxt.velocity);
  } else if (o2->grid_ifc.collide_mask == 0) { // o2 is a bouncer
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

inline static int _cell_checked_collisions(object *o1, object *o2,
                                           framectx *fc) {

  metrics.collision_grid_overlap_check++;

  if (o1->grid_ifc.tick != fc->tick) { // list out of date
    dynp_clear(&o1->grid_ifc.checked_collisions_list);
    o1->grid_ifc.tick = fc->tick;
  }
  if (o2->grid_ifc.tick != fc->tick) { // list out of date
    dynp_clear(&o2->grid_ifc.checked_collisions_list);
    o2->grid_ifc.tick = fc->tick;
  }
  if (dynp_has(&o1->grid_ifc.checked_collisions_list, o2) ||
      dynp_has(&o2->grid_ifc.checked_collisions_list, o1)) {
    return 1;
  }
  return 0;
}

inline static void cell_resolve_collisions(cell *o, framectx *fc) {
  //	printf("[ cell %p ] detect collisions\n",(void*)o);
  dynp *ls = &o->objrefs;
  for (unsigned i = 0; i < ls->count - 1; i++) {
    for (unsigned j = i + 1; j < ls->count; j++) {
      metrics.collision_detections_possible_prv_frame++;

      object *Oi = (object *)dynp_get(ls, i);
      object *Oj = (object *)dynp_get(ls, j);
      // printf("%s vs %s\n",Oi->name.data,Oj->name.data);

      if (!((Oi->grid_ifc.collide_mask & Oj->grid_ifc.collide_bits) ||
            (Oj->grid_ifc.collide_mask & Oi->grid_ifc.collide_bits))) {
        //				printf("not tried [ cell ][ collision ][
        //%p
        //][ %p ]\n",(void*)Oi,(void*)Oj);
        continue;
      }

      if (is_bit_set(&Oi->grid_ifc.bits, grid_ifc_overlaps) &&
          is_bit_set(&Oj->grid_ifc.bits, grid_ifc_overlaps)) {
        // both overlap grids, this detection might have been tried

        //				printf("[ cell %p ][ coldet ][ %p ][ %p
        //] overlapp\n",(void*)o,
        // (void*)Oi,(void*)Oj);

        if (_cell_checked_collisions(Oi, Oj, fc)) {
          //					printf("[ cell %p ][ coldet ][
          //%p
          //][ %p ] already checked\n",(void*)o,
          // (void*)Oi,(void*)Oj);
          continue;
        }
      }

      dynp_add(&Oi->grid_ifc.checked_collisions_list, Oj);
      //			dynp_add(&Oj->g.checked_collisions_list,Oi);//?

      metrics.collision_detections_considered_prv_frame++;

      if (!_cell_detect_and_resolve_collision_for_spheres(Oi, Oj, fc)) {
        //				printf("[ cell %p ][ coldet ][ %s ][ %s
        //] not in collision\n",
        // (void*)o,(void*)Oi->name.data,(void*)Oj->name.data);
        continue;
      }

      //			printf("[ %u ][ cell %p ][ coldet ]  '%s' and
      //'%s'\n",
      // fc->tick,(void*)o,Oi->name.data,Oj->name.data);

      metrics.collision_detections_prv_frame++;

      if (Oi->grid_ifc.collide_mask & Oj->grid_ifc.collide_bits) {
        if (Oi->vtbl.collision)
          Oi->vtbl.collision(Oi, Oj, fc);
      }

      if (Oj->grid_ifc.collide_mask & Oi->grid_ifc.collide_bits) {
        if (Oj->vtbl.collision)
          Oj->vtbl.collision(Oj, Oi, fc);
      }
    }
  }
  //	object* *ptr_ls=o->objrefs.data;
  //	object* ptr_obj_first=*ptr_ls;
}

inline static void cell_print(cell *o) { printf("cell{%u}", o->objrefs.count); }

inline static void cell_clear(cell *o) { dynp_clear(&o->objrefs); }

inline static void cell_free(cell *o) { dynp_free(&o->objrefs); }

inline static void cell_add_object(cell *o, object *oo) {
  dynp_add(&o->objrefs, oo);
  //	oo->g.main_cell=o;
}
