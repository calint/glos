#pragma once
#include "../object.h"
#include "../parts/anim.h"

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

	anim*a=malloc(sizeof(anim));
	*a=default_anim;
	a->animation_time_left_for_current_frame=0.5f;
	a->current_index_in_textures=a->texture_index_for_first_frame=10;
	a->texture_index_for_last_frame=19;
	a->time_duration_per_frame=.2f;
	this->extended=a;
}

//----------------------------------------------------------------------- free

inline static void free_ninja(object*this){
	free(this->extended);
}

//----------------------------------------------------------------------update

inline static void update_ninja(object*this,dt dt){
//	printf(" ninja y: %f\n",this->position.y);
	if(this->position.y>620){
		this->velocity.y=-this->velocity.y;
	}else if(this->position.y<600){
		this->velocity.y=-this->velocity.y;
	}

	anim*a=(anim*)this->extended;
	update_anim(a,dt);
	this->texture_id=a->current_index_in_textures;
}

//---------------------------------------------------------------------------

static object default_ninja={
		.type={{'e',0,0,0,0,0,0,0}},
		.init=init_ninja,
		.free=free_ninja,
		.texture_id=10,
		.update=update_ninja,
		.render=draw_texture_and_bounding_sphere,
};

//---------------------------------------------------------------------------
