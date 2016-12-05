#pragma once
#include"glob.h"
#define globs_cap 1

//---------------------------------------------------------------------

static glob __globs[globs_cap];

inline static void globs_init(){}

inline static void globs_free(){}

inline static void globs_render(){
	glob*g=__globs;
	int n=globs_cap;
	while(n--){
		glob_render(g++);
	}
}

inline static void globs_render_id(size_t id){
	glob*sr=(glob*)&__globs[id];//? bounds
	shader_render_triangle_array(
			sr->vbufid,
			sr->vbufn,
			sr->texbufid,
			sr->mtx_mw
	);
}

inline static glob*globs_get(size_t id){
	return&__globs[id];//? bounds
}
