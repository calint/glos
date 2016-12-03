#pragma once
#include "../object.h"
#include "../parts/animator_part.h"

//----------------------------------------------------------------------- init

inline static void init_ninja(object*this) {
	this->scale=(scale){.5,.5,.5,0};
	this->angular_velocity.z=90;
//	this->velocity.x=.5f;
	update_bounding_radius_using_scale(this);
}

//----------------------------------------------------------------------update

inline static void update_ninja(object*this,dt dt){
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

//---------------------------------------------------------------------------

static object default_ninja={
	.type={{'e',0,0,0,0,0,0,0}},
	.init=init_ninja,
	.drawable_id=1,
	.update=update_ninja,
	.render=render_drawable,
};

//---------------------------------------------------------------------------
