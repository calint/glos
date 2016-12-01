#pragma once
#include "../object.h"

//--------------------------------------------------------------------- logo_1

#define santa_texture_on_ground_bias 131

inline static void init_santa(object*this) {
	this->position=(position){
			(float)(window_width-texture[this->texture_id].width/2),
			(float)(window_height-texture[this->texture_id].height
						+santa_texture_on_ground_bias
					),
			0,0
		};

	this->scale=(scale){
			(float)texture[this->texture_id].width,
			(float)texture[this->texture_id].height,
			1,0
		};

	set_bounding_radius_from_xy_scale(this);
}

//---------------------------------------------------------------------------

static object default_santa={
		.type={{'f',0,0,0,0,0,0,0}},
		.init=init_santa,
		.texture_id=20,
		.render=draw_texture_and_bounding_sphere,
};

//---------------------------------------------------------------------------
