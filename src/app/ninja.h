#pragma once
#include "../object.h"
#include "ninja_animator_part.h"
//----------------------------------------------------------------------- init
inline static void _ninja_init_(object*this) {
	_object_init_(this);

	this->scale=(scale){.5,.5,.5,0};
	this->angular_velocity.z=90;
//	this->velocity.x=.5f;
	update_bounding_radius_using_scale(this);

	this->part[0]=&default_ninja_animator_part;
}
//----------------------------------------------------------------------update
inline static void _ninja_update_(object*this,dt dt){
	object_update(this,dt);

	if(this->position.x>1){
		this->position.x=1;
		this->velocity.x=-this->velocity.x;
		this->angular_velocity.z=-this->angular_velocity.z;
	}else if(this->position.x<-1){
		this->position.x=-1;
		this->velocity.x=-this->velocity.x;
		this->angular_velocity.z=-this->angular_velocity.z;
	}
}
//----------------------------------------------------------------------- init
inline static void _ninja_free_(object*this) {
	_object_free_(this);
}
//-------------------------------------------------------------------- default
static object default_ninja={
	.position={0,0,0,0},
	.velocity={0,0,0,0},
	.angle={0,0,0,0},
	.angular_velocity={0,0,0,0},
	.bounding_radius=0,
	.scale={0,0,0,0},
	.type={{'e',0,0,0,0,0,0,0}},
	.id=0,
	.texture_id=0,
	.color=0,
	.bits_ref=NULL,
	.drawable_id=1,
	.model_to_world_matrix={1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1},
	.init=_ninja_init_,
	.update=_ninja_update_,
	.collision=NULL,
	.render=render_drawable,
	.free=_ninja_free_,
	.part={NULL,NULL,NULL,NULL}
};
//---------------------------------------------------------------------------
