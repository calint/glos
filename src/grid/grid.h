#pragma once
#include"../lib.h"

typedef struct cell{
	dynp objrefs;
}cell;

#define cell_def {dynp_def}

inline static void cell_update(cell*o,dt dt){
	unsigned i=o->objrefs.count;
	object*oi=(object*)o->objrefs.data;
	while(i--){
		object_update(oi++,dt);//? overlapping updates
	}
}

inline static void cell_render(cell*o){
	unsigned i=o->objrefs.count;
	object*oi=(object*)o->objrefs.data;
	while(i--){
		object_render(oi++);//? overlapping updates
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

#define grid_cell_size 20
#define grid_ncells_wide 8
#define grid_ncells_high 8
#define grid_ncells grid_ncells_wide*grid_ncells_high

static cell cells[grid_ncells_wide][grid_ncells_high];

inline static void grid_init(){}

inline static void grid_update(dt dt){
	cell*p=&cells[0][0];
	unsigned i=grid_ncells;
	while(i--){
		cell_update(p++,dt);
	}
}

inline static void grid_collisions(dt dt){}

inline static void grid_render(){
	cell*p=&cells[0][0];
	unsigned i=grid_ncells;
	while(i--){
		cell_render(p++);
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
//	printf("grid{");
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
