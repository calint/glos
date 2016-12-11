#pragma once
//---------------------------------------------------------------------- ninja
#include"../obj.h"
#include"ninja_part.h"
//----------------------------------------------------------------------- init
static object ninja_def={
	._node=node_def,
	.bvol={1.4f,{1,1,1,0}},
	.position={0,0,0,0},
	.velocity={0,0,0,0},
	.angle={0,0,0,0},
	.angular_velocity={0,0,0,0},
	.type={{'a',0,0,0,0,0,0,0}},
	.ptr_to_bits=NULL,
	.init=NULL,
	.update=NULL,
	.collision=NULL,
	.render=object_render_glob,
	.free=NULL,
	.part={NULL,NULL,NULL,NULL}
};
//----------------------------------------------------------------------------
