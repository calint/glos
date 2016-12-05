#pragma once
#include"glob.h"
#include"mat4.h"
#define globs_cap 2
#define globs_bound_check 0
//---------------------------------------------------------------------

static glob globs[globs_cap];

inline static glob*globs_ref(size_t id){
	return&globs[id];
}

inline static const glob*globs_ro(size_t id){
	return&globs[id];
}

inline static void globs_init(){}
inline static void globs_free(){}
inline static void globs_render(){
	glob*g=globs;
	int n=globs_cap;
	while(n--){
		if(g->vbuf)
			glob_render(g++,mat4_ident);
	}
}


inline static void globs_load_obj_file(size_t ix,const char*path){
	glob_load_obj_file(&globs[ix],path);
}

//inline static void globs_render_id(size_t id,float*mat4_model_to_world){
//	glob*sr=globs_ref(id);
//	if(!sr->vbufid)
//		return;
//	shader_render_triangle_array(
//			sr->vbufid,
//			sr->vbufn,
//			sr->texbufid,
//			mat4_model_to_world
//	);
//}

