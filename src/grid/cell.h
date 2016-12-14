#pragma once
#include"../lib.h"
#include"../obj/object.h"
#include"../obj/part.h"

typedef struct cell{
	dynp objrefs;
}cell;

#define cell_def {dynp_def}

inline static void cell_update(cell*o,framectx*fc){
	unsigned i=o->objrefs.count;

	if(i==0)
		return;

//	object*oi=*(object**)(o->objrefs.data);
	while(i--){
		object*oi=dynp_get(&o->objrefs,i);
		if(oi->updtk==fc->tick){
//			printf("[ grid ] skipped updated %s %p\n",oi->n.glo_ptr->name.data,(void*)oi);
//			printf("[ grid ] %u  %u\n",oi->updtk,fc->tick);
			oi++;
			continue;
		}
//		printf("[ grid ] updated %s %p\n",oi->n.glo_ptr->name.data,(void*)oi);
//		printf("[ grid ] %u  %u\n",oi->updtk,fc->tick);
		oi->updtk=fc->tick;
		if(oi->v.update){
			oi->v.update(oi,fc);
		}else{
			object_update(oi,fc);
		}
		metrics.objects_updated_prv_frame++;

		for(int i=0;i<object_part_cap;i++){
			if(!oi->part[i])
				continue;
			part*p=oi->part[i];
			if(p->update){
				p->update(oi,p,fc);
				metrics.parts_updated_prv_frame++;
			}
		}
		oi++;
	}
}

