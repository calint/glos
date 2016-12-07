#pragma once
//---------------------------------------------------------------------- santa
#include"../object.h"
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
	.update=_santa_update_,
	.collision=NULL,
	.render=object_render_glob,
	.free=NULL,
	.part={NULL,NULL,NULL,NULL}
};
//------------------------------------------------------------------------ init
inline static void _santa_init_(object*this){
	this->scale=(scale){.5,.5,.5,0};
	this->angular_velocity.z=90;
	this->velocity.x=.5f;
	object_update_bounding_radius_using_scale(this);
}
//---------------------------------------------------------------------- update
inline static void _santa_update_(object*this){
//	printf(" * new %-12s [ %4s %p ]\n","santa",this->type.path,this);
	_object_init_(this);

	this->scale=(scale){.5,.5,.5,0};
	this->angular_velocity.z=90;
	this->velocity.x=.5f;
	object_update_bounding_radius_using_scale(this);
}
//----------------------------------------------------------------------------
