#pragma once
//---------------------------------------------------------------------- santa
#include"../lib.h"
//------------------------------------------------------------------------ def
static object santa_def={
	.position={0,0,0,0},
	.velocity={0,0,0,0},
	.angle={0,0,0,0},
	.angular_velocity={0,0,0,0},
	.bounding_radius=0,
	.scale={0,0,0,0},
	.type={{'a',0,0,0,0,0,0,0}},
	.ptr_to_bits=NULL,
	.glob_id=1,
	.matrix_model_to_local={1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1},
	.init=_santa_init_,
	.update=NULL,
	.collision=NULL,
	.render=object_render_glob,
	.free=NULL,
	.part={NULL,NULL,NULL,NULL}
};
//------------------------------------------------------------------------ init
inline static void _santa_init_(object*this){
	this->bvol.s=(scale){.5,.5,.5,0};
	this->phy.av.z=90;
	this->phy.p.x=.5f;
	bvol_update_radius_using_scale(&this->bvol);
}
