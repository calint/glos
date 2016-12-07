#pragma once
#include "glo.h"
#include"mat4.h"
//--------------------------------------------------------------------- storage
#define glos_cap 16
#define glos_assert_bounds
static glo __glo[glos_cap];

inline static void _glos_assert_index_(indx i){
#ifdef glos_assert_bounds
	if(i>=glos_cap){
		fprintf(stderr,"\nout of bounds\n");
		fprintf(stderr,"\n    %s %d\n",__FILE__,__LINE__);
		exit(-1);
	}
#endif
}
//---------------------------------------------------------------------

inline static glo*glo_at(indx i){
	_glos_assert_index_(i);
	return&__glo[i];
}

inline static const glo*glo_at_const(indx i){
	_glos_assert_index_(i);
	return&__glo[i];
}

inline static void glos_init(){}
inline static void glos_free(){}
inline static void glos_render(){
	glo*g=__glo;
	int n=glos_cap;
	while(n--){
		if(g->ranges.count)
			glo_render(g++,mat4_ident);
	}
}


inline static void globs_load_obj_file(indx i,const char*path){
	_glos_assert_index_(i);
	__glo[i]=glo_from_file(path);
}

