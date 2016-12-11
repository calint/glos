#pragma once
//---------------------------------------------------------------------- ninja
#include"../obj.h"
#include"ninja_part.h"
//----------------------------------------------------------------------- init
static object ninja_def={
	.node=node_def,
	.bvol={
			.bounding_radius=1.4f,
			.scale={1,1,1,0}
	},
	.phy=phy_def,
	.type={{'a',0,0,0,0,0,0,0}},
	.ptr_to_bits=NULL,
	.vtbl={
		.init=NULL,
		.update=NULL,
		.collision=NULL,
		.render=object_render_glob,
		.free=NULL,
	},
	.part={NULL,NULL,NULL,NULL}
};
//----------------------------------------------------------------------------
