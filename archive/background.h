#pragma once
#include "../object.h"
#include "../parts/animator_part.h"

//----------------------------------------------------------------------- init

inline static void init_background(object*this){
	this->position=(position){
		100,
		(float)(window_height-texture[10].height),
		0,0
	};

	this->position=(position){
		(float)(texture[this->texture_id].width-window_width),
		(float)(window_height-texture[this->texture_id].height+100),
		0,0
	};

	this->velocity=(position){-17,+43,0,0};

	this->scale=(scale){
		(float)texture[this->texture_id].width,
		(float)texture[this->texture_id].height,
		1,0
	};

	object_update_bounding_radius_using_scale(this);
}

inline static void update_background(object*this,dt dt){
//
	this->scale.x=this->scale.x-17*dt;
	this->scale.y=this->scale.y+17*dt;
//
//	update_bounding_radius_using_scale(this);
}

//---------------------------------------------------------------------------

static object default_background={
	.type={{'f',0,0,0,0,0,0,0}},
	.texture_id=1,
	.init=init_background,
	.update=update_background,
	.render=_draw_texture_,
};

//---------------------------------------------------------------------------
