#pragma once
#include "../object.h"

//--------------------------------------------------------------------- extends

typedef struct ninja{

	float animation_time_left_for_current_frame;

}ninja;

//----------------------------------------------------------------------- init

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

	ninja*ni=malloc(sizeof(ninja));
	ni->animation_time_left_for_current_frame=0.5f;
	this->extended_by=ni;
}

//----------------------------------------------------------------------- free

inline static void free_ninja(object*this){
	free(this->extended_by);
}

//----------------------------------------------------------------------update

inline static void update_ninja(object*this,dt dt){
	ninja*ext=(ninja*)this->extended_by;

	float t=ext->animation_time_left_for_current_frame-dt;

	if(t<0){ // next frame
		this->texture_id++;
		if(this->texture_id>19)
			this->texture_id=10;

		ext->animation_time_left_for_current_frame+=.2f;
	}else{
		ext->animation_time_left_for_current_frame+=.2f;
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
