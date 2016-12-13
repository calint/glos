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


inline static void cell_clear(cell*o){
	dynp_clear(&o->objrefs);
}

inline static void cell_free(cell*o){
	dynp_free(&o->objrefs);
}

inline static void cell_add_object(cell*o,object*oo){
	dynp_add(&o->objrefs,oo);
}

#define grid_cell_size 10
#define grid_ncells_wide 1
#define grid_ncells_high 1
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

inline static void grid_add(object*o){
	const float gw=grid_cell_size*grid_ncells_wide;
	const float gh=grid_cell_size*grid_ncells_high;

	const float r=o->b.r;

	const float xl=gw/2-o->p.p.x-r;
	const float zl=gh/2-o->p.p.z-r;

	const float xr=gw/2-o->p.p.x+r;
	const float zr=gh/2-o->p.p.z+r;

	const unsigned xil=(unsigned)(xl/gw);
	const unsigned xir=(unsigned)(xr/gw);

	const unsigned zil=(unsigned)(zl/gw);
	const unsigned zir=(unsigned)(zr/gw);

	for(unsigned z=zil;z<=zir;z++){
		for(unsigned x=xil;x<=xir;x++){
			cell*c=&cells[x][x];
			cell_add_object(c,o);
		}
	}
}

inline static void grid_free(){
	cell*p=&cells[0][0];
	unsigned i=grid_ncells;
	while(i--){
		cell_free(p++);
	}

}
