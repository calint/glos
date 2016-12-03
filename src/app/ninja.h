#pragma once
#include "../object.h"
#include "../parts/animator_part.h"

//----------------------------------------------------------------------- init

inline static void init_ninja(object*this) {
	this->scale=(scale){1,1,1,0};
	this->angular_velocity.z=90;
	update_bounding_radius_using_scale(this);
}

//----------------------------------------------------------------------update

inline static void update_ninja(object*this,dt dt){
//	object_update_physics(this,dt);
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