inline static void cell_render(cell*o,framectx*fc){
	unsigned i=o->objrefs.count;

	if(i==0)
		return;

//	object*oi=*(object**)(o->objrefs.data);
	while(i--){
		object*oi=dynp_get(&o->objrefs,i);
		if(oi->drwtk==fc->tick){
//			printf("[ grid ] skipped rendered %s\n",oi->n.glo_ptr->name.data);
//			printf("[ grid ] %u  %u\n",oi->updtk,fc->tick);
			oi++;
			continue;
		}
//		printf("[ grid ] rendered %s %p\n",oi->n.glo_ptr->name.data,(void*)oi);
//		printf("[ grid ] %u  %u\n",oi->updtk,fc->tick);
		oi->drwtk=fc->tick;

		if(oi->n.glo_ptr){
			const float*Mmw=object_get_updated_Mmw(oi);
			glo_render(oi->n.glo_ptr,Mmw);
		}

		if(oi->v.render)
			oi->v.render(oi,fc);

		metrics.objects_rendered_prv_frame++;

		for(int i=0;i<object_part_cap;i++){
			if(!oi->part[i])
				continue;
			part*p=oi->part[i];
			if(p->render){
				p->render(oi,p,fc);
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
inline static bool _cell_detect_and_resolve_collision_for_spheres1(
		object*o1,object*o2,framectx*fc){

	const vec4 v;vec3_minus(&v,&o2->p.p,&o1->p.p);
	const float d=o1->b.r+o2->b.r;
	const float dsq=d*d;
	const float vsq=vec3_dot(&v,&v);
	const float diff=dsq-vsq;
	if(!(vsq<dsq))//?
		return false;

	// in collision
	printf("frame: %u   %s vs %s\n",fc->tick,o1->name.data,o2->name.data);
	if(o1->g.collide_mask==0){// o1 is a bouncer
		vec3_negate(&o2->p.v);
	}else if(o2->g.collide_mask==0){// o1 is a bouncer
		vec3_negate(&o1->p.v);
	}else{// swap velocities
		const vec4 swap=o1->p.v;
		o1->p.v=o2->p.v;
		o2->p.v=swap;
	}
//	vec4 swap=o1->p.v;
//	o1->p.v=o2->p.v;
//	o2->p.v=swap;
	vec3_print(&o1->p.p);vec3_print(&o2->p.p);puts("");
	vec3_print(&o1->p.v);vec3_print(&o2->p.v);puts("");
	return true;
}

inline static bool _cell_detect_and_resolve_collision_for_spheres(
		object*o1,object*o2,framectx*fc){

		const vec4 v;vec3_minus(&v,&o2->p.p,&o1->p.p);
		const float d=o1->b.r+o2->b.r;
		const float dsq=d*d;
		const float vsq=vec3_dot(&v,&v);
		const float diff=dsq-vsq;
//		if(diff>-.000001f){
//			return false;
//		}
		if(!(vsq<dsq))//?
			return false;
//		puts("");

//	printf(" frame: %u\n",fc->tick);
	// partial dt to collision
	const float x1=o1->p.p.x;
	const float u1=o1->p.v.x;
	const float r1=o1->b.r;
	const float x2=o2->p.p.x;
	const float u2=o2->p.v.x;
	const float r2=o1->b.r;
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
	const float div=u2-u1;
	if(div==0){
//		fprintf(stderr,"\n%s:%u: was in collision already\n",__FILE__,__LINE__);
//		stacktrace_print(stderr);
//		fprintf(stderr,"\n\n");
//		exit(-1);
//		// not collision, ? precision 9.53674316e-07
		printf(" divisor is zero   %s  %s  diff: %f\n",
				o1->name.data,o2->name.data,diff);
		return false;
	}
	float t;
	if(x1<x2){
		t=(x1-x2+r1+r2)/div;
	}else if(x1>x2){
		t=(x1-x2-r1-r2)/div;
	}else{
		fprintf(stderr,"\n%s:%u: ??\n",__FILE__,__LINE__);
		stacktrace_print(stderr);
		fprintf(stderr,"\n\n");
		exit(-1);
	}

	// move to collision
	vec3_inc_with_vec3_over_dt(&o1->p_nxt.p,&o1->p_nxt.v,t);//*0.99999f for resolved
	vec3_inc_with_vec3_over_dt(&o2->p_nxt.p,&o2->p_nxt.v,t);

	// validate
//	{
////		const float dist=o2->p_nxt.p.x-o1->p_nxt.p.x;
//		vec4 vv;vec3_minus(&vv,&o2->p_nxt.p,&o1->p_nxt.p);
//		const float d=o1->b.r+o2->b.r;
//		const float dsq=d*d;
//		const float epsilon=0;//.001f;
//		const float vsq=vec3_dot(&vv,&vv)+epsilon;
//		if(vsq<dsq){
//			fprintf(stderr,"\n%s:%u: collision not fully resolved\n",__FILE__,__LINE__);
//			stacktrace_print(stderr);
//			fprintf(stderr,"\n\n");
//			exit(-1);
//		}
//	}

	// swap velocity
	if(o1->g.collide_mask==0){// o1 is a bouncer
		vec3_negate(&o2->p_nxt.v);
	}else if(o2->g.collide_mask==0){// o1 is a bouncer
		vec3_negate(&o1->p_nxt.v);
	}else{// swap velocities
		vec4 swap_from_o1=o1->p_nxt.v;
		vec4 swap_from_o2=o2->p_nxt.v;
		o1->p_nxt.v=swap_from_o2;
		o2->p_nxt.v=swap_from_o1;
		//	// move in new direction
	}
	//? discards the rest of t, toavoidoverlap?
//	vec3_inc_with_vec3_over_dt(&o1->p_nxt.p,&o1->p_nxt.v,-t);
//	vec3_inc_with_vec3_over_dt(&o2->p_nxt.p,&o2->p_nxt.v,-t);

	return true;
}

inline static bool _cell_checked_collisions(
		object*o1,object*o2,framectx*fc){

	metrics.collision_grid_overlap_check++;

	if(o1->g.tick != fc->tick){ // list out of date
		dynp_clear(&o1->g.checked_collisions_list);
		o1->g.tick = fc->tick;
	}
	if(o2->g.tick != fc->tick){ // list out of date
		dynp_clear(&o2->g.checked_collisions_list);
		o2->g.tick = fc->tick;
	}
	if(dynp_has(&o1->g.checked_collisions_list,o2) ||
		dynp_has(&o2->g.checked_collisions_list,o1)){
		return true;
	}
	return false;
}

inline static void cell_resolve_collisions(cell*o,framectx*fc){
//	printf("[ cell %p ] detect collisions\n",(void*)o);
	dynp*ls=&o->objrefs;
	for(unsigned i=0;i<ls->count-1;i++){
		for(unsigned j=i+1;j<ls->count;j++){
			metrics.collision_detections_possible_prv_frame++;

			object*Oi=dynp_get(ls,i);
			object*Oj=dynp_get(ls,j);

			if(!((Oi->g.collide_mask & Oj->g.collide_bits) ||
				(Oj->g.collide_mask & Oi->g.collide_bits ))){
//				printf("not tried [ cell ][ collision ][ %p ][ %p ]\n",(void*)Oi,(void*)Oj);
				continue;
			}

			if(is_bit_set(&Oi->g.bits,grid_ifc_overlaps) &&
				is_bit_set(&Oj->g.bits,grid_ifc_overlaps)){
				// both overlap grids, this detection might have been tried

//				printf("[ cell %p ][ coldet ][ %p ][ %p ] overlapp\n",(void*)o,
//						(void*)Oi,(void*)Oj);

				if(_cell_checked_collisions(Oi,Oj,fc)){
//					printf("[ cell %p ][ coldet ][ %p ][ %p ] already checked\n",(void*)o,
//							(void*)Oi,(void*)Oj);
					continue;
				}
			}

			dynp_add(&Oi->g.checked_collisions_list,Oj);
//			dynp_add(&Oj->g.checked_collisions_list,Oi);//?

			metrics.collision_detections_considered_prv_frame++;

			if(!_cell_detect_and_resolve_collision_for_spheres(Oi,Oj,fc)){
//				printf("[ cell %p ][ coldet ][ %s ][ %s ] not in collision\n",
//						(void*)o,(void*)Oi->name.data,(void*)Oj->name.data);
				continue;
			}

			printf("[ %u ][ cell %p ][ coldet ]  '%s' and '%s'\n",
					fc->tick,(void*)o,Oi->name.data,Oj->name.data);

			metrics.collision_detections_prv_frame++;

			if(Oi->g.collide_mask & Oj->g.collide_bits){
				if(Oi->v.collision)
					Oi->v.collision(Oi,Oj,fc);
			}

			if(Oj->g.collide_mask & Oi->g.collide_bits){
				if(Oj->v.collision)
					Oj->v.collision(Oj,Oi,fc);
			}

		}
	}
//	object* *ptr_ls=o->objrefs.data;
//	object* ptr_obj_first=*ptr_ls;

}

inline static void cell_print(cell*o){
	printf("cell{%u}",o->objrefs.count);
}

inline static void cell_clear(cell*o){
	dynp_clear(&o->objrefs);
}

inline static void cell_free(cell*o){
	dynp_free(&o->objrefs);
}

inline static void cell_add_object(cell*o,object*oo){
	dynp_add(&o->objrefs,oo);
//	oo->g.main_cell=o;
}

