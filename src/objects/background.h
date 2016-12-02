#pragma once
#include "../object.h"
#include "../parts/anim.h"

//----------------------------------------------------------------------- init

inline static void init_background(object*this) {
	this->position=(position){
			100,
			(float)(window_height-texture[10].height),
			0,0
		};
	this->position=(position){
			(float)(texture[1].width-window_width),
			(float)(window_height-texture[1].height),
			0,0
		};

	this->velocity=(position){-7,7,0,0};

	this->scale=(scale){
			(float)texture[1].width,
			(float)texture[1].height,
			1,0
		};

	set_bounding_radius_from_xy_scale(this);
}

inline static void update_background(object*this,dt dt){

	this->scale.x=this->scale.x-7*dt;
	this->scale.y=this->scale.y-7*dt;

	set_bounding_radius_from_xy_scale(this);
}
//---------------------------------------------------------------------------

static object default_background={
		.type={{'f',0,0,0,0,0,0,0}},
		.texture_id=1,
		.init=init_background,
		.update=update_background,
		.render=draw_texture,
};

//---------------------------------------------------------------------------
