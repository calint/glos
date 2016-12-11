#pragma once
//---------------------------------------------------------------------- ninja
#include"../obj.h"
#include"ninja_part.h"
//----------------------------------------------------------------------- init
static object ninja_def={
	.node=node_def,
	.bvol={	.bounding_radius=1.4f,
			.scale={1,1,1,0}},
	.phy={	.position={0,0,0,0},
			.velocity={0,0,0,0},
			.angle={0,0,0,0},
			.angular_velocity={0,0,0,0},},
	.vtbl={	.init=NULL,
			.update=NULL,
			.collision=NULL,
			.render=object_render_glob,
			.free=NULL,
	},
	.type={{'a',0,0,0,0,0,0,0}},
	.part={NULL,NULL,NULL,NULL},
	.ptr_to_bits=NULL,
};
//----------------------------------------------------------------------------
