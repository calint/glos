#pragma once
#include"glob.h"
#include"mat4.h"
//--------------------------------------------------------------------- storage
#define globs_cap 16
#define globs_assert_bounds
static glob __globs[globs_cap];

inline static void _globs_assert_index_(arrayix i){
#ifdef globs_assert_bounds
	if(i>=globs_cap){
		fprintf(stderr,"\nout of bounds\n");
		fprintf(stderr,"\n    %s %d\n",__FILE__,__LINE__);
		exit(-1);
	}
#endif
}
//---------------------------------------------------------------------

inline static glob*globs_at(arrayix i){
	_globs_assert_index_(i);
	return&__globs[i];
}

inline static const glob*globs_at_const(arrayix i){
	_globs_assert_index_(i);
	return&__globs[i];
}

inline static void globs_init(){}
inline static void globs_free(){}
inline static void globs_render(){
	glob*g=__globs;
	int n=globs_cap;
	while(n--){
		if(g->vbuf)
			glob_render(g++,mat4_ident);
	}
}


inline static void globs_load_obj_file(arrayix i,const char*path){
	_globs_assert_index_(i);
	glob_load_obj_file(&__globs[i],path);
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

