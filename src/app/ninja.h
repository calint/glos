#pragma once
//---------------------------------------------------------------------- ninja
#include"../obj.h"
//------------------------------------------------------------------------ def
static object ninja_def={
	.n=node_def,
	.b={	.r=1.4f,
			.s={1,1,1,0}},
	.p={	.p={0,0,0,0},
			.v={0,0,0,0},
			.a={0,0,0,0},
			.av={0,0,0,0},},
	.v={	.init=NULL,
			.update=NULL,
			.collision=NULL,
			.render=object_render_glob,
			.free=NULL,
	},
	.t={{'a',0,0,0,0,0,0,0}},
	.part={NULL,NULL,NULL,NULL},
	.ptr_to_bits=NULL,
};
//----------------------------------------------------------------------------
