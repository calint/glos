#pragma once
#include"../lib.h"
#include"../obj/object.h"

typedef struct cell{
	dynp objrefs;
}cell;

#define cell_def {dynp_def}


#define grid_cell_size 20
#define grid_ncells_wide 1
#define grid_ncells_high 1
#define grid_ncells grid_ncells_wide*grid_ncells_high

static cell cells[grid_ncells_high][grid_ncells_wide];

inline static void cell_update(cell*o,framectx*fc){
	unsigned i=o->objrefs.count;

	if(i==0)
		return;

	object*oi=(object*)(*o->objrefs.data);
	while(i--){
		if(oi->updtk==fc->tick){
//			printf("[ grid ] skipped updated %s %p\n",oi->n.glo_ptr->name.data,(void*)oi);
//			printf("[ grid ] %u  %u\n",oi->updtk,fc->tick);
			oi++;
			continue;
		}
//		printf("[ grid ] updated %s %p\n",oi->n.glo_ptr->name.data,(void*)oi);
//		printf("[ grid ] %u  %u\n",oi->updtk,fc->tick);
		oi->updtk=fc->tick;
		metrics.objects_updated_prv_frame++;
		if(oi->v.update){
			oi->v.update(oi,fc);
		}else{
			object_update(oi,fc);
		}

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

	object*oi=(object*)(*o->objrefs.data);
	while(i--){
		if(oi->drwtk==fc->tick){
//			printf("[ grid ] skipped rendered %s\n",oi->n.glo_ptr->name.data);
//			printf("[ grid ] %u  %u\n",oi->updtk,fc->tick);
			oi++;
			continue;
		}
//		printf("[ grid ] rendered %s %p\n",oi->n.glo_ptr->name.data,(void*)oi);
//		printf("[ grid ] %u  %u\n",oi->updtk,fc->tick);
		oi->drwtk=fc->tick;
		object_render_glo(oi);
		metrics.objects_rendered_prv_frame++;
		oi++;
	}
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
}

inline static void grid_init(){}

inline static void grid_update(framectx*fc){
	cell*p=&cells[0][0];
	unsigned i=grid_ncells;
//	printf("-------------------------\n");
	while(i--){
		cell_update(p,fc);
		p++;
	}
}

inline static void grid_collisions(framectx*fc){}

inline static void grid_render(framectx*fc){
	cell*p=&cells[0][0];
	unsigned i=grid_ncells;
	while(i--){
		cell_render(p++,fc);
	}
}

inline static void grid_clear(){
	cell*p=&cells[0][0];
	unsigned i=grid_ncells;
	while(i--){
		cell_clear(p++);
	}
}

inline static int clamp(int i,int min,int max_plus_one){
	if(i<min)
		return min;

	if(i>=max_plus_one)
		return max_plus_one-1;

	return i;
}

inline static void grid_add(object*o){
//	printf("[ grid ] add %s %p\n",o->n.glo_ptr->name.data,(void*)o);
	const float gw=grid_cell_size*grid_ncells_wide;
	const float gh=grid_cell_size*grid_ncells_high;

	const float r=o->b.r;

	const float xl=gw/2-o->p.p.x-r;
	const float xr=gw/2-o->p.p.x+r;
	const float zl=gh/2-o->p.p.z-r;
	const float zr=gh/2-o->p.p.z+r;

	int xil=(int)(xl/grid_cell_size);
	xil=clamp(xil,0,grid_ncells_wide);

	int xir=(int)((xr)/grid_cell_size);
	xir=clamp(xir,0,grid_ncells_wide);

	int zil=(int)(zl/grid_cell_size);
	zil=clamp(zil,0,grid_ncells_high);

	int zir=(int)((zr)/grid_cell_size);
	zir=clamp(zir,0,grid_ncells_high);

	for(int z=zil;z<=zir;z++){
		for(int x=xil;x<=xir;x++){
			cell*c=&cells[z][x];
			cell_add_object(c,o);
		}
	}
}

inline static void grid_print(){
	cell*p=&cells[0][0];
	unsigned i=grid_ncells;
	while(i--){
//		cell_print(p++);
		printf(" %d ",p++->objrefs.count);
		if(!(i%grid_ncells_wide))
			printf("\n");
	}
	printf("\n------------------------\n");
}

inline static void grid_free(){
	cell*p=&cells[0][0];
	unsigned i=grid_ncells;
	while(i--){
		cell_free(p++);
	}
}
