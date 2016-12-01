#pragma once
#include "../object.h"

//--------------------------------------------------------------------- logo_1

inline static void init_ninja(object*this) {
	this->position = (position){
			100,
			(float)(window_height-texture[10].height),
			0,0
		};

	this->scale=(scale){
			(float)texture[10].width,
			(float)texture[10].height,
			1,0
		};

	set_bounding_radius_from_xy_scale(this);
}

//---------------------------------------------------------------------------

static object default_ninja={
		.type={{'e',0,0,0,0,0,0,0}},
		.init=init_ninja,
		.texture_id=10,
		.render=draw_texture_and_bounding_sphere,
};

//---------------------------------------------------------------------------
