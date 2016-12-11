#pragma once
//---------------------------------------------------------------------- ninja
#include"../obj.h"
#include"ninja_part.h"
//----------------------------------------------------------------------- init
static object ninja_def={
	.node=node_def,
	.bvol={	.bounding_radius=1.4f,
			.scale={1,1,1,0}},
	.phy={	.position=vec4_def,
			.velocity=vec4_def,
			.angle=vec4_def,
			.angular_velocity=vec4_def,},.
	vtbl={	.init=NULL,
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
