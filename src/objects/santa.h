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

inline static void update_santa(object*this,dt dt){
	float t=this->time_in_seconds-dt;
	if(t<0){ // next frame
		this->texture_id++;
		if(this->texture_id>35)
			this->texture_id=20;
		this->time_in_seconds+=.2f;
	}else{
		this->time_in_seconds-=dt;
	}
}

//---------------------------------------------------------------------------

static object default_santa={
		.type={{'f',0,0,0,0,0,0,0}},
		.texture_id=20,
		.init=init_santa,
		.update=update_santa,
		.render=draw_texture_and_bounding_sphere,
};

//---------------------------------------------------------------------------
