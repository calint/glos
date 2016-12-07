#pragma once
#include "glo.h"
#include"mat4.h"
//--------------------------------------------------------------------- storage
//#define glos_cap 16
//#define glos_assert_bounds
//static glo __glo[glos_cap];

static dynp glos=dynp_def;

//---------------------------------------------------------------------

inline static glo*glo_at(indx i){
	return dynp_get(&glos,i);
}

inline static const glo*glo_at_const(indx i){
	return dynp_get(&glos,i);
}

inline static void glos_init(){}
inline static void glos_free(){}
inline static void glos_render(){
	for(unsigned i=0;i<glos.count;i++){
		glo*g=dynp_get(&glos,i);
		if(g->ranges.count)
			glo_render(g++,mat4_ident);
	}
}


inline static void glos_load_obj_file(indx i,const char*path){
	glo*g=glo_from_file(path);
	dynp_add(&glos,g);
}

