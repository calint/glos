#pragma once
//---------------------------------------------------------------------- ninja
#include"../obj.h"
#include"ninja_part.h"
//------------------------------------------------------------------ overrides
inline static void _ninja_init_(object*this);
inline static void _ninja_free_(object*this);
//----------------------------------------------------------------------- init
static object ninja_def={
//----------------------------------------------------------------------------
	.position={0,0,0,0},
	.velocity={0,0,0,0},
	.angle={0,0,0,0},
	.angular_velocity={0,0,0,0},
	.bounding_radius=0,
	.scale={1,1,1,0},
	.type={{'a',0,0,0,0,0,0,0}},
	.ptr_to_bits=NULL,
	.glo=NULL,
	.model_to_world_matrix={1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1},
	.init=NULL,
//	.init=_ninja_init_,
	.update=NULL,
//	.update=object_update,
	.collision=NULL,
	.render=object_render_glob,
//	.free=_ninja_free_,
	.free=NULL,
	.part={NULL,NULL,NULL,NULL}
};
//----------------------------------------------------------------------------
inline static void _ninja_init_(object*this){
//	printf(" * new %-12s [ %4s %p ]\n","ninja",this->type.path,this);
//	this->scale=(scale){1,1,1,0};
//	this->angular_velocity.z=90;
//	this->velocity.x=.5f;
	object_update_bounding_radius_using_scale(this);

//	this->part[0]=&ninja_part_def;
//
//	((ninja_part*)(this->part[0]))->part.init(
//			this,
//			&((ninja_part*)(this->part[0]))->part
//		);
}
//----------------------------------------------------------------------------
//inline static void _ninja_free_(object*this){
////	printf(" * del %-12s [ %4s %p ]\n","ninja",this->type.path,this);
//	_object_free_(this);
//
//	part*p=this->part[0];
//	p->free(this,p);
//}
//----------------------------------------------------------------------------
