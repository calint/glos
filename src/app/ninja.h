#pragma once
//---------------------------------------------------------------------- ninja
#include"../obj.h"
#include"ninja_part.h"
//----------------------------------------------------------------------- init
static object ninja_def={
	.position={0,0,0,0},
	.velocity={0,0,0,0},
	.angle={0,0,0,0},
	.angular_velocity={0,0,0,0},
	.scale={1,1,1,0},
	.bounding_radius=1.4f,
	.type={{'a',0,0,0,0,0,0,0}},
	.ptr_to_bits=NULL,
	.glo=NULL,
	.matrix_vertices_model_to_world={1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1},
	.init=NULL,
	.update=NULL,
	.collision=NULL,
	.render=object_render_glob,
	.free=NULL,
	.part={NULL,NULL,NULL,NULL}
};
//----------------------------------------------------------------------------
