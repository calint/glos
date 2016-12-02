#pragma once
#include "../object.h"
#include "../parts/animator_part.h"

//----------------------------------------------------------------------- init

inline static void init_snowman(object*this){
	this->position=(position){
		700,
		600,
		0,0
	};

	this->scale=(scale){
		(float)texture[this->texture_id].width,
		(float)texture[this->texture_id].height,
		1,0
	};

	update_bounding_radius_using_scale(this);
}

//--------------------------------------------------------------------- update

inline static void update_snowman(object*this,dt dt){
}
//---------------------------------------------------------------------------

static object snowman={
	.type={{'f',0,0,0,0,0,0,0}},
	.texture_id=2,
	.init=init_snowman,
	.update=update_snowman,
	.render=draw_texture_and_bounding_sphere,
};

//---------------------------------------------------------------------------
