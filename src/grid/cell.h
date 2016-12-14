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
		if(oi->v.render)
			oi->v.render(oi,fc);
//		object_render_glo(oi);
		metrics.objects_rendered_prv_frame++;

		for(int i=0;i<object_part_cap;i++){
			if(!oi->part[i])
				continue;
			part*p=oi->part[i];
			if(p->render){
				p->render(oi,p,fc);
				metrics.parts_updated_prv_frame++;
			}
		}
		oi++;
	}
}

inline static int solve_3d_are_spheres_in_collision(
		const position*s1,float r1,
		const position*s2,float r2){

	const vec4 v;vec3_minus(&v,s2,s1);// vector from sphere 1 to 2
	const float d=r1+r2;// minimum distance
	const float d2=d*d;
	const float vlensq=vec3_dot(&v,&v);
	if(vlensq<d2){
		return 1;
	}
	return 0;
}

inline static int grid_checked_collisions(
		object*o1,object*o2,framectx*fc){
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
		return 1;
	}
	return 0;
}

inline static void cell_collisions(cell*o,framectx*fc){
//	printf("[ cell %p ] detect collisions\n",(void*)o);
	dynp*ls=&o->objrefs;
	for(unsigned i=0;i<ls->count-1;i++){
		for(unsigned j=i+1;j<ls->count;j++){
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

				if(grid_checked_collisions(Oi,Oj,fc)){
//					printf("[ cell %p ][ coldet ][ %p ][ %p ] already checked\n",(void*)o,
//							(void*)Oi,(void*)Oj);
					continue;
				}
			}

			dynp_add(&Oi->g.checked_collisions_list,Oj);
//			dynp_add(&Oj->g.checked_collisions_list,Oi);//?

			metrics.collision_detections_prv_frame++;

			if(!solve_3d_are_spheres_in_collision(
					&Oi->p.p, Oi->b.r,
					&Oj->p.p, Oj->b.r)){

//				printf("[ cell %p ][ coldet ][ %p ][ %p ] not in collision\n",
//						(void*)o,(void*)Oi,(void*)Oj);
				continue;
			}

//			printf("[ cell %p ][ coldet ][ %p ][ %p ] collision\n",(void*)o,
//					(void*)Oi,(void*)Oj);

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

