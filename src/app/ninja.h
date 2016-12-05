//---------------------------------------------------------------------- ninja
#pragma once
#include"../object.h"
#include"ninja_part.h"
//------------------------------------------------------------------ overrides
inline static void _ninja_free_(object*this);
//----------------------------------------------------------------------- init
inline static void _ninja_init_(object*this);static object ninja_def={
//-----------------------------------------------------------------------
	.position={0,0,0,0},
	.velocity={0,0,0,0},
	.angle={0,0,0,0},
	.angular_velocity={0,0,0,0},
	.bounding_radius=0,
	.scale={0,0,0,0},
	.type={{'e',0,0,0,0,0,0,0}},
	.id=0,
	.color=0,
	.bits_ref=NULL,
	.glob_id=1,
	.model_to_world_matrix={1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1},
	.init=_ninja_init_,
	.update=_object_update_,
	.collision=_object_collision_,
	.render=_render_glob_,
	.free=_ninja_free_,
	.part={NULL,NULL,NULL,NULL}
};inline static void _ninja_init_(object*this){
//	printf(" * new %-12s [ %4s %p ]\n","ninja",this->type.path,this);
	_object_init_(this);

	this->scale=(scale){.5,.5,.5,0};
	this->angular_velocity.z=90;
	this->velocity.x=.5f;
	object_update_bounding_radius_using_scale(this);

	this->part[0]=&_ninja_part_;

	((ninja_part*)(this->part[0]))->part.init(
			this,
			&((ninja_part*)(this->part[0]))->part
		);
}
//---------------------------------------------------------------------------
inline static void _ninja_free_(object*this){
//	printf(" * del %-12s [ %4s %p ]\n","ninja",this->type.path,this);
	_object_free_(this);

	((ninja_part*)(this->part[0]))->part.free(
			this,
			&((ninja_part*)(this->part[0]))->part
		);
}
//---------------------------------------------------------------------------
