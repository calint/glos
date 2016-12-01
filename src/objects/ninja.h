#pragma once
#include "../object.h"

//--------------------------------------------------------------------- logo_1

inline static void init_ninja(object*this) {
	this->position = (position){
			100,
			(float)(window_height-texture[10].height),
			0,0
		};

	this->velocity=(position){5,10,0,0};

	this->scale=(scale){
			(float)texture[10].width,
			(float)texture[10].height,
			1,0
		};

	set_bounding_radius_from_xy_scale(this);
}

//---------------------------------------------------------------------- update

inline static void update_ninja(object*this,dt dt){
	float t=this->time_in_seconds-dt;

	if(t<0){ // next frame
		this->texture_id++;
		if(this->texture_id>19)
			this->texture_id=10;
		this->time_in_seconds+=.2f;
	}else{
		this->time_in_seconds-=dt;
	}

//	printf(" ninja y: %f\n",this->position.y);
	if(this->position.y>620){
		this->velocity.y=-this->velocity.y;
	}else if(this->position.y<600){
		this->velocity.y=-this->velocity.y;
	}
}

//---------------------------------------------------------------------------

static object default_ninja={
		.type={{'e',0,0,0,0,0,0,0}},
		.init=init_ninja,
		.texture_id=10,
		.update=update_ninja,
		.render=draw_texture_and_bounding_sphere,
};

//---------------------------------------------------------------------------
